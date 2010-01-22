// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/ResourceType.hpp"

#include <stdexcept>
#include "rezin/MacRoman.hpp"
#include "rezin/ResourceEntry.hpp"
#include "sfz/BinaryReader.hpp"
#include "sfz/Foreach.hpp"
#include "sfz/Format.hpp"
#include "sfz/Range.hpp"
#include "sfz/SmartPtr.hpp"

using sfz::BytesBinaryReader;
using sfz::BytesPiece;
using sfz::Exception;
using sfz::FormatItem;
using sfz::StringPiece;
using sfz::ascii_encoding;
using sfz::range;
using sfz::scoped_ptr;
using std::map;

namespace rezin {

ResourceType::~ResourceType() {
    foreach (it, _entries) {
        delete it->second;
    }
}

const sfz::String& ResourceType::code() const { return _code; }

const ResourceEntry& ResourceType::at(int16_t i) const {
    map<int16_t, ResourceEntry*>::const_iterator it = _entries.find(i);
    if (it == _entries.end()) {
        throw Exception("no such resource entry '{0}' {1}", _code, i);
    }
    return *it->second;
}

ResourceType::const_iterator ResourceType::begin() const {
    return const_iterator(_entries.begin());
}

ResourceType::const_iterator ResourceType::end() const {
    return const_iterator(_entries.end());
}

ResourceType::ResourceType(
        const BytesPiece& type_data, int index, const BytesPiece& name_data,
        const BytesPiece& data_data) {
    _code.assign(type_data.substr(2 + index * 8, 4), mac_roman_encoding());
    BytesBinaryReader bin(type_data.substr(6 + index * 8, 4));
    uint16_t count;
    uint16_t offset;
    bin.read(&count);
    bin.read(&offset);
    ++count;

    BytesPiece entry_data = type_data.substr(offset);
    foreach (i, range(count)) {
        scoped_ptr<ResourceEntry> entry(new ResourceEntry(entry_data, i, name_data, data_data));
        int16_t id = entry->id();
        _entries.insert(std::make_pair(id, entry.release()));
    }
}

}  // namespace rezin
