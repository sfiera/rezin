// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/StringList.hpp"

#include <vector>
#include "rezin/MacRoman.hpp"
#include "rgos/Json.hpp"
#include "sfz/Foreach.hpp"
#include "sfz/Range.hpp"
#include "sfz/ReadItem.hpp"
#include "sfz/ReadSource.hpp"
#include "sfz/SmartPtr.hpp"

using rgos::Json;
using sfz::Bytes;
using sfz::BytesPiece;
using sfz::ReadItem;
using sfz::ReadSource;
using sfz::String;
using sfz::range;
using sfz::read;
using sfz::scoped_ptr;
using std::vector;

namespace rezin {

Json read_string_list(const BytesPiece& data) {
    BytesPiece in(data);
    uint16_t array_size;
    read(&in, &array_size);

    vector<Json> result;
    foreach (i, range(array_size)) {
        uint8_t data[256];
        uint8_t size;
        read(&in, &size);
        read(&in, data, size);
        Bytes utf8;
        String string(BytesPiece(data, size), mac_roman_encoding());
        cr2nl(&string);
        result.push_back(Json::string(string));
    }
    return Json::array(result);
}

}  // namespace rezin
