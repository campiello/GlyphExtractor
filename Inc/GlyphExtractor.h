// Copyright(c) <2024 > <Wilhelm Kurz>, see License.txt

#ifndef GLYPHEXTRACTOR_H_
#define GLYPHEXTRACTOR_H_

#include "BmpReader.h"

class GlyphExtractor : private BmpReader
{
public:
    GlyphExtractor();

    bool read(const char* fname) { return BmpReader::read(fname);}

    struct WriteArgs
    {
        int32_t charheight{ 0 };
        int32_t rowspacing{ 0 };
        int32_t numrows{ 0 };
        int32_t charwidth{ 0 };
        int32_t colspacing{ 0 };
        int32_t numcolumns{ 0 };
        int32_t flat{ 1 };
        int32_t invert{ -1 };
        int32_t littleendian{ 1 };
    };

    bool writeCArray(const char* fname_out, const WriteArgs& args);


private:
    void computeEmptyHlines();
    void computeEmptyVlines();
    bool isEmptyHLine(int32_t lineIdx) const;
    bool isEmptyVLine(uint32_t pixelidx) const;
    bool findVDimensions(uint32_t& charheight, uint32_t& rowspacing, size_t& numrows);
    bool findHDimensions(uint32_t& charwidth, uint32_t& colspacing, size_t& numcolumns);

    int32_t m_firstHline{ 0 };
    int32_t m_lastHline{ 0 };
    int32_t m_firstVline{ 0 };
    int32_t m_lastVline{ 0 };
    std::vector<bool> m_HlineEmpty{};
    std::vector<bool> m_VlineEmpty{};
};

#endif // GLYPHEXTRACTOR_H_
