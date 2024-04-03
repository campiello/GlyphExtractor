// Copyright(c) <2024 > <Wilhelm Kurz>, see License.txt

#include <algorithm>
#include "GlyphExtractor.h"


GlyphExtractor::GlyphExtractor()
:BmpReader()
{}


static void printPixelSymbols(const uint8_t& a, FILE* f, uint32_t& pixelno, const uint32_t& charwidth)
{
    uint8_t mask{ 0x80 };

    for (uint8_t i = 0; i < 8; i++)
    {
        if (a & mask)
            fprintf(f, "x");
        else
            fprintf(f, ".");

        printf("%c", (a & mask ? 'x' : '.'));
        mask >>= 1;
        pixelno++;
        if (pixelno >= charwidth)
        {
            pixelno = 0;
            printf("\n");
            fprintf(f, "\n");
            break;
        }
    }
}

static void printchar(FILE* f, const std::vector<uint8_t>& v, uint32_t charwidth, bool flat, bool littleendian)
{
    uint32_t pixelno{ 0 };
    if (!flat)
        fprintf(f, "{\n");
    size_t bytes = ((size_t)charwidth + 7) / 8;
    for (size_t b = 0; b < v.size(); b += bytes)
    {
        for (size_t j = 0; j < bytes; j++)
        {
            size_t k = littleendian ? bytes - 1 - j : j;
            fprintf(f, "0x%02x, ", v[b + k]);
        }

        fprintf(f, " // ");

        for (size_t j = 0; j < bytes; j++)
        {
            uint8_t a = v[b + j];
            printPixelSymbols(a, f, pixelno, charwidth);
        }
    }
    if (!flat)
        fprintf(f, "},");
    fprintf(f, "\n");
    printf("\n");
}


/* This function shifts an array of byte of size len by shft number of
bits to the left. Assumes array is big endian. */
// see https://stackoverflow.com/questions/2773890/efficient-bitshifting-an-array-of-int
// posted by https://stackoverflow.com/users/1698972/p8me

#define ARR_TYPE uint8_t

static std::vector<ARR_TYPE> ShiftLeft(const std::vector<ARR_TYPE>& arr_in, int shft)
{
    if (shft == 0)
        return arr_in;

    const size_t int_n_bits = sizeof(ARR_TYPE) * 8;
    const size_t arr_len = arr_in.size();
    std::vector<ARR_TYPE> arr_out;
    arr_out.resize(arr_len);

    int msb_shifts = shft % int_n_bits;
    int lsb_shifts = int_n_bits - msb_shifts;
    int byte_shft = shft / int_n_bits;
    int last_byt = (int)arr_len - byte_shft - 1;
    for (int i = 0; i < arr_len; i++)
    {
        if (i <= last_byt)
        {
            int msb_idx = i + byte_shft;
            arr_out[i] = arr_in[msb_idx] << msb_shifts;
            if (i != last_byt)
                arr_out[i] |= arr_in[msb_idx + 1] >> lsb_shifts;
        }
        else
        {
            arr_out[i] = 0;
        }
    }

    return arr_out;
}


void GlyphExtractor::computeEmptyHlines()
{
    for (int32_t i = 0; i < pixelheight(); i++)
        m_HlineEmpty.push_back(isEmptyHLine(i));
}

void GlyphExtractor::computeEmptyVlines()
{
    for (int32_t i = 0; i < bytesPerLine() * 8; i++)
        m_VlineEmpty.push_back(isEmptyVLine(i));
}


static bool isEmpty(const std::vector<uint8_t>& v) 
{
    return std::all_of(v.cbegin(), v.cend(), [](int a) { return a == 0; });
}

bool GlyphExtractor::isEmptyHLine(int32_t lineIdx) const
{
    if (lineIdx >= pixelheight())
        return false;

    int y = pixelheight() - 1 - lineIdx;
    std::vector<uint8_t> line(Data().begin() + bytesPerLine() * y, Data().begin() + bytesPerLine() * (y + 1));
    return isEmpty(line);
}



