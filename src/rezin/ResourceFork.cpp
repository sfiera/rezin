// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/ResourceFork.hpp"

#include <stdexcept>
#include "rezin/ResourceType.hpp"
#include "sfz/sfz.hpp"

using sfz::BytesPiece;
using sfz::Exception;
using sfz::StringKey;
using sfz::StringPiece;
using sfz::format;
using sfz::quote;
using sfz::range;
using sfz::read;
using sfz::scoped_ptr;
using std::map;

namespace rezin {

ResourceFork::ResourceFork(const BytesPiece& data) {
    // Resource header.
    BytesPiece header(data.substr(0, 16));
    uint32_t data_offset;
    uint32_t map_offset;
    uint32_t data_length;
    uint32_t map_length;
    read(&header, &data_offset);
    read(&header, &map_offset);
    read(&header, &data_length);
    read(&header, &map_length);

    BytesPiece map_data = data.substr(map_offset, map_length);
    BytesPiece data_data = data.substr(data_offset, data_length);

    // Map header.
    BytesPiece map(map_data.substr(16, 14));
    uint16_t type_offset;
    uint16_t name_offset;
    uint16_t type_count;
    map.shift(8);
    read(&map, &type_offset);
    read(&map, &name_offset);
    read(&map, &type_count);
    ++type_count;

    BytesPiece type_data = map_data.substr(type_offset);
    BytesPiece name_data = map_data.substr(name_offset);

    foreach (i, range(type_count)) {
        scoped_ptr<ResourceType> type(new ResourceType(type_data, i, name_data, data_data));
        StringKey key(type->code());
        if (_types.find(key) != _types.end()) {
            throw Exception(format(
                        "duplicate resource type in resource fork {0}", quote(type->code())));
        }
        _types[key] = type.release();
    }
}

ResourceFork::~ResourceFork() {
    foreach (it, _types) {
        delete it->second;
    }
}

const ResourceType& ResourceFork::at(const StringPiece& code) const {
    map<StringKey, ResourceType*>::const_iterator it = _types.find(code);
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
