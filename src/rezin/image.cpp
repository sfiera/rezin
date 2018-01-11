// Copyright (c) 2011 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/image.hpp>

#include <algorithm>
#include <rezin/png.hpp>

using sfz::WriteTarget;
using std::max;
using std::min;

namespace rezin {

Image::Image(Rect bounds) : _bounds(bounds) {}

Image::~Image() {}

const Rect& Image::bounds() const { return _bounds; }

bool Image::contains(int16_t x, int16_t y) const {
    return (x >= _bounds.left) && (x < _bounds.right) && (y >= _bounds.top) &&
           (y < _bounds.bottom);
}

RectImage::RectImage(Rect bounds, AlphaColor color) : Image(bounds), _color(color) {}

AlphaColor RectImage::get(int16_t x, int16_t y) const {
    if (contains(x, y)) {
        return _color;
    }
    return AlphaColor();
}

RasterImage::RasterImage(Rect bounds)
        : Image(bounds), _pixels(bounds.width() * bounds.height(), AlphaColor()) {}

void RasterImage::set(int16_t x, int16_t y, const AlphaColor& color) {
    if (contains(x, y)) {
        _pixels[index(x, y)] = color;
    }
}

AlphaColor RasterImage::get(int16_t x, int16_t y) const {
    if (contains(x, y)) {
        return _pixels[index(x, y)];
    } else {
        return AlphaColor();
    }
}

void RasterImage::src(const Image& src, const Image& mask) {
    Rect area = {
            max(max(src.bounds().top, mask.bounds().top), bounds().top),
            max(max(src.bounds().left, mask.bounds().left), bounds().left),
            min(min(src.bounds().bottom, mask.bounds().bottom), bounds().bottom),
            min(min(src.bounds().right, mask.bounds().right), bounds().right),
    };
    for (int16_t y = area.top; y < area.bottom; ++y) {
        for (int16_t x = area.left; x < area.right; ++x) {
            if (mask.get(x, y).alpha) {
                set(x, y, src.get(x, y));
            }
        }
    }
}

size_t RasterImage::index(int16_t x, int16_t y) const {
    x -= bounds().left;
    y -= bounds().top;
    return x + (y * bounds().width());
}

PngRasterImage png(const RasterImage& image) {
    PngRasterImage png = {image};
    return png;
}

void write_to(WriteTarget out, const PngRasterImage& png) {
    PngWriter writer(out, png.image.bounds().width(), png.image.bounds().height());
    for (int16_t y = png.image.bounds().top; y < png.image.bounds().bottom; ++y) {
        for (int16_t x = png.image.bounds().left; x < png.image.bounds().right; ++x) {
            const AlphaColor& color = png.image.get(x, y);
            writer.append_pixel(color.red, color.green, color.blue, color.alpha);
        }
    }
}

namespace {

Rect translate_rect(Rect r, int16_t dx, int16_t dy) {
    Rect result = {
            int16_t(r.top + dy), int16_t(r.left + dx), int16_t(r.bottom + dy),
            int16_t(r.right + dx),
    };
    return result;
}

}  // namespace

TranslatedImage::TranslatedImage(const Image& image, int16_t dx, int16_t dy)
        : Image(translate_rect(image.bounds(), dx, dy)), _image(image) {}

AlphaColor TranslatedImage::get(int16_t x, int16_t y) const {
    return _image.get(
            x + _image.bounds().left - bounds().left, y + _image.bounds().top - bounds().top);
}

}  // namespace rezin
