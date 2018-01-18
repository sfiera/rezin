// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/cicn.hpp>

#include <algorithm>
#include <map>
#include <rezin/bits-slice.hpp>
#include <rezin/clut.hpp>
#include <rezin/image.hpp>
#include <rezin/primitives.hpp>
#include <sfz/sfz.hpp>
#include <vector>

using sfz::StringMap;
using sfz::range;
using std::make_pair;
using std::map;
using std::swap;
using std::unique_ptr;
using std::vector;

namespace rezin {

struct ColorIcon::Rep {
    AddressedPixMap         icon_pixmap;
    BitMap                  mask_bitmap;
    BitMap                  icon_bitmap;
    uint32_t                icon_data;
    unique_ptr<RasterImage> mask_bitmap_image;
    unique_ptr<RasterImage> icon_bitmap_image;
    ColorTable              color_table;
    unique_ptr<RasterImage> icon_pixmap_image;
};

void read_from(pn::file_view in, ColorIcon::Rep* rep) {
    read_from(in, &rep->icon_pixmap);
    read_from(in, &rep->mask_bitmap);
    read_from(in, &rep->icon_bitmap);
    in.read(&rep->icon_data).check();

    const AlphaColor black(0, 0, 0);
    const AlphaColor white(255, 255, 255);
    const AlphaColor clear(0, 0, 0, 0);
    rep->mask_bitmap_image = rep->mask_bitmap.read_image(in, black, clear);
    rep->icon_bitmap_image = rep->icon_bitmap.read_image(in, black, white);
    read_from(in, &rep->color_table);
    rep->icon_pixmap_image = rep->icon_pixmap.read_image(in, rep->color_table);
}

ColorIcon::ColorIcon(pn::data_view in) : rep(new Rep) {
    pn::file f = in.open();
    read_from(f, rep.get());
    if (!f.read(pn::pad(1)).eof()) {
        throw std::runtime_error("extra bytes at end of 'cicn' resource.");
    }
}

ColorIcon::~ColorIcon() {}
pn::data png(const ColorIcon& cicn) {
    const ColorIcon::Rep& rep = *cicn.rep;
    RasterImage           composite(rep.mask_bitmap.bounds);
    composite.src(*rep.icon_pixmap_image, *rep.mask_bitmap_image);
    return png(composite);
}

}  // namespace rezin