bool GlyphExtractor::isEmptyVLine(uint32_t pixelidx) const
{
    int32_t byteIdx = pixelidx / 8;
    int32_t pixelinByte = pixelidx - byteIdx * 8;
    uint8_t bitmask = 0x80 >> pixelinByte;
    if (byteIdx >= bytesPerLine())
        return false;

    for (int y = pixelheight() - 1; y >= 0; y--)
    {
        std::vector<uint8_t> line(Data().begin() + bytesPerLine() * y, Data().begin() + bytesPerLine() * (y + 1));
        if (line[byteIdx] & bitmask)
            return false;
    }
    return true;
}


static void getNumPeriodsAndWidth(const std::vector<bool>& emptyLines, size_t& numperiods, uint32_t& maxwidth)
{
    enum class tState { startState, inGap, inChar };
    tState state = tState::startState;
    maxwidth = 0;
    numperiods = 0;
    uint32_t startidx = 0;

    for (uint32_t i = 0; i < emptyLines.size(); i++)
    {
        if (emptyLines[i])
        {
            switch (state)
            {
            case tState::inChar:        // end of character 
            {
                uint32_t width = i - startidx;
                maxwidth = (width > maxwidth) ? width : maxwidth;
                state = tState::inGap;
                numperiods++;
                break;
            }

            case tState::inGap: // continue in empty
                break;

            default:
                state = tState::inGap;
                break;
            }
        }
        else
        {
            switch (state)
            {
            case tState::inChar: // continue in char
                break;

            case tState::inGap: // end of pause
                startidx = i;
                state = tState::inChar;
                break;

            default:
                state = tState::inChar;
                startidx = i;
                break;
            }
        }
    }
    if (state == tState::inChar)
    {
        numperiods++;
        uint32_t width = (uint32_t)emptyLines.size()-1 - startidx;
        if (width > maxwidth)
            maxwidth = width;
    }

}

static bool findRasterAndFirstLine(const std::vector<bool>& emptyLine, int32_t& io_firstLine, int32_t& io_raster, size_t numrows)
{
    if (numrows <= 1)
        return true;

    bool bFound{ false };
    int32_t raster{ io_raster + 10 };
    int32_t fl;
    for (; raster >= io_raster - 10 && raster > 4; raster--)
    {
        fl = io_firstLine + 3;
        for (; fl >= io_firstLine - 3 && fl >= 0; fl--)
        {
            for (size_t row = 1; row < numrows - 1; row++)
            {
                bFound = true;
                if (!emptyLine[fl - 1 + row * raster])
                {
                    bFound = false;
                    break;
                }
            }
            if (bFound)
                break;
        }
        if (bFound)
            break;
    }
    if (!bFound)
    {
        printf("no row spacing found\n");
        return false;
    }

    io_firstLine = fl;
    io_raster = raster;

    return true;
}


bool GlyphExtractor::findVDimensions(uint32_t& charheight, uint32_t& rowspacing, size_t& numrows)
{
    getNumPeriodsAndWidth(m_HlineEmpty, numrows, charheight);
    int32_t rasterY = (m_lastHline - m_firstHline) / (int32_t)numrows;
    if (!findRasterAndFirstLine(m_HlineEmpty, m_firstHline, rasterY, numrows))
        return false;

    rowspacing = rasterY - charheight;
    return true;
}


bool GlyphExtractor::findHDimensions(uint32_t& charwidth, uint32_t& colspacing, size_t& numcolumns)
{
    getNumPeriodsAndWidth(m_VlineEmpty, numcolumns, charwidth);
    int32_t rasterX = (m_lastVline - m_firstVline) / (int32_t)numcolumns;

    if (!findRasterAndFirstLine(m_VlineEmpty, m_firstVline, rasterX, numcolumns))
        return false;

    colspacing = rasterX - charwidth;
    return true;
}


static bool isInverted(const std::vector<uint8_t>& data)
{
    uint64_t sum{ 0 };
    for (auto a : data)
    {
        uint8_t mask = 0x01;
        for (uint32_t i = 0; i < 8; i++)
        {
            if (a & mask)
                sum++;
            mask <<= 1;
        }
    }
    return  sum > data.size() * 8 / 2;
}


bool GlyphExtractor::writeCArray(const char* fname_out, const WriteArgs& args)

