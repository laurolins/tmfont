#include "tmfont.hh"

#include <cstring>


namespace tmfont {
    
    //--------------------------------------------------------------------------------
    // Library
    //--------------------------------------------------------------------------------
    
    Library::Library() {
        FT_Init_FreeType(&_library);
    }
    
    Library::~Library() {
        FT_Done_FreeType(_library);
    }
    
    auto Library::face(std::string ttf_filename) -> Face& {
        auto it = faces.find(ttf_filename);
        if (it == faces.end()) {
            auto face = new Face(*this, ttf_filename);
            faces[ttf_filename].reset(face);
            return *face;
        }
        else {
            return *it->second.get();
        }
    }
    
    //--------------------------------------------------------------------------------
    // Face
    //--------------------------------------------------------------------------------
    
    Face::Face(Library& library, std::string ttf_filename):
    library(library)
    {
        auto error = FT_New_Face( library._library, ttf_filename.c_str(), 0, &_face );
        if (error) {
            throw FreeTypeException("Could not create face");
        }
    }
    
    Face::~Face()
    {
        // FT_Done_Face(_face);
    }
    
    Face& Face::setCharSize(int pt_size, int dpi)
    {
        auto error = FT_Set_Char_Size(_face,  pt_size * 64, pt_size * 64, dpi, dpi);
        if (error) {
            throw FreeTypeException("Could not set char size");
        }
        return *this;
    }
    
    
    
    static void _raster_callback(const int y,
                                 const int count,
                                 const FT_Span * const spans,
                                 void * const user)
    {
        Spans* sptr = (Spans*) user;
        for (int i = 0; i < count; ++i)
            sptr->insert(Span(spans[i].x, y, spans[i].len, spans[i].coverage));
    }
    
    auto Face::glyph(Character code) -> Glyph
    {
        FT_Error error;
        
        auto _glyph_index = FT_Get_Char_Index(_face, code);
        
        error = FT_Load_Glyph(_face, _glyph_index, FT_LOAD_NO_BITMAP);
        if (error) {
            throw FreeTypeException("Could not load glyph");
        }
        
        FT_Glyph _glyph;
        error = FT_Get_Glyph(_face->glyph, &_glyph);
        if (error) {
            throw FreeTypeException("Could not load glyph");
        }
        
        Glyph result(code, _face->glyph->metrics);
        
        float border_width = 0.0f;
        
        // now get the opacity texture
        FT_Stroker _stroker;
        FT_Stroker_New(library._library, &_stroker);
        FT_Stroker_Set(_stroker,
                       (int)(border_width * 64),
                       FT_STROKER_LINECAP_ROUND,  // FT_STROKER_LINECAP_SQUARE
                       FT_STROKER_LINEJOIN_ROUND, // FT_STROKER_LINEJOIN_MITER
                       0);
        
        static const int outside       = 0;
        static const int destroy_glyph = 1;
        FT_Glyph_StrokeBorder(&_glyph, _stroker, outside, destroy_glyph);
        
        FT_Outline *outline = &reinterpret_cast<FT_OutlineGlyph>(_glyph)->outline;
        
        FT_Raster_Params params;
        std::memset(&params, 0, sizeof(params));
        params.flags      = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
        params.gray_spans = _raster_callback;
        params.user       = static_cast<void*>(&result.spans); // object
        
        //
        FT_Outline_Render(library._library,
                          outline,
                          &params);
        
        // Clean up afterwards.
        FT_Stroker_Done(_stroker);
        
        // std::cout << result.spans.bounds << std::endl;
        
        return result;
        
    }
        
        
        
