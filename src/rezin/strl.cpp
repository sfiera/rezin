// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/strl.hpp>

#include <vector>
#include <rezin/options.hpp>
#include <sfz/sfz.hpp>

using sfz::Bytes;
using sfz::BytesSlice;
using sfz::Exception;
using sfz::Json;
using sfz::String;
using sfz::format;
using sfz::linked_ptr;
using sfz::make_linked_ptr;
using sfz::range;
using sfz::read;
using std::vector;

namespace rezin {

StringList::StringList(BytesSlice in, const Options& options) {
    uint16_t array_size;
    read(in, array_size);

    SFZ_FOREACH(uint16_t i, range(array_size), {
        uint8_t data[255];
        uint8_t size;
        read(in, size);
        read(in, data, size);
        Bytes utf8;
        linked_ptr<const String> string(new String(options.decode(BytesSlice(data, size))));
        strings.push_back(string);
    });

    if (!in.empty()) {
        throw Exception(format("{0} extra bytes at end of 'STR#' resource.", in.size()));
    }
}

Json json(const StringList& strings) {
    vector<Json> array;
    SFZ_FOREACH(const linked_ptr<const String>& string, strings.strings, {
        array.push_back(Json::string(*string));
    });
    return Json::array(array);
}

}  // namespace rezin
