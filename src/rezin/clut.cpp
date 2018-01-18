// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/clut.hpp>

#include <sfz/sfz.hpp>

using sfz::range;

namespace rezin {

ColorTable::ColorTable() {}

ColorTable::ColorTable(pn::data_view in) {
    pn::file f = in.open();
    read_from(f, this);
    if (!f.read(pn::pad(1)).eof()) {
        throw std::runtime_error("extra bytes at end of 'clut' resource.");
    }
}

void read_from(pn::file_view in, ColorTable* out) {
    in.read(&out->seed, &out->flags, &out->size).check();
    for (uint32_t i : range(uint32_t(out->size) + 1)) {
        in.read(pn::pad(2)).check();
        read_from(in, &out->table[i]);
    }
}

pn::value value(const ColorTable& color_table) {
    pn::map                            m;
    typedef std::pair<uint16_t, Color> pair;
    for (const pair& p : color_table.table) {
        pn::string key = pn::format("{0}", p.first);
        m[key]         = value(p.second);
    }
    return std::move(m);
}

void read_from(pn::file_view in, Color* out) {
    in.read(&out->red, &out->green, &out->blue).check();
}

pn::value value(const Color& spec) {
    return pn::map{
            {"r", spec.red / 65535.0 * 255},
            {"g", spec.green / 65535.0 * 255},
            {"b", spec.blue / 65535.0 * 255},
    };
}

}  // namespace rezin
