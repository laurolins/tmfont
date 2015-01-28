#include <image.hh>

#include <FreeImage.h>

#include <iostream>
#include <fstream>

//-----------------------------------------------------------------------------
// Image Impl
//-----------------------------------------------------------------------------

Image::Image():
    width(0), height(0)
{}

void Image::resize(int w, int h) {
    buffer.resize(w * h);
    width  = w;
    height = h;
    std::fill(buffer.begin(),buffer.end(),0);
}

Pixel &Image::operator()(int i, int j) {
    return buffer[i * width + j];
}

const Pixel &Image::operator()(int i, int j) const {
    return buffer[i * width + j];
}

void Image::setRegion(Image &subimg, int i0, int j0) {
    Image &img = *this;
    for (int i = 0; i < subimg.height; i++) {
        for (int j = 0; j < subimg.width; j++) {
            // std::cout << (int) subimg(i,j) << "   ";
            img(i0 + i, j0 + j) = subimg(i,j);
        }
        // std::cout  << std::endl;
    }
}

void Image::save(std::string filename) const {
    int bytes_per_pixel = 32;
    FIBITMAP* bitmap = FreeImage_Allocate(this->width, this->height, bytes_per_pixel);
    RGBQUAD color;

    const Image &img = *this;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Pixel pixel = img(i, j);
            // std::cout << std::setw(3) << (int) pixel << " ";
            color.rgbRed = color.rgbGreen = color.rgbBlue = 255;
            color.rgbReserved = pixel;
            // color.rgbReserved = 255;
            // FreeImage_SetPixelColor(bitmap, j , height - 1 - i , &color);
            FreeImage_SetPixelColor(bitmap, j , i , &color);
        }
        // std::cout << std::endl;
    }

    if (FreeImage_Save(FIF_PNG, bitmap, filename.c_str(), 0))
        std::cerr << "Image " << filename << " was successfully saved" << std::endl;
}
