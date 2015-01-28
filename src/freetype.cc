#include <freetype.hh>

#include <cstring>

//--------------------------------------------------------------------------------
// freetype::Library
//--------------------------------------------------------------------------------

freetype::Library::Library() {
    FT_Init_FreeType(&_library);
}

freetype::Library::~Library() {
    FT_Done_FreeType(_library);
}

auto freetype::Library::createFace(std::string ttf_filename) -> Face& {
    faces.push_back(std::unique_ptr<Face>(new Face(*this, ttf_filename)));
    return *faces.back().get();
}

//--------------------------------------------------------------------------------
// freetype::Face
//--------------------------------------------------------------------------------

freetype::Face::Face(Library& library, std::string ttf_filename):
    library(library)
{
    auto error = FT_New_Face( library._library, ttf_filename.c_str(), 0, &_face );
    if (error) {
        throw FreeTypeException("Could not create face");
    }
}

freetype::Face::~Face()
{
    // FT_Done_Face(_face);
}

void freetype::Face::setCharSize(int pt_size, int dpi)
{
    auto error = FT_Set_Char_Size(_face,  pt_size * 64, pt_size * 64, dpi, dpi);
    if (error) {
        throw FreeTypeException("Could not set char size");
    }
}

auto freetype::Face::getGlyph(freetype::Character code) -> freetype::Glyph&
{
    if (glyphs.count(code)) {
        return *glyphs[code].get();
    }
    else {
        Glyph* g     =  new Glyph(*this, code);
        glyphs[code] = std::unique_ptr<Glyph>(g);
        return *g;
    }
//    auto lambda = [code](const std::unique_ptr<Glyph> &glyph) -> bool {
//        return (glyph->code == code);
//    };
//    auto it = std::find_if(glyphs.begin(), glyphs.end(), lambda);
    //    if (it == glyphs.end()) {
}

void freetype::Face::load(freetype::Glyph &glyph)
{
    auto error = FT_Load_Glyph(_face, glyph._glyph_index, FT_LOAD_NO_BITMAP);
    if (error) {
        throw FreeTypeException("Could not load glyph");
    }
    this->last_loaded_glyph_code = glyph.code;
}


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


std::ostream &operator<<(std::ostream &os, const freetype::Rect &r)
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
// freetype::Glyph
//------------------------------------------------------------------------------

freetype::Glyph::Glyph(freetype::Face &face, freetype::Character code):
    face(face), code(code)
{
    _glyph_index = FT_Get_Char_Index(face._face, code);
    face.load(*this);
    auto error = FT_Get_Glyph(face._face->glyph, &_glyph);

    _metrics = face._face->glyph->metrics;

    std::cout << _metrics ;

    if (error) {
        throw FreeTypeException("Could not load glyph");
    }
}

freetype::Glyph::~Glyph()
{
    // FT_Done_Glyph(_glyph);
}

bool freetype::Glyph::hasOutlineInfo()
{
    return _glyph->format == FT_GLYPH_FORMAT_OUTLINE;
}

static void _raster_callback(const int y,
               const int count,
               const FT_Span * const spans,
               void * const user)
{
    freetype::Spans* sptr = (freetype::Spans*) user;
    for (int i = 0; i < count; ++i)
        sptr->insert(freetype::Span(spans[i].x, y, spans[i].len, spans[i].coverage));
}

auto freetype::Glyph::_render(FT_Outline* outline) -> freetype::Spans
{
    Spans result;

    FT_Raster_Params params;
    std::memset(&params, 0, sizeof(params));
    params.flags      = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
    params.gray_spans = _raster_callback;
    params.user       = static_cast<void*>(&result); // object

    //
    FT_Outline_Render(face.library._library,
                      outline,
                      &params);

    return result;
}


auto freetype::Glyph::render(float width) -> freetype::Spans
{
    FT_Stroker _stroker;
    FT_Stroker_New(face.library._library, &_stroker);
    FT_Stroker_Set(_stroker,
                   (int)(width * 64),
                   FT_STROKER_LINECAP_ROUND,  // FT_STROKER_LINECAP_SQUARE
                   FT_STROKER_LINEJOIN_ROUND, // FT_STROKER_LINEJOIN_MITER
                   0);

    static const int outside       = 0;
    static const int destroy_glyph = 1;
    FT_Glyph_StrokeBorder(&_glyph, _stroker, outside, destroy_glyph);

    FT_Outline *outline = &reinterpret_cast<FT_OutlineGlyph>(_glyph)->outline;

    Spans result = _render(outline);

    // Clean up afterwards.
    FT_Stroker_Done(_stroker);

    std::cout << result.bounds << std::endl;

    return result;
}


//------------------------------------------------------------------------------
// FreeTypeException Impl.
//------------------------------------------------------------------------------

freetype::FreeTypeException::FreeTypeException(const std::string &message):
    std::runtime_error(message)
{}

//------------------------------------------------------------------------------
// freetype::Span
//------------------------------------------------------------------------------

freetype::Span::Span(int _x, int _y, int _width, int _coverage)
    : x(_x), y(_y), width(_width), coverage(_coverage) { }


//------------------------------------------------------------------------------
// freetype::io
//------------------------------------------------------------------------------

std::ostream& freetype::io::operator<<(std::ostream &os, const Span &span) {
    os << "Span[x:" << span.x << ", y: " << span.y << ", w: " << span.width << ", coverage: " << span.coverage << "]";
    return os;
}

//------------------------------------------------------------------------------
// freetype::Vec2
//------------------------------------------------------------------------------

freetype::Vec2::Vec2(int x, int y):
    x(x), y(y)
{}

//------------------------------------------------------------------------------
// freetype::Rect
//------------------------------------------------------------------------------

freetype::Rect::Rect(int xmin, int ymin, int xmax, int ymax)
    : xmin(xmin), ymin(ymin), xmax(xmax), ymax(ymax)
{}

void freetype::Rect::insert(const Vec2 &r)
{
    xmin = std::min(xmin, r.x);
    ymin = std::min(ymin, r.y);
    xmax = std::max(xmax, r.x);
    ymax = std::max(ymax, r.y);
}

void freetype::Rect::merge(const freetype::Rect &other)
{
    xmin = std::min(xmin, other.xmin);
    ymin = std::min(ymin, other.ymin);
    xmax = std::max(xmax, other.xmax);
    ymax = std::max(ymax, other.ymax);
}

int freetype::Rect::width() const
{
    return xmax - xmin + 1;
}

int freetype::Rect::height() const
{
    return ymax - ymin + 1;
}

auto freetype::operator+(const Rect& a, const Rect &b) -> Rect {
    return Rect(std::min(a.xmin, b.xmin),
                std::min(a.ymin, b.ymin),
                std::max(a.xmax, b.xmax),
                std::max(a.ymax, b.ymax));
}


//------------------------------------------------------------------------------
// freetype::Spans
//------------------------------------------------------------------------------

void freetype::Spans::insert(const freetype::Span &span)
{
    spans.push_back(span);
    bounds.insert({span.x, span.y});
    bounds.insert({span.x+span.width-1, span.y});
}




