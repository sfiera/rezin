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
#include <rezin/image.hpp>
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
using sfz::scoped_ptr;
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
    scoped_ptr<RasterImage> mask_bitmap_image;
    scoped_ptr<RasterImage> icon_bitmap_image;
    ColorTable color_table;
    scoped_ptr<RasterImage> icon_pixmap_image;
};

void read_from(ReadSource in, ColorIcon::Rep& rep) {
    read(in, rep.icon_pixmap);
    read(in, rep.mask_bitmap);
    read(in, rep.icon_bitmap);
    read(in, rep.icon_data);

    const AlphaColor black(0, 0, 0);
    const AlphaColor white(255, 255, 255);
    const AlphaColor clear(0, 0, 0, 0);
    rep.mask_bitmap.read_image(in, black, clear, rep.mask_bitmap_image);
    rep.icon_bitmap.read_image(in, black, white, rep.icon_bitmap_image);
    read(in, rep.color_table);
    rep.icon_pixmap.read_indexed_image(in, rep.color_table, rep.icon_pixmap_image);
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

void write_to(WriteTarget out, PngColorIcon png_cicn) {
    const ColorIcon::Rep& rep = *png_cicn.cicn.rep;
    RasterImage composite(rep.mask_bitmap.bounds);
    composite.src(*rep.icon_pixmap_image, *rep.mask_bitmap_image);
    write(out, png(composite));
}

}  // namespace rezin
