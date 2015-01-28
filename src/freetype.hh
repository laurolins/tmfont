#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

namespace freetype {

//------------------------------------------------------------------------------
// Forward
//------------------------------------------------------------------------------

struct Library;
struct Glyph;
struct Face;
struct Span;
struct Spans;
struct Stroker;

using Character = wchar_t;

//------------------------------------------------------------------------------
// Vec2
//------------------------------------------------------------------------------

struct Vec2
{
    Vec2() = default;
    Vec2(int x, int y);
    int x {0}, y {0};
};

//------------------------------------------------------------------------------
// Rect
//------------------------------------------------------------------------------

struct Rect
{
public:

    Rect() = default;
    Rect(int xmin, int ymin, int xmax, int ymax);

    Rect(const Rect &other) = default;
    Rect& operator=(const Rect &other) = default;

public:
    void insert(const Vec2 &r);
    void merge(const Rect& other);
    int width() const;
    int height() const;
    int xmin {0}, ymin {0}, xmax {0}, ymax {0};
};

//------------------------------------------------------------------------------
// Library
//------------------------------------------------------------------------------

struct Library {
    Library();
    ~Library();
public:
    auto createFace(std::string ttf_filename) -> Face&;
public:
    FT_Library _library;
    std::vector<std::unique_ptr<Face>> faces;
};

//------------------------------------------------------------------------------
// Stroker
//------------------------------------------------------------------------------

struct Stroker {
public:
    Stroker(Library &library, float width);
    ~Stroker();
public:
    Library &library;
    float width;
    FT_Stroker _stroker;
};

//------------------------------------------------------------------------------
// Face
//------------------------------------------------------------------------------

struct Face {
private:
    Face(Library& library, std::string ttf_filename);
    friend struct Library;
public:
    ~Face();
    void setCharSize(int pt_size, int dpi);
    auto getGlyph(Character code) -> Glyph&;
private:
    void load(Glyph &glyph);
    friend struct Glyph;
public:
    Library& library;
    FT_Face  _face; // low level c
    std::unordered_map<Character,std::unique_ptr<Glyph>> glyphs;
    Character last_loaded_glyph_code;
};


struct GlyphMetrics {
    int advance      { 0 };
    int left         { 0 };
    int top          { 0 };
    bool initialized { false };
};

//------------------------------------------------------------------------------
// Glyph
//------------------------------------------------------------------------------

struct Glyph {
private:
    Glyph(Face &face, Character code);
    friend struct Face;
    auto _render(FT_Outline *outline) -> freetype::Spans;
public:
    ~Glyph();
    void load();
    bool hasOutlineInfo();
    Spans render(float border_width=0.0f);
public:
    Face&     face;
    Character code;
    FT_Glyph  _glyph;
    FT_UInt   _glyph_index;
    FT_Glyph_Metrics _metrics;
};

//------------------------------------------------------------------------------
// Span: horizontal segment with intensity
//------------------------------------------------------------------------------

struct Span
{
    Span() = default;
    Span(int _x, int _y, int _width, int _coverage);
public:
    int x        { 0 };
    int y        { 0 };
    int width    { 0 };
    int coverage { 0 };
};

//------------------------------------------------------------------------------
// Spans
//------------------------------------------------------------------------------

struct Spans {
public:
    void insert(const Span &span);
public:
    Rect bounds; // bounding box
    std::vector<Span> spans;
};




auto operator+(const Rect& a, const Rect &b) -> Rect;

//------------------------------------------------------------------------------
// io utility functions
//------------------------------------------------------------------------------

namespace  io {
std::ostream& operator<<(std::ostream &os, const Span &span);
}

//------------------------------------------------------------------------------
// FreeTypeException
//------------------------------------------------------------------------------

struct FreeTypeException: public std::runtime_error {
public:
    FreeTypeException(const std::string &message);
};

}