        //    bool Glyph::hasOutlineInfo()
        //    {
        //        return _glyph->format == FT_GLYPH_FORMAT_OUTLINE;
        //    }
        
        
        //
        //    auto Glyph::_render(FT_Outline* outline) -> Spans
        //    {
        //        Spans result;
        //
        //        FT_Raster_Params params;
        //        std::memset(&params, 0, sizeof(params));
        //        params.flags      = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
        //        params.gray_spans = _raster_callback;
        //        params.user       = static_cast<void*>(&result); // object
        //
        //        //
        //        FT_Outline_Render(face.library._library,
        //                          outline,
        //                          &params);
        //
        //        return result;
        //    }
        //
        //
        //    auto Glyph::render(float width) -> Spans
        //    {
        //        FT_Stroker _stroker;
        //        FT_Stroker_New(face.library._library, &_stroker);
        //        FT_Stroker_Set(_stroker,
        //                       (int)(width * 64),
        //                       FT_STROKER_LINECAP_ROUND,  // FT_STROKER_LINECAP_SQUARE
        //                       FT_STROKER_LINEJOIN_ROUND, // FT_STROKER_LINEJOIN_MITER
        //                       0);
        //
        //        static const int outside       = 0;
        //        static const int destroy_glyph = 1;
        //        FT_Glyph_StrokeBorder(&_glyph, _stroker, outside, destroy_glyph);
        //
        //        FT_Outline *outline = &reinterpret_cast<FT_OutlineGlyph>(_glyph)->outline;
        //
        //        Spans result = _render(outline);
        //
        //        // Clean up afterwards.
        //        FT_Stroker_Done(_stroker);
        //
        //        std::cout << result.bounds << std::endl;
        //
        //        return result;
        //        }
        
        
        
        
        
        
        
        
        //        void Face::load(Glyph &glyph)
        //        {
        //            auto error = FT_Load_Glyph(_face, glyph._glyph_index, FT_LOAD_NO_BITMAP);
        //            if (error) {
        //                throw FreeTypeException("Could not load glyph");
        //            }
        //            this->last_loaded_glyph_code = glyph.code;
        //        }
        
        
        std::ostream &operator<<(std::ostream &os, const FT_Glyph_Metrics &m)
        {
            os << "FT_Glyph_Metrics [ " << std::endl;
            os << "    height:        " << m.height << std::endl;
            os << "    width:         " << m.width << std::endl;
            os << "    horiAdvance:   " << m.horiAdvance << std::endl;
            os << "    horiBearingX:  " << m.horiBearingX << std::endl;
            os << "    horiBearingY:  " << m.horiBearingY << std::endl;
            os << "    vertAdvance:   " << m.vertAdvance << std::endl;
            os << "    vertBearingX:  " << m.vertBearingX << std::endl;
            os << "    vertBearingY:  " << m.vertBearingY << std::endl;
            os << "]" << std::endl;
            
            return os;
        }
        
        
        std::ostream &operator<<(std::ostream &os, const Rect &r)
        {
            os << "Rect [ " << std::endl;
            os << "    xmin: " << r.xmin << std::endl;
            os << "    ymin: " << r.ymin << std::endl;
            os << "    xmax: " << r.xmax << std::endl;
            os << "    ymax: " << r.ymax << std::endl;
            os << "]" << std::endl;
            
            return os;
        }
        
        //------------------------------------------------------------------------------
        // Glyph
        //------------------------------------------------------------------------------
        
        Glyph::Glyph(Character code, const GlyphMetrics& metrics):
        code(code), metrics(metrics)
        {}
        
        //------------------------------------------------------------------------------
        // FreeTypeException Impl.
        //------------------------------------------------------------------------------
        
        FreeTypeException::FreeTypeException(const std::string &message):
        std::runtime_error(message)
        {}
        
        //------------------------------------------------------------------------------
        // Span
        //------------------------------------------------------------------------------
        
        Span::Span(int _x, int _y, int _width, int _coverage)
        : x(_x), y(_y), width(_width), coverage(_coverage) { }
        
        
        //------------------------------------------------------------------------------
        // io
        //------------------------------------------------------------------------------
        
        std::ostream& io::operator<<(std::ostream &os, const Span &span) {
            os << "Span[x:" << span.x << ", y: " << span.y << ", w: " << span.width << ", coverage: " << span.coverage << "]";
            return os;
        }
        
        //------------------------------------------------------------------------------
        // Vec2
        //------------------------------------------------------------------------------
        
        Vec2::Vec2(int x, int y):
        x(x), y(y)
        {}
        
        //------------------------------------------------------------------------------
        // Rect
        //------------------------------------------------------------------------------
        
        Rect::Rect(int xmin, int ymin, int xmax, int ymax)
        : xmin(xmin), ymin(ymin), xmax(xmax), ymax(ymax)
        {}
        
        void Rect::insert(const Vec2 &r)
        {
            xmin = std::min(xmin, r.x);
            ymin = std::min(ymin, r.y);
            xmax = std::max(xmax, r.x);
            ymax = std::max(ymax, r.y);
        }
        
        void Rect::merge(const Rect &other)
        {
            xmin = std::min(xmin, other.xmin);
            ymin = std::min(ymin, other.ymin);
            xmax = std::max(xmax, other.xmax);
            ymax = std::max(ymax, other.ymax);
        }
        
        int Rect::width() const
        {
            return xmax - xmin + 1;
        }
        
        int Rect::height() const
        {
            return ymax - ymin + 1;
        }
        
        auto operator+(const Rect& a, const Rect &b) -> Rect {
            return Rect(std::min(a.xmin, b.xmin),
                        std::min(a.ymin, b.ymin),
                        std::max(a.xmax, b.xmax),
                        std::max(a.ymax, b.ymax));
        }
        
        
        //------------------------------------------------------------------------------
        // Spans
        //------------------------------------------------------------------------------
        
        void Spans::insert(const Span &span)
        {
            spans.push_back(span);
            bounds.insert({span.x, span.y});
            bounds.insert({span.x+span.width-1, span.y});
        }
        
        }
        
        
        
