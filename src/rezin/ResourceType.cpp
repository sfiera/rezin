// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/ResourceType.hpp>

#include <rezin/ResourceEntry.hpp>
#include <sfz/sfz.hpp>

using sfz::BytesPiece;
using sfz::Exception;
using sfz::format;
using sfz::linked_ptr;
using sfz::range;
using sfz::read;
using std::map;

namespace macroman = sfz::macroman;

namespace rezin {

ResourceType::~ResourceType() { }

const sfz::String& ResourceType::code() const { return _code; }

const ResourceEntry& ResourceType::at(int16_t i) const {
    map<int16_t, linked_ptr<ResourceEntry> >::const_iterator it = _entries.find(i);
    if (it == _entries.end()) {
        throw Exception(format("no such resource entry '{0}' {1}", _code, i));
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
        const BytesPiece& data_data, const Options& options) {
    _code.assign(macroman::decode(type_data.substr(2 + index * 8, 4)));
    BytesPiece type(type_data.substr(6 + index * 8, 4));
    uint16_t count;
    uint16_t offset;
    read(&type, &count);
    read(&type, &offset);
    ++count;

    BytesPiece entry_data = type_data.substr(offset);
    foreach (i, range(count)) {
        linked_ptr<ResourceEntry> entry(
                new ResourceEntry(entry_data, i, name_data, data_data, options));
        _entries[entry->id()] = entry;
    }
}

}  // namespace rezin
