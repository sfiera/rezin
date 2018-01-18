// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/strl.hpp>

#include <rezin/options.hpp>
#include <sfz/sfz.hpp>
#include <vector>

using sfz::range;
using std::vector;

namespace rezin {

StringList::StringList(pn::data_view in, const Options& options) {
    pn::file f = in.open();
    uint16_t array_size;
    f.read(&array_size).check();

    for (uint16_t i : range(array_size)) {
        uint8_t size;
        f.read(&size).check();

        pn::data data;
        data.resize(size);
        f.read(&data).check();

        strings.push_back(options.decode(data));
    }

    if (!f.read(pn::pad(1)).eof()) {
        throw std::runtime_error(pn::format("extra bytes at end of 'STR#' resource.").c_str());
    }
}

pn::value value(const StringList& strings) {
    pn::array a;
    for (const pn::string& string : strings.strings) {
        a.push_back(string.copy());
    }
    return std::move(a);
}

}  // namespace rezin
