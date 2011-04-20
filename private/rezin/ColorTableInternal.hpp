// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_COLOR_TABLE_INTERNAL_HPP_
#define REZIN_COLOR_TABLE_INTERNAL_HPP_

#include <stdint.h>
#include <vector>
#include <sfz/sfz.hpp>

namespace rezin {

struct RgbColor {
    uint16_t red;
    uint16_t green;
    uint16_t blue;

    sfz::Json to_json() const;
};
void read_from(sfz::ReadSource in, RgbColor* out);

struct ColorSpec {
    uint16_t value;
    RgbColor rgb;

    sfz::Json to_json() const;
};
void read_from(sfz::ReadSource in, ColorSpec* out);

struct ColorTable {
    uint32_t ct_seed;
    uint16_t ct_flags;
    uint16_t ct_size;
    std::vector<ColorSpec> ct_table;

    sfz::Json to_json() const;
};
void read_from(sfz::ReadSource in, ColorTable* out);

}  // namespace rezin

#endif // REZIN_COLOR_TABLE_INTERNAL_HPP_
