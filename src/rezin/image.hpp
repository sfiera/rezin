// Copyright (c) 2011 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_IMAGE_HPP_
#define REZIN_IMAGE_HPP_

#include <rezin/primitives.hpp>
#include <sfz/sfz.hpp>
#include <vector>

namespace rezin {

struct PngRasterImage;

struct AlphaColor {
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    AlphaColor() : alpha(0), red(0), green(0), blue(0) {}

    AlphaColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
            : alpha(alpha), red(red), green(green), blue(blue) {}
};

class Image {
  public:
    Image(Rect bounds);
    virtual ~Image();

    const Rect&        bounds() const;
    bool               contains(int16_t x, int16_t y) const;
    virtual AlphaColor get(int16_t x, int16_t y) const = 0;

  private:
    const Rect _bounds;

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
};

class RectImage : public Image {
  public:
    RectImage(Rect bounds, AlphaColor color);

    virtual AlphaColor get(int16_t x, int16_t y) const;

  private:
    AlphaColor _color;
};

class RasterImage : public Image {
  public:
    explicit RasterImage(Rect bounds);

    virtual AlphaColor get(int16_t x, int16_t y) const;

    void set(int16_t x, int16_t y, const AlphaColor& color);

    void src(const Image& src, const Image& mask);

  private:
    size_t index(int16_t x, int16_t y) const;

    std::vector<AlphaColor> _pixels;
};

pn::data png(const RasterImage& image);

class TranslatedImage : public Image {
  public:
    TranslatedImage(const Image& image, int16_t dx, int16_t dy);

    virtual AlphaColor get(int16_t x, int16_t y) const;

  private:
    const Image& _image;
};

}  // namespace rezin

#endif  // REZIN_IMAGE_HPP_
