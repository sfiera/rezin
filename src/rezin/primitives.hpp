// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_PRIMITIVES_HPP_
#define REZIN_PRIMITIVES_HPP_

#include <stdint.h>
#include <sfz/sfz.hpp>

namespace rezin {

class AlphaColor;
class RasterImage;
class ColorTable;

struct Rect {
    int16_t top;
    int16_t left;
    int16_t bottom;
    int16_t right;

    int16_t width() const;
    int16_t height() const;

    sfz::Json to_json() const;
};
void read_from(sfz::ReadSource in, Rect& out);

struct fixed32_t {
    int32_t int_value;

    double to_double() const;
    sfz::Json to_json() const;
};
void read_from(sfz::ReadSource in, fixed32_t& out);

struct PixMap {
    int16_t row_bytes;
    Rect bounds;
    int16_t pm_version;
    int16_t pack_type;
    int32_t pack_size;
    fixed32_t h_res;
    fixed32_t v_res;
    int16_t pixel_type;
    int16_t pixel_size;
    int16_t cmp_count;
    int16_t cmp_size;
    int32_t plane_bytes;
    uint32_t pm_table;
    int32_t pm_reserved;

    void read_indexed_image(
            sfz::ReadSource in, const ColorTable& clut, sfz::scoped_ptr<RasterImage>& image) const;
};
void read_from(sfz::ReadSource in, PixMap& out);

struct AddressedPixMap : PixMap {
    uint32_t base_addr;
};
void read_from(sfz::ReadSource in, AddressedPixMap& out);

struct BitMap {
    uint32_t base_addr;
    uint16_t row_bytes;
    Rect bounds;

    sfz::Json to_json() const;
    void read_image(
            sfz::ReadSource in, AlphaColor on, AlphaColor off,
            sfz::scoped_ptr<RasterImage>& image) const;
};
void read_from(sfz::ReadSource in, BitMap& out);

}  // namespace rezin

#endif // REZIN_PRIMITIVES_HPP_
