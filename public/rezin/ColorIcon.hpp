// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_COLOR_ICON_HPP_
#define REZIN_COLOR_ICON_HPP_

#include <sfz/sfz.hpp>
#include <rezin/BasicTypes.hpp>
#include <rezin/ColorTable.hpp>

namespace rezin {

struct PngColorIcon;

struct ColorIcon {
    ColorIcon(sfz::BytesSlice in);

    PixMap icon_pixmap;
    BitMap mask_bitmap;
    BitMap icon_bitmap;
    uint32_t icon_data;
    std::vector<uint8_t> mask_bitmap_pixels;
    std::vector<uint8_t> icon_bitmap_pixels;
    ColorTable color_table;
    std::vector<uint8_t> icon_pixmap_pixels;
};

PngColorIcon png(const ColorIcon& cicn);

struct PngColorIcon { const ColorIcon& cicn; };
void write_to(sfz::WriteTarget out, PngColorIcon png);

}  // namespace rezin

#endif // REZIN_COLOR_ICON_HPP_
