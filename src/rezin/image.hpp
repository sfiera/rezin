// Copyright (c) 2011 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_IMAGE_HPP_
#define REZIN_IMAGE_HPP_

#include <vector>
#include <sfz/sfz.hpp>
#include <rezin/primitives.hpp>

namespace rezin {

struct PngRasterImage;

struct AlphaColor {
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    AlphaColor():
            alpha(0),
            red(0),
            green(0),
            blue(0) { }

    AlphaColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255):
            alpha(alpha),
            red(red),
            green(green),
            blue(blue) { }
};

class Image {
  public:
    Image();

  private:
    DISALLOW_COPY_AND_ASSIGN(Image);
};

class RasterImage : public Image {
  public:
    explicit RasterImage(const Rect& bounds);

    const Rect& bounds() const;
    bool contains(int16_t x, int16_t y) const;
    void set(int16_t x, int16_t y, const AlphaColor& color);
    AlphaColor get(int16_t x, int16_t y) const;

    void src(const RasterImage& src, const RasterImage& mask);

  private:
    size_t index(int16_t x, int16_t y) const;

    const Rect _bounds;
    std::vector<AlphaColor> _pixels;

    DISALLOW_COPY_AND_ASSIGN(RasterImage);
};

PngRasterImage png(const RasterImage& image);

struct PngRasterImage {
    const RasterImage& image;
};
void write_to(sfz::WriteTarget out, const PngRasterImage& png);

}  // namespace rezin

#endif // REZIN_IMAGE_HPP_
