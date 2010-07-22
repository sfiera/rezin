// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/ColorTable.hpp"

#include <rgos/rgos.hpp>
#include <sfz/sfz.hpp>

using rgos::Json;
using rgos::StringMap;
using sfz::BytesPiece;
using sfz::Exception;
using sfz::ReadSource;
using sfz::format;
using sfz::range;
using sfz::read;
using std::make_pair;
using std::vector;

namespace rezin {

Json RgbColor::to_json() const {
    StringMap<Json> result;
    result["red"]   = Json::number(red);
    result["green"] = Json::number(green);
    result["blue"]  = Json::number(blue);
    return Json::object(result);
}

void read_from(ReadSource in, RgbColor* out) {
    read(in, &out->red);
    read(in, &out->green);
    read(in, &out->blue);
}

Json ColorSpec::to_json() const {
    StringMap<Json> result;
    result["id"]    = Json::number(value);
    result["red"]   = Json::number(rgb.red);
    result["green"] = Json::number(rgb.green);
    result["blue"]  = Json::number(rgb.blue);
    return Json::object(result);
}

void read_from(ReadSource in, ColorSpec* out) {
    read(in, &out->value);
    read(in, &out->rgb);
}

Json ColorTable::to_json() const {
    vector<Json> result;
    foreach (i, range(ct_size + 1)) {
        result.push_back(ct_table[i].to_json());
    }
    return Json::array(result);
}

void read_from(ReadSource in, ColorTable* out) {
    read(in, &out->ct_seed);
    read(in, &out->ct_flags);
    read(in, &out->ct_size);
    out->ct_table.resize(out->ct_size + 1);
    foreach (i, range(out->ct_size + 1)) {
        read(in, &out->ct_table[i]);
    }
}

Json read_clut(const BytesPiece& in) {
    BytesPiece remainder(in);
    ColorTable clut;
    read(&remainder, &clut);
    if (!remainder.empty()) {
        throw Exception(format("{0} extra bytes at end of 'clut' resource.", remainder.size()));
    }
    return clut.to_json();
}

}  // namespace rezin
