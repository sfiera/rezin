// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_CLUT_HPP_
#define REZIN_CLUT_HPP_

#include <stdint.h>
#include <sfz/sfz.hpp>
#include <vector>

namespace rezin {

struct Color;

struct ColorTable {
    ColorTable();
    ColorTable(pn::data_view in);

    uint32_t                  seed;
    uint16_t                  flags;
    uint16_t                  size;
    std::map<uint16_t, Color> table;
};
void      read_from(pn::file_view in, ColorTable* out);
pn::value value(const ColorTable& color_table);

struct Color {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
};
void      read_from(pn::file_view in, Color* out);
pn::value value(const Color& spec);

}  // namespace rezin

#endif  // REZIN_CLUT_HPP_
