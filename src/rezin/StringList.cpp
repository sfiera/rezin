// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/StringList.hpp"

#include <vector>
#include "rezin/MacRoman.hpp"
#include "rgos/Json.hpp"
#include "sfz/BinaryReader.hpp"
#include "sfz/Foreach.hpp"
#include "sfz/Range.hpp"
#include "sfz/SmartPtr.hpp"

using rgos::Json;
using sfz::Bytes;
using sfz::BytesBinaryReader;
using sfz::BytesPiece;
using sfz::String;
using sfz::range;
using sfz::scoped_ptr;
using std::vector;

namespace rezin {

Json read_string_list(const BytesPiece& data) {
    BytesBinaryReader bin(data);
    uint16_t array_size;
    bin.read(&array_size);

    vector<Json> result;
    foreach (i, range(array_size)) {
        uint8_t data[256];
        uint8_t size;
        bin.read(&size);
        bin.read(data, size);
        Bytes utf8;
        String string(BytesPiece(data, size), mac_roman_encoding());
        cr2nl(&string);
        result.push_back(Json::string(string));
    }
    return Json::array(result);
}

}  // namespace rezin
