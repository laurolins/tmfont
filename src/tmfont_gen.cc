#include <stdio.h>
#include <math.h>

#include <limits.h>
#include <stdint.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include <algorithm>

#include <ft2build.h>

#include <FreeImage.h>

#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_IMAGE_H

#include "glyph.hh"
#include "image.hh"

#include "tmfont.hh"

std::vector<std::string> &split(const std::string &s, char delim,
                                std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

//-----------------------------------------------------------------------------
// InitializeLibraries
//-----------------------------------------------------------------------------

struct InitializeLibraries {
    InitializeLibraries();
    ~InitializeLibraries();
};

InitializeLibraries::InitializeLibraries() {
    FreeImage_Initialise();
}

InitializeLibraries::~InitializeLibraries() {
    FreeImage_DeInitialise();
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------

int main(int argc, char**  argv )
{

    int   pt_size   = 12;
    int   dpi       = 72;
    int   img_width = 512;
    int   margin    = 5;
    float border    = 0.0f;
    bool  bw        = false;
    bool  squared   = false;
    std::string font_filename;   //("FreeSans.ttf");
    std::string output_filename; //("FreeSans");

    // parse parameters
    try {
        std::string       type_st("");
        for (int i=1;i<argc;i++) {
            std::vector<std::string> tokens;
            split(std::string(argv[i]),'=',tokens);
            if (tokens.size() < 2) {
                continue;
            }
            else if (tokens[0].compare("--pt") == 0) {
                pt_size = std::stoi(tokens[1]);
            }
            else if (tokens[0].compare("--dpi") == 0) {
                dpi = std::stoi(tokens[1]);
            }
            else if (tokens[0].compare("--texwidth") == 0) {
                img_width = std::stoi(tokens[1]);
            }
            else if (tokens[0].compare("--margin") == 0) {
                margin = std::stoi(tokens[1]);
            }
            else if (tokens[0].compare("--border") == 0) {
                border = std::stof(tokens[1]);
            }
            else if (tokens[0].compare("--squared") == 0) {
                squared = std::stoi(tokens[1]) != 0;
            }
            else if (tokens[0].compare("--font") == 0) {
                font_filename = tokens[1];
            }
            else if (tokens[0].compare("--out") == 0) {
                output_filename = tokens[1];
            }
            else if (tokens[0].compare("--bw") == 0) {
                bw = std::stoi(tokens[1]) != 0;
            }
        }
    }
    catch (...)
    {}


    if (font_filename.size() == 0 || output_filename.size() == 0) {
        std::cout << "Usage: " << std::endl << std::endl;
        std::cout << "    tmfont --font=FreeSans.ttf --out=FreeSans" << std::endl << std::endl;
        std::cout << "Options: " << std::endl;
        std::cout << "    --pt=12" << std::endl;
        std::cout << "    --dpi=72" << std::endl;
        std::cout << "    --texwidth=512" << std::endl;
        std::cout << "    --margin=5" << std::endl << std::endl;
        std::cout << "    --border=0.0f" << std::endl;
        return 0;
    }

    InitializeLibraries initialize_libraries_raii;

    // create an empty GlyphList
    GlyphList glyph_list(dpi, pt_size);



    tmfont::Library library;
    tmfont::Face   &face = library.face(font_filename);
    face.setCharSize(pt_size, dpi);

//    {
//        // load font and
//        FT_Library    library;
//        // FT_Face       face;
//        FT_GlyphSlot  slot;
//        /* FT_Matrix     matrix;  /\* transformation matrix *\/ */
//        /* FT_UInt       glyph_index; */
//        FT_Vector     pen;                    /* untransformed origin  */
//        FT_Error      error;
//        error = FT_Init_FreeType( &library );              /* initialize library */
//        error = FT_New_Face( library, font_filename.c_str(), 0, &face ); /* create face object */
//        error = FT_Set_Char_Size( face,  pt_size * 64, pt_size * 64, dpi, dpi ); /* set character size */
        //
//        float outline_width = 1.0f;
//        // Set up a stroker.
//        FT_Stroker stroker;
//        FT_Stroker_New(library, &stroker);
//        FT_Stroker_Set(stroker,
//                       (int)(outline_width * 64),
//                       FT_STROKER_LINECAP_ROUND,
//                       FT_STROKER_LINEJOIN_ROUND,
//                       0);
//        /* error handling omitted */
//        slot = face->glyph;

        /* the pen position in 26.6 cartesian space coordinates; */
        /* start at (300,200) relative to the upper left corner  */

    tmfont::Character code;

    // what to generate?...
    int       num_ranges  = 1;
    // tmfont::Character ranges[]    = {0,255};
    // printable ascii chars
    tmfont::Character ranges[]    = {0x20,0x7E};
    // tmfont::Character ranges[]    = {(int) 'O', (int) 'O'};
    // int ranges[]    = {(int) 'B',(int) 'B'};
    // int ranges[]    = {(int) 'p',(int) 'p', (int) 't',(int) 't'};
    /* int ranges[]    = {(int)'/', (int)'/'}; */
    /* int ranges[]    = {469,469}; */

    int range_index;
    // int sum_widths = 0;
    for (range_index=0;range_index<num_ranges;range_index++)
    {
        auto min_code = ranges[2*range_index];
        auto max_code = ranges[2*range_index+1];

        /* num_glyphs += (ranges[2*range_index+1] - ranges[2*range_index]) + 1; */
        for (code=min_code;code<=max_code;code++)
        {
            printf("generating texture for code %d %c\n",code,(char) code);

            //
            tmfont::Glyph glyph = face.glyph(code);

            //
            tmfont::Spans& spans = glyph.spans;

//            /* ignore errors */
//            error = FT_Load_Char( face, code, FT_LOAD_RENDER ); // FT_LOAD_RENDER
//            // error = FT_Load_Char( face, code, FT_LOAD_TARGET_MONO | FT_LOAD_MONOCHROME | FT_LOAD_RENDER);
//            if ( error ) {
//                printf("WARNING: Could not load char code %d\n",code);
//                continue;
//            }
//            std::cout << "    --margin=5" << std::endl << std::endl;


            Glyph &g = glyph_list.addGlyph(code,spans.bounds.width(), spans.bounds.height()); //width,height,left,top,advance, xoff, yoff);

            g.metrics.width   = glyph.metrics.width/64;  // spans.bounds.width();    // these include borders
            g.metrics.height  = glyph.metrics.height/64; // spans.bounds.height();
            g.metrics.left    = glyph.metrics.horiBearingX/64; // - border;
            g.metrics.top     = glyph.metrics.horiBearingY/64; // + border; // spans.bounds.ymax;
            g.metrics.advance = glyph.metrics.horiAdvance/64;

            g.xoff = -spans.bounds.xmin;
            g.yoff = -spans.bounds.ymin;

            // TODO: write a test to check if margin is enough to handle xoff and yoff

            // prepare image of glyph
            // g.image.resize(spans.bounds.width(), spans.bounds.height());
            for (auto &s: spans.spans)
            {
                // using namespace tmfont::io;
                // std::cout << s << std::endl;
                int i = s.y - spans.bounds.ymin;
                int j = s.x - spans.bounds.xmin;
                // int linear_index = (img_height - 1 - yy) * img_width + xx;
                for (int jj = 0; jj < s.width; ++jj) {
                    g.image(i, j + jj) = s.coverage;
                    // g.image(height - 1 - i, j + jj) = s.coverage;
                }
                // printf("\n");
            }


                    // int left         =  spans.bounds.xmin;       // glyph._metrics.horiBearingX/64; // - border;
                    // int top          =  spans.bounds.ymax + 1;   // glyph._metrics.horiBearingY/64; // + border; // spans.bounds.ymax;

//            // FT_Bitmap bitmap = slot->bitmap;
//            int width        =  glyph._metrics.width;  // spans.bounds.width();    // these include borders
//            int height       =  glyph._metrics.height; // spans.bounds.height();
//            // int left         =  spans.bounds.xmin;       // glyph._metrics.horiBearingX/64; // - border;
//            // int top          =  spans.bounds.ymax + 1;   // glyph._metrics.horiBearingY/64; // + border; // spans.bounds.ymax;
//            int left         =  glyph._metrics.horiBearingX/64; // - border;
//            int top          =  glyph._metrics.horiBearingY/64; // + border; // spans.bounds.ymax;
//            int advance      =  glyph._metrics.horiAdvance/64;

//            int xoff         = -span.bounds.xmin;
//            int yoff         = -span.bounds.ymin; // height - (span.bounds.ymax + 1); // + (g->image.height - g->top);

//            printf("width:      %4d\n",width);
//            printf("height:     %4d\n",height);
//            printf("left:       %4d\n",left);
//            printf("top:        %4d\n",top);
//            printf("advance:    %4d\n",advance);
//            printf("pitch:      %4d\n",bitmap.pitch);
//            printf("pixel_mode: %4d\n",bitmap.pixel_mode);

            // create glyph

        }
    }

    int glyph_good_width, glyph_good_height;

    glyph_list.getGoodGlyphSize(0, glyph_good_width, glyph_good_height);

    std::cout << "good size: " << glyph_good_width << ", " << glyph_good_height << std::endl;

    glyph_list.save(img_width, margin, false, output_filename);

    return 0;

}



//    FT_Done_Face    ( face );
//    FT_Done_FreeType( library );

//#if 1
//#else
//            Glyph &g = glyph_list.addGlyph(code,width,height,left,top,advance);
//            for (int i=0;i<height;i++) {
//                for (int j=0;j<pitch;j++) {
//                    Pixel value = slot->bitmap.buffer[i * pitch + j];
//                    if (j * 8 > width)
//                        break; // row is over
//                    printf("%02x ",value);
//                    for (int k=0;k<8;k++) {
//                        if (8 * j + k >= width)
//                            break;
//                        Pixel pk = (value & (1 << (7 - k)) ? 255 : 0);
//                        // printf("%02x ",pk);
//                        g.image(i,j + k) = pk;
//                    }
//                }
//                printf("\n");
//            }
//#endif
