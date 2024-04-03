// Copyright(c) <2024 > <Wilhelm Kurz>, see License.txt


#include <stdint.h>
#include <stdio.h>
#include "GlyphExtractor.h"
#include <map>
#include <string>


int main(int argc, char* argv[]) 
{
    if (argc < 3) 
    {
        printf("Usage : exe FNAME_SRC FNAME_DST [-cH CHARHEIGHT -rS ROWSPACING -nR NUMROWS -cW CHARWIDTH -cS COLSPACING -nC NUMCOLUMNS\n-f FLAT -i INVERT -lE LITTLEENDIAN]\n");
        return -1;
    }
    GlyphExtractor::WriteArgs args;

    std::map<const std::string, int32_t*> argmap
    {
        {"-cH", &args.charheight},
        {"-rS", &args.rowspacing },
        {"-nR", &args.numrows },
        {"-cW", &args.charwidth },
        {"-cS", &args.colspacing },
        {"-nC", &args.numcolumns },
        {"-f",  &args.flat },
        {"-i",  &args.invert },
        {"-lE", &args.littleendian },
    };



    for (int32_t i = 3; i < argc-1; i++)
    {
        auto a = argmap.find(std::string(argv[i]));
        if (a != argmap.end())
            *(a->second) = std::stoi(argv[i+1]);
    }

    GlyphExtractor glex;
    if (!glex.read(argv[1]))
        return 1;

    return glex.writeCArray(argv[2], args) == true;
}

