#pragma once

#include <string>
#include <cstdint>
#include <vector>

using Pixel = uint8_t;

struct Image {

    Image();

//    Image(const Image& img) = default;
//    auto operator=(const Image& img) -> Image& = default;
//    Image(Image&& img) = default;
//    auto operator=(Image&& img) -> Image& = default;

    void resize(int w, int h);

    Pixel& operator()(int i, int j);

    const Pixel& operator()(int i, int j) const;

    void setRegion(Image &subimg, int i0, int j0);

    void save(std::string filename) const;

    int      width   { 0 };
    int      height  { 0 };
    std::vector<Pixel> buffer;

};
