# The name of project is GlyphExtractor.
cmake_minimum_required (VERSION 3.5)

project (GlyphExtractor)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)


set( SOURCES  
./Src/main.cpp 
./Src/BmpReader.cpp 
./Src/GlyphExtractor.cpp 
)

include_directories( ./Inc)

add_definitions(-DDebug  -D_CRT_SECURE_NO_WARNINGS -D_UNICODE)
add_executable( GlyphExtractor ${SOURCES} )


if (W32)
  set_target_properties(GlyphExtractor PROPERTIES COMPILE_FLAGS -m32 LINK_FLAGS -m32)
endif ()


