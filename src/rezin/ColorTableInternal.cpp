// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/ColorTableInternal.hpp>

#include <sfz/sfz.hpp>

using sfz::Exception;
using sfz::Json;
using sfz::ReadSource;
using sfz::StringMap;
using sfz::format;
using sfz::range;
using sfz::read;
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
    SFZ_FOREACH(uint32_t i, range(uint32_t(ct_size) + 1), {
        result.push_back(ct_table[i].to_json());
    });
    return Json::array(result);
}

void read_from(ReadSource in, ColorTable* out) {
    read(in, &out->ct_seed);
    read(in, &out->ct_flags);
    read(in, &out->ct_size);
    out->ct_table.resize(out->ct_size + 1);
    SFZ_FOREACH(uint32_t i, range(uint32_t(out->ct_size) + 1), {
        read(in, &out->ct_table[i]);
    });
}

}  // namespace rezin
