// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/resource.hpp>

#include <rezin/options.hpp>
#include <sfz/sfz.hpp>

using sfz::BytesSlice;
using sfz::Exception;
using sfz::String;
using sfz::StringMap;
using sfz::StringSlice;
using sfz::format;
using sfz::quote;
using sfz::range;
using sfz::read;
using std::map;
using std::shared_ptr;

namespace macroman = sfz::macroman;

namespace rezin {

ResourceFork::ResourceFork(const BytesSlice& data, const Options& options) {
    // Resource header.
    BytesSlice header(data.slice(0, 16));
    uint32_t data_offset;
    uint32_t map_offset;
    uint32_t data_length;
    uint32_t map_length;
    read(header, data_offset);
    read(header, map_offset);
    read(header, data_length);
    read(header, map_length);

    BytesSlice map_data = data.slice(map_offset, map_length);
    BytesSlice data_data = data.slice(data_offset, data_length);

    // Map header.
    BytesSlice map(map_data.slice(16, 14));
    uint16_t type_offset;
    uint16_t name_offset;
    uint16_t type_count;
    map.shift(8);
    read(map, type_offset);
    read(map, name_offset);
    read(map, type_count);
    ++type_count;

    BytesSlice type_data = map_data.slice(type_offset);
    BytesSlice name_data = map_data.slice(name_offset);

    for (uint16_t i: range(type_count)) {
        shared_ptr<ResourceType> type(
                new ResourceType(type_data, i, name_data, data_data, options));
        _types[StringSlice(type->code())] = type;
    }
}

ResourceFork::~ResourceFork() { }

const ResourceType& ResourceFork::at(const StringSlice& code) const {
    StringMap<shared_ptr<ResourceType> >::const_iterator it = _types.find(code);
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

ResourceType::ResourceType(
        const BytesSlice& type_data, int index, const BytesSlice& name_data,
        const BytesSlice& data_data, const Options& options) {
    _code.assign(macroman::decode(type_data.slice(2 + index * 8, 4)));
    BytesSlice type(type_data.slice(6 + index * 8, 4));
    uint16_t count;
    uint16_t offset;
    read(type, count);
    read(type, offset);
    ++count;

    BytesSlice entry_data = type_data.slice(offset);
    for (uint16_t i: range(count)) {
        shared_ptr<ResourceEntry> entry(
                new ResourceEntry(entry_data, i, name_data, data_data, options));
        _entries[entry->id()] = entry;
    }
}

ResourceType::~ResourceType() { }

const sfz::String& ResourceType::code() const { return _code; }

const ResourceEntry& ResourceType::at(int16_t i) const {
    map<int16_t, shared_ptr<ResourceEntry> >::const_iterator it = _entries.find(i);
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

int16_t ResourceEntry::id() const {
    return _id;
}

const String& ResourceEntry::name() const {
    return _name;
}

const BytesSlice& ResourceEntry::data() const {
    return _data;
}

ResourceEntry::ResourceEntry(
        const BytesSlice& entry_data, int index, const BytesSlice& name_data,
        const BytesSlice& data_data, const Options& options) {
    BytesSlice entry_remainder(entry_data.slice(index * 12, 12));
    read(entry_remainder, _id);
    uint16_t name_offset;
    read(entry_remainder, name_offset);
    uint32_t data_offset;
    read(entry_remainder, data_offset);
    data_offset &= 0x00FFFFFF;
    entry_remainder.shift(4);

    if (name_offset != (uint16_t)-1) {
        uint8_t name_size = name_data.at(name_offset);
        _name.assign(options.decode(name_data.slice(name_offset + 1, name_size)));
    }

    BytesSlice data_remainder(data_data.slice(data_offset, 4));
    uint32_t data_size;
    read(data_remainder, data_size);
    _data = data_data.slice(data_offset + 4, data_size);
}

}  // namespace rezin
