// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/ResourceFork.hpp>

#include <rezin/ResourceType.hpp>
#include <sfz/sfz.hpp>

using sfz::BytesSlice;
using sfz::Exception;
using sfz::StringMap;
using sfz::StringSlice;
using sfz::format;
using sfz::linked_ptr;
using sfz::quote;
using sfz::range;
using sfz::read;

namespace rezin {

ResourceFork::ResourceFork(const BytesSlice& data, const Options& options) {
    // Resource header.
    BytesSlice header(data.slice(0, 16));
    uint32_t data_offset;
    uint32_t map_offset;
    uint32_t data_length;
    uint32_t map_length;
    read(&header, &data_offset);
    read(&header, &map_offset);
    read(&header, &data_length);
    read(&header, &map_length);

    BytesSlice map_data = data.slice(map_offset, map_length);
    BytesSlice data_data = data.slice(data_offset, data_length);

    // Map header.
    BytesSlice map(map_data.slice(16, 14));
    uint16_t type_offset;
    uint16_t name_offset;
    uint16_t type_count;
    map.shift(8);
    read(&map, &type_offset);
    read(&map, &name_offset);
    read(&map, &type_count);
    ++type_count;

    BytesSlice type_data = map_data.slice(type_offset);
    BytesSlice name_data = map_data.slice(name_offset);

    SFZ_FOREACH(uint16_t i, range(type_count), {
        linked_ptr<ResourceType> type(
                new ResourceType(type_data, i, name_data, data_data, options));
        _types[StringSlice(type->code())] = type;
    });
}

ResourceFork::~ResourceFork() { }

const ResourceType& ResourceFork::at(const StringSlice& code) const {
    StringMap<linked_ptr<ResourceType> >::const_iterator it = _types.find(code);
    if (it == _types.end()) {
        throw Exception(format("no such resource type '{0}'", code));
    }
    return *it->second;
}

ResourceFork::const_iterator ResourceFork::begin() const {
    return const_iterator(_types.begin());
}

ResourceFork::const_iterator ResourceFork::end() const {
    return const_iterator(_types.end());
}

}  // namespace rezin