{
    bool invert{ false };
    if (args.invert > 0)
        invert = args.invert;
    else
        invert = isInverted(Data());

    if (invert)
        invertBitmap();


    computeEmptyHlines();
    computeEmptyVlines();

    m_firstHline = 0;
    while (m_firstHline < pixelheight() && m_HlineEmpty[m_firstHline])
        m_firstHline++;

    m_lastHline = pixelheight() - 1;
    while (m_lastHline > 0 && m_HlineEmpty[m_lastHline])
        m_lastHline--;

    m_firstVline = 0; // first non empty vertical line
    while (m_firstVline < bytesPerLine() * 8 && m_VlineEmpty[m_firstVline])
        m_firstVline++;

    m_lastVline = bytesPerLine() * 8 - 1;
    while (m_lastVline > 0 && m_VlineEmpty[m_lastVline])
        m_lastVline--;


    uint32_t rowspacing;
    size_t numrows;
    uint32_t charheight;
    if (!args.charheight || !args.rowspacing || !args.numrows)
    {
        if (!findVDimensions(charheight, rowspacing, numrows))
            return false;
    }
    if (args.charheight)
        charheight = args.charheight;

    if (args.rowspacing)
        rowspacing = args.rowspacing;

    if (args.numrows)
        numrows = args.numrows;


    uint32_t charwidth;
    uint32_t colspacing;
    size_t numcolumns;
    if (!args.charwidth || !args.colspacing || !args.numcolumns)
    {
        if (!findHDimensions(charwidth, colspacing, numcolumns))
            return false;
    }

    if (args.charwidth)
        charwidth = args.charwidth;

    if (args.colspacing)
        colspacing = args.colspacing;
    
    if (args.numcolumns)
        numcolumns = args.numcolumns;

    FILE* of = fopen(fname_out, "w");
    if (!of)
    {
        printf("Unable to open the output ile.");
        return false;
    }

    std::string sArrayName{m_fname};

    auto const pos = sArrayName.find_last_of('.');
    if (pos != std::string::npos)
        sArrayName = sArrayName.substr(0, pos);

    fprintf(of, "// font created from %s\n", m_fname.c_str());
    fprintf(of, "// %dx%d %s\n\n", charwidth, charheight, (args.littleendian ? "LE" : "BE"));
    fprintf(of, "const uint8_t %s_%dx%d[] = {\n", sArrayName.c_str(), charwidth, charheight);

    uint32_t numBytesPerChar = (charwidth + 7) / 8;

    uint32_t mask32 = (1 << (numBytesPerChar * 8 - charwidth)) - 1;
    mask32 = ~mask32;
    uint8_t mask8 = mask32 & 0xff;

    std::vector<std::vector<uint8_t>> characters;
    characters.resize(numcolumns);
    uint32_t charlineIdx{ 0 };
    uint32_t row{ 0 };

    for (int32_t lineIdx = m_firstHline; lineIdx < pixelheight(); )
    {
        int y = pixelheight() - 1 - lineIdx;
        std::vector<uint8_t> line(Data().begin() + bytesPerLine() * y, Data().begin() + bytesPerLine() * (y + 1));
        line = ShiftLeft(line, m_firstVline);

        for (uint32_t cNo = 0; cNo < numcolumns; cNo++)
        {
            std::vector<uint8_t>& character = characters[cNo];
            character.insert(std::end(character), line.begin(), line.begin() + numBytesPerChar);
            character[character.size() - 1] &= mask8;

            line = ShiftLeft(line, charwidth + colspacing);
        }

        charlineIdx++;
        lineIdx++;
        if (charlineIdx >= charheight)
        {
            row++;
            lineIdx += rowspacing;
            for (uint32_t cNo = 0; cNo < numcolumns; cNo++)
            {
                if (row >= numrows && isEmpty(characters[cNo]))
                    break;
                printchar(of, characters[cNo], charwidth, args.flat, args.littleendian);
                characters[cNo].clear();
                printf("\n");
            }
            charlineIdx = 0;
        }
        if(row >= numrows)
            break;
    }
    fprintf(of, "};\n");
    fclose(of);
    return true;
}


