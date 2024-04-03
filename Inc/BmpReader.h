// Copyright(c) <2024 > <Wilhelm Kurz>, see License.txt

#ifndef  BMPREADER_H_
#define  BMPREADER_H_


#include <vector>
#include <string>

class BmpReader 
{
public:
    BmpReader() 
    :m_bytesPerLine()
    ,m_pixelheight()
    ,m_data()
    {}

    bool read(const char* fname);

protected:
    int32_t pixelheight() const
    {
        return m_pixelheight; 
    }

    int32_t bytesPerLine() const
    {
        return m_bytesPerLine;
    }

    const std::vector<uint8_t>& Data() const
    {
        return m_data;
    }

    void invertBitmap();

    std::string m_fname;

private:
    uint32_t m_bytesPerLine;
    uint32_t m_pixelheight;
    std::vector<uint8_t> m_data;
};

#endif // BMPREADER_H_
