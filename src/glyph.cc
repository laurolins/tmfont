#include "glyph.hh"

#include <cmath>
#include <iostream>
#include <fstream>

//-----------------------------------------------------------------------------
// Metrics
//-----------------------------------------------------------------------------

Metrics::Metrics(int width, int height, int left, int top, int advance):
    width(width), height(height), left(left), top(top), advance(advance)
{}

//-----------------------------------------------------------------------------
// Glyph
//-----------------------------------------------------------------------------

//Glyph::Glyph(int            code,
//             const Metrics& metrics,
//             int            xoff,
//             int            yoff):
//    code(code),
//    metrics(metrics),
//    xoff(xoff),
//    yoff(yoff)
//{
//    // image.resize(width, height);
//}

Glyph::Glyph(int code, int bitmap_width, int bitmap_height):
    code ( code )
{
    image.resize(bitmap_width, bitmap_height);
}


//-----------------------------------------------------------------------------
// GlyphList
//-----------------------------------------------------------------------------

GlyphList::GlyphList(int dpi, int pt_size):
    dpi(dpi), pt_size(pt_size), max_code(-1), max_glyph_width(0), max_glyph_height(0)
{}

void GlyphList::getGoodGlyphSize(int margin, int &width, int &height) const
{
    int width_exponent  = (int) std::ceil(log2(max_glyph_width + 2 * margin));
    int height_exponent = (int) std::ceil(log2(max_glyph_height + 2 * margin));

    width  = 1 << width_exponent;
    height = 1 << height_exponent;
}

Glyph &GlyphList::addGlyph(int code, int bitmap_width, int bitmap_height)
{
    glyphs.push_back(std::unique_ptr<Glyph>(new Glyph(code, bitmap_width, bitmap_height)));

    max_code         = std::max(max_code, code);
    max_glyph_width  = std::max(max_glyph_width,  bitmap_width);
    max_glyph_height = std::max(max_glyph_height, bitmap_height);

    return *glyphs.back().get();
}

void GlyphList::save(int img_max_width, int margin, bool squared, std::string filename) {

    // set file names
    std::string tmfont_description_file = filename+std::string("_tmfont.txt");
    std::string tmfont_texture_file     = filename+std::string("_tmfont.png");

    // get uniform width and height that are good for all glyphs
    int glyph_width, glyph_height;
    this->getGoodGlyphSize(margin, glyph_width, glyph_height);

    // no. glyphs
    auto n = this->glyphs.size();

    //
    // int rows = std::ceil((1.0f * n * glyph_width) / image_max_width);
    int cols = img_max_width / glyph_width;
    int rows = std::ceil((1.0f * n) / cols);

    int img_width  = cols * glyph_width;
    int img_height = rows * glyph_height;
    Image result;
    result.resize(img_width, img_height);

    int index = 0;
    for (int i=0; i < rows && index < n ; i++) {
        for (int j=0; j < cols && index < n; j++) {
            Glyph &g = *glyphs[index++].get();

            int i0 = i * glyph_height ;
            int j0 = j * glyph_width  ;

            g.xtex = j0;
            g.ytex = i0;

            g.wtex = glyph_width;
            g.htex = glyph_height;

            result.setRegion(g.image, i0 + margin, j0 + margin);

            // subtract this from cursor to get the texture
            // lower left coord

            // it is a length

            g.xoff += margin;// - g->left;
            g.yoff += margin;// + (g->image.height - g->top);
        }
    }

    //
    result.save(tmfont_texture_file);

    std::ofstream os(tmfont_description_file);
    { // dump description
        char sep = ' ';

        // write global properties of the font
        os << "#pt_size dpi max_code width height" << std::endl; // min_px_y max_px_y margin" << std::endl;

        // write global properties of the font
        os << this->pt_size << sep
           << this->dpi << sep
           << this->max_code << sep
           << img_width << sep
           << img_height << std::endl;

        // write global properties of the font
        os << "#textured mapped font" << std::endl;

        // write global properties of the font
        os << tmfont_texture_file << std::endl;


        os << "#code xoff yoff glyph_width glyph_height left top advance xtex ytex wtex htex" << std::endl;

        int left_margin = 0;
        int top_margin  = 0;

        for (auto &g: glyphs) {
            os << g->code << sep
               << g->xoff + left_margin << sep
               << g->yoff + top_margin  << sep
               << g->metrics.width << sep
               << g->metrics.height << sep
               << g->metrics.left << sep
               << g->metrics.top  << sep
               << g->metrics.advance << sep
               << g->xtex << sep
               << g->ytex << sep
               << g->wtex << sep
               << g->htex << std::endl;
        }
    }
}




