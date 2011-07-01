// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/ColorTable.hpp>

#include <sfz/sfz.hpp>

using sfz::BytesSlice;
using sfz::Exception;
using sfz::Json;
using sfz::ReadSource;
using sfz::StringMap;
using sfz::format;
using sfz::range;
using sfz::read;
using std::make_pair;
using std::vector;

namespace rezin {

ColorTable::ColorTable() { }

ColorTable::ColorTable(BytesSlice in) {
    read(&in, this);
    if (!in.empty()) {
        throw Exception(format("{0} extra bytes at end of 'clut' resource.", in.size()));
    }
}

void read_from(ReadSource in, ColorTable* out) {
    read(&in, &out->seed);
    read(&in, &out->flags);
    read(&in, &out->size);
    out->table.resize(out->size + 1);
    SFZ_FOREACH(uint32_t i, range(uint32_t(out->size) + 1), {
        read(&in, &out->table[i]);
    });
}

Json json(const ColorTable& color_table) {
    vector<Json> specs;
    SFZ_FOREACH(const ColorSpec& spec, color_table.table, {
        specs.push_back(json(spec));
    });
    return Json::array(specs);
}

void read_from(ReadSource in, ColorSpec* out) {
    read(in, &out->id);
    read(in, &out->red);
    read(in, &out->green);
    read(in, &out->blue);
}

sfz::Json json(const ColorSpec& spec) {
    StringMap<Json> attributes;
    attributes.insert(make_pair("id", Json::number(spec.id)));
    attributes.insert(make_pair("red", Json::number(spec.red)));
    attributes.insert(make_pair("green", Json::number(spec.green)));
    attributes.insert(make_pair("blue", Json::number(spec.blue)));
    return Json::object(attributes);
}

}  // namespace rezin
