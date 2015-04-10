#pragma once

#include <memory>

#include "image.hh"

struct Metrics {
public:
    Metrics() = default;
    Metrics(int width, int height, int left, int top, int advance);
public:
    int width   { 0 };    // metrics of the base font (border is not considered here).
    int height  { 0 };
    int left    { 0 };     // horiz. distance from cursor to glyph left most border
    int top     { 0 };  // vert.  distance from cursor to glyph top  most border
    int advance { 0 };  // x advance
};

//-----------------------------------------------------------------------------
// Glyph
//-----------------------------------------------------------------------------

struct Glyph {

    Glyph(int code, int bitmap_width, int bitmap_height);

//    Glyph(int code,
//          int width,
//          int height,
//          int left,
//          int top,
//          int advance,
//          int xoff,
//          int yoff);

public:

    int code { 0 };

    Metrics metrics;

    Image   image;

    int xoff { 0 };     // x baseline cursor location in reference to xtex, ytex
    int yoff { 0 };     //

    int xtex { 0 };     // texture for the glyph is the subimage
    int ytex { 0 };     //   [xtex,xtex+wtex) x [ytex,ytex+htex)

    int wtex { 0 };
    int htex { 0 };

};


//-----------------------------------------------------------------------------
// GlyphList
//-----------------------------------------------------------------------------

struct GlyphList
{
    GlyphList(int dpi, int pt_size);

    Glyph &addGlyph(int code, int bitmap_width, int bitmap_height);

    void save(int img_max_width, int margin, bool squared, std::string filename);

    void getGoodGlyphSize(int margin, int &width, int &height) const;

public:

    std::vector<std::unique_ptr<Glyph>> glyphs;

    int   dpi;
    int   pt_size;
    int   max_code;
    int   max_glyph_width;
    int   max_glyph_height;

};

