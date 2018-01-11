// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/strl.hpp>

#include <rezin/options.hpp>
#include <sfz/sfz.hpp>
#include <vector>

using sfz::Bytes;
using sfz::BytesSlice;
using sfz::Exception;
using sfz::Json;
using sfz::String;
using sfz::format;
using sfz::range;
using sfz::read;
using std::shared_ptr;
using std::vector;

namespace rezin {

StringList::StringList(BytesSlice in, const Options& options) {
    uint16_t array_size;
    read(in, array_size);

    for (uint16_t i : range(array_size)) {
        static_cast<void>(i);
        uint8_t data[255];
        uint8_t size;
        read(in, size);
        read(in, data, size);
        Bytes                    utf8;
        shared_ptr<const String> string(new String(options.decode(BytesSlice(data, size))));
        strings.push_back(string);
    }

    if (!in.empty()) {
        throw Exception(format("{0} extra bytes at end of 'STR#' resource.", in.size()));
    }
}

Json json(const StringList& strings) {
    vector<Json> array;
    for (const shared_ptr<const String>& string : strings.strings) {
        array.push_back(Json::string(*string));
    }
    return Json::array(array);
}

}  // namespace rezin
