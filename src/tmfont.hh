#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

namespace tmfont {
    
    //------------------------------------------------------------------------------
    // Forward
    //------------------------------------------------------------------------------
    
    struct Library;
    struct Glyph;
    struct Face;
    struct Span;
    struct Spans;
    struct Stroker;
    
    using  Character = FT_ULong;
    
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
    
    auto operator+(const Rect& a, const Rect &b) -> Rect;
    
    //------------------------------------------------------------------------------
    // Library
    //------------------------------------------------------------------------------
    
    struct Library {
        Library();
        ~Library();
    public:
        auto face(std::string ttf_filename) -> Face&;
    public:
        FT_Library _library;
        std::unordered_map<std::string, std::unique_ptr<Face>> faces;
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

    //------------------------------------------------------------------------------
    // Face
    //------------------------------------------------------------------------------
    
    struct Face {
    private:
        Face(Library& library, std::string ttf_filename);
        friend struct Library;
    public:
        ~Face();
        Face& setCharSize(int pt_size, int dpi);
        auto glyph(Character code) -> Glyph;
    public:
        Library& library;
        FT_Face  _face; // low level c
    };

    //------------------------------------------------------------------------------
    // GlyphMetrics
    //------------------------------------------------------------------------------
    
    using GlyphMetrics = FT_Glyph_Metrics;
    
    //------------------------------------------------------------------------------
    // Glyph
    //------------------------------------------------------------------------------
    
    struct Glyph {
    public:
        Glyph(Character code, const GlyphMetrics& metrics);
    public:
        Character    code;
        GlyphMetrics metrics;
        Spans        spans;
    };
    
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
