// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/cicn.hpp>

#include <algorithm>
#include <map>
#include <vector>
#include <rezin/bits-slice.hpp>
#include <rezin/clut.hpp>
#include <rezin/png.hpp>
#include <rezin/primitives.hpp>
#include <sfz/sfz.hpp>

using sfz::Bytes;
using sfz::BytesSlice;
using sfz::Exception;
using sfz::Json;
using sfz::JsonDefaultVisitor;
using sfz::ReadSource;
using sfz::StringMap;
using sfz::StringSlice;
using sfz::WriteTarget;
using sfz::format;
using sfz::range;
using sfz::read;
using std::make_pair;
using std::map;
using std::swap;
using std::vector;

namespace rezin {

struct ColorIcon::Rep {
    AddressedPixMap icon_pixmap;
    BitMap mask_bitmap;
    BitMap icon_bitmap;
    uint32_t icon_data;
    std::vector<uint8_t> mask_bitmap_pixels;
    std::vector<uint8_t> icon_bitmap_pixels;
    ColorTable color_table;
    std::vector<uint8_t> icon_pixmap_pixels;
};

void read_from(ReadSource in, ColorIcon::Rep& rep) {
    read(in, rep.icon_pixmap);
    read(in, rep.mask_bitmap);
    read(in, rep.icon_bitmap);
    read(in, rep.icon_data);

    rep.mask_bitmap.read_pixels(in, rep.mask_bitmap_pixels);
    rep.icon_bitmap.read_pixels(in, rep.icon_bitmap_pixels);
    read(in, rep.color_table);
    rep.icon_pixmap.read_pixels(in, rep.icon_pixmap_pixels);
}

ColorIcon::ColorIcon(BytesSlice in):
        rep(new Rep) {
    read(in, *rep);
    if (!in.empty()) {
        throw Exception("extra bytes at end of 'cicn' resource.");
    }
}

ColorIcon::~ColorIcon() { }
PngColorIcon png(const ColorIcon& cicn) {
    PngColorIcon png = {cicn};
    return png;
}

void write_to(WriteTarget out, PngColorIcon png) {
    const ColorIcon::Rep& rep = *png.cicn.rep;
    int16_t width = rep.icon_pixmap.bounds.width();
    int16_t height = rep.icon_pixmap.bounds.height();

    PngWriter p(out, width, height);

    SFZ_FOREACH(int16_t row, range(height), {
        SFZ_FOREACH(int16_t col, range(width), {
            uint8_t index = rep.icon_pixmap_pixels[(row * height) + col];
            uint8_t mask = rep.mask_bitmap_pixels[(row * height) + col];
            if (mask == 0) {
                p.append_pixel(0, 0, 0, 0);
            } else {
                if (rep.color_table.table.find(index) == rep.color_table.table.end()) {
                    throw Exception(format("No color with id {0}", index));
                }
                const Color& color = rep.color_table.table.find(index)->second;
                p.append_pixel(color.red, color.green, color.blue, 255);
            }
        });
    });
}

}  // namespace rezin
