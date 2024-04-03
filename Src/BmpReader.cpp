// Copyright(c) <2024 > <Wilhelm Kurz>, see License.txt

#include "BmpReader.h"


#pragma pack(push, 1)
struct BITMAPFILEHEADER
{
    uint16_t   bfType;
    uint32_t   bfSize;
    uint16_t   bfReserved1;
    uint16_t   bfReserved2;
    uint32_t   bfOffBits;
};

struct BITMAPINFOHEADER
{
    uint32_t      biSize{ sizeof(BITMAPINFOHEADER) };
    int32_t       biWidth;
    int32_t       biHeight;
    uint16_t      biPlanes;
    uint16_t      biBitCount;
    uint32_t      biCompression;
    uint32_t      biSizeImage;
    int32_t       biXPelsPerMeter;
    int32_t       biYPelsPerMeter;
    uint32_t      biClrUsed;
    uint32_t      biClrImportant;
};
#pragma pack(pop)


bool BmpReader::read(const char* fname)
{
    m_fname = fname;
    FILE* f = fopen(fname, "rb");

    if (!f) 
    {
        printf("Cannot open file %s.", fname);
        return false;
    }

    BITMAPFILEHEADER bitmapfileheader{};
    BITMAPINFOHEADER bitmapinfoheader{};
    bitmapfileheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    fread(&bitmapfileheader, sizeof(BITMAPFILEHEADER), 1, f);
    if (bitmapfileheader.bfType != 0x4D42)
    {
        printf("Illegal file format.");
        fclose(f);
        return false;
    }
    fread(&bitmapinfoheader, sizeof(BITMAPINFOHEADER), 1, f);

    if (bitmapinfoheader.biBitCount != 1)
    {
        printf("Illegal file format. Must be 1bit/pixel file.");
        fclose(f);
        return false;
    }
    fseek(f, bitmapfileheader.bfOffBits, 0);

    if (bitmapinfoheader.biHeight < 0)
    {
        printf("Illegal file format.");
        fclose(f);
        return false;
    }

    m_pixelheight = bitmapinfoheader.biHeight;
    m_bytesPerLine = (bitmapinfoheader.biWidth * bitmapinfoheader.biBitCount + 7) / 8;
    int32_t paddingBytes = (4 - (m_bytesPerLine % 4)) % 4;
    m_bytesPerLine += paddingBytes;

    m_data.resize(bitmapinfoheader.biHeight * m_bytesPerLine);
    for (int y = 0; y < bitmapinfoheader.biHeight; ++y)
    {
        fread(m_data.data() + m_bytesPerLine * y, 1, m_bytesPerLine, f);
    }
    fclose(f);
    return true;
}


void BmpReader::invertBitmap()
{
    for (auto& a : m_data)
        a = ~a;
}



