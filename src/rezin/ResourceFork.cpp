// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/ResourceFork.hpp"

#include <stdexcept>
#include "rezin/MacRoman.hpp"
#include "rezin/ResourceType.hpp"
#include "sfz/BinaryReader.hpp"
#include "sfz/Foreach.hpp"
#include "sfz/Format.hpp"
#include "sfz/Formatter.hpp"
#include "sfz/Range.hpp"
#include "sfz/SmartPtr.hpp"

using sfz::Bytes;
using sfz::BytesBinaryReader;
using sfz::BytesPiece;
using sfz::Exception;
using sfz::FormatItem;
using sfz::StringKey;
using sfz::StringPiece;
using sfz::ascii_encoding;
using sfz::quote;
using sfz::range;
using sfz::scoped_ptr;
using std::map;

namespace rezin {

ResourceFork::ResourceFork(const BytesPiece& data) {
    // Resource header.
    BytesBinaryReader header_bin(data.substr(0, 16));
    uint32_t data_offset;
    uint32_t map_offset;
    uint32_t data_length;
    uint32_t map_length;
    header_bin.read(&data_offset);
    header_bin.read(&map_offset);
    header_bin.read(&data_length);
    header_bin.read(&map_length);

    BytesPiece map_data = data.substr(map_offset, map_length);
    BytesPiece data_data = data.substr(data_offset, data_length);

    // Map header.
    BytesBinaryReader map_bin(map_data.substr(16, 14));
    uint16_t type_offset;
    uint16_t name_offset;
    uint16_t type_count;
    map_bin.discard(8);
    map_bin.read(&type_offset);
    map_bin.read(&name_offset);
    map_bin.read(&type_count);
    ++type_count;

    BytesPiece type_data = map_data.substr(type_offset);
    BytesPiece name_data = map_data.substr(name_offset);

    foreach (i, range(type_count)) {
        scoped_ptr<ResourceType> type(new ResourceType(type_data, i, name_data, data_data));
        StringKey key(type->code());
        if (_types.find(key) != _types.end()) {
            throw Exception("duplicate resource type in resource fork {0}", quote(type->code()));
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
        throw Exception("no such resource type '{0}'", code);
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
