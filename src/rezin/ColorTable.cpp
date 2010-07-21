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
using sfz::format;
using sfz::range;
using sfz::read;
using std::make_pair;
using std::vector;

namespace rezin {

namespace {

// Reads a single ColorSpec entry from a 'clut' resource.
Json read_color_spec(BytesPiece* in) {
    StringMap<Json> result;
    uint16_t id, red, green, blue;
    read(in, &id);
    read(in, &red);
    read(in, &green);
    read(in, &blue);
    result.insert(make_pair("id", Json::number(id)));
    result.insert(make_pair("red", Json::number(red)));
    result.insert(make_pair("green", Json::number(green)));
    result.insert(make_pair("blue", Json::number(blue)));
    return Json::object(result);
}

}  // namespace

Json read_clut(const BytesPiece& in) {
    vector<Json> result;
    BytesPiece remainder(in);

    uint32_t seed;
    uint16_t flags;
    uint16_t size;
    read(&remainder, &seed);
    read(&remainder, &flags);
    read(&remainder, &size);
    foreach (i, range(size + 1)) {
        result.push_back(read_color_spec(&remainder));
    }

    if (!remainder.empty()) {
        throw Exception(format("{0} extra bytes at end of 'clut' resource.", remainder.size()));
    }
    return Json::array(result);
}

}  // namespace rezin
