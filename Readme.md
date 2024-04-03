
# GlyphExtractor

## Extract font glyphs from a BMP file into a C-array

To use a bitmap font in an embedded system it needs to be converted into a C-array.
If the font is available as a BMP file GlyphExtractror can be used to do this conversion.
GlyphExtractor expects the individual glyphs to be arranged in a regular grid in a monochrome bitmap.


## Syntax
<p>

```batch
GlyphExtractor FNAME_SRC FNAME_DST [-cH CHARHEIGHT -rS ROWSPACING -nR NUMROWS -cW CHARWIDTH -cS COLSPACING -nC NUMCOLUMNS -f FLAT -i INVERT -lE LITTLEENDIAN]\n");

```

</p>

<dl>
  <dt>FNAME_SRC</dt>
  <dd>Input file. Must be a monochrome (1bit/pixel) file in the BMP format. </dd>
</dl>

<dl>
  <dt>FNAME_DST</dt>
  <dd>Output file. </dd>
</dl>

<dl>
  <dt>CHARHEIGHT</dt>
  <dd>Optional. Maximum height of a glyph. Program will try to determine this value, if not given. </dd>
</dl>

<dl>
  <dt>ROWSPACING</dt>
  <dd>Optional. Gap between the individual rows of glyphs. Program will try to determine this value, if not given. </dd>
</dl>

<dl>
  <dt>NUMROWS</dt>
  <dd>Optional. Number of rows. Program will try to determine this value, if not given. </dd>
</dl>

<dl>
  <dt>CHARWIDTH</dt>
  <dd>Optional. Maximum width of a glyph. Program will try to determine this value, if not given. </dd>
</dl>

<dl>
  <dt>COLSPACING</dt>
  <dd>Optional. Gap between the individual columns of glyphs. Program will try to determine this value, if not given. </dd>
</dl>

<dl>
  <dt>NUMCOLUMNS</dt>
  <dd>Optional. Number of columns. Program will try to determine this value, if not given. </dd>
</dl>

<dl>
  <dt>FLAT</dt>
  <dd>Optional. Default: 1. 1: C-array is a flat sequence of bytes. 0: Bytes of a characters are  enclosed in curly braces.</dd>
</dl>

<dl>
  <dt>INVERT</dt>
  <dd>Optional.1: Invert colors. 0: do not invert colors. Program will try to determine this value, if not given.</dd>
</dl>

<dl>
  <dt>LITTLEENDIAN</dt>
  <dd>Optional. Default: 1. Endianness of characters with more than one byte per line. 1: Littlendian, 0: Bigendian.</dd>
</dl>



## How to use GlyphExtractor

To create a BMP file from a given TTF font you may e.g. use the online tool https://stmn.itch.io/font2bitmap
Convert the PNG file into a bitmap using paint.exe or any other graphics program and feed the BMP file into GlyphExtractor.
If the bitmap file is well bahaved it will not be necessary to specify any of the optional arguments .



##History<br>



