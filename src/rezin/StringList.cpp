// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/StringList.hpp>

#include <vector>
#include <rezin/Options.hpp>
#include <sfz/sfz.hpp>

using sfz::Bytes;
using sfz::BytesSlice;
using sfz::Json;
using sfz::String;
using sfz::range;
using sfz::read;
using std::vector;

namespace rezin {

Json read_string_list(const BytesSlice& data, const Options& options) {
    BytesSlice in(data);
    uint16_t array_size;
    read(&in, &array_size);

    vector<Json> result;
    SFZ_FOREACH(uint16_t i, range(array_size), {
        uint8_t data[255];
        uint8_t size;
        read(&in, &size);
        read(&in, data, size);
        Bytes utf8;
        String string(options.decode(BytesSlice(data, size)));
        result.push_back(Json::string(string));
    });
    return Json::array(result);
}

}  // namespace rezin
