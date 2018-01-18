// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_PRIMITIVES_HPP_
#define REZIN_PRIMITIVES_HPP_

#include <stdint.h>
#include <sfz/sfz.hpp>

namespace rezin {

struct AlphaColor;
class RasterImage;
struct ColorTable;

struct Rect {
    int16_t top;
    int16_t left;
    int16_t bottom;
    int16_t right;

    int16_t width() const;
    int16_t height() const;

    pn::value to_value() const;
};

bool operator==(const Rect& x, const Rect& y);
bool operator!=(const Rect& x, const Rect& y);
void read_from(pn::file_view in, Rect* out);

struct fixed32_t {
    int32_t int_value;

    double    to_double() const;
    pn::value to_value() const;
};
void read_from(pn::file_view in, fixed32_t* out);

struct PixMap {
    int16_t   row_bytes;
    Rect      bounds;
    int16_t   pm_version;
    int16_t   pack_type;
    int32_t   pack_size;
    fixed32_t h_res;
    fixed32_t v_res;
    int16_t   pixel_type;
    int16_t   pixel_size;
    int16_t   cmp_count;
    int16_t   cmp_size;
    int32_t   plane_bytes;
    uint32_t  pm_table;
    int32_t   pm_reserved;

    std::unique_ptr<RasterImage> read_image(pn::file_view in, const ColorTable& clut) const;
    std::unique_ptr<RasterImage> read_packed_image(pn::file_view in, const ColorTable& clut) const;
    std::unique_ptr<RasterImage> read_direct_image(pn::file_view in) const;
};
void read_from(pn::file_view in, PixMap* out);

struct AddressedPixMap : PixMap {
    uint32_t base_addr;
};
void read_from(pn::file_view in, AddressedPixMap* out);

struct BitMap {
    uint32_t base_addr;
    uint16_t row_bytes;
    Rect     bounds;

    pn::value                    to_value() const;
    std::unique_ptr<RasterImage> read_image(pn::file_view in, AlphaColor on, AlphaColor off) const;
};
void read_from(pn::file_view in, BitMap* out);

}  // namespace rezin

#endif  // REZIN_PRIMITIVES_HPP_
