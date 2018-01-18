// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/resource.hpp>

#include <rezin/options.hpp>
#include <sfz/sfz.hpp>

using sfz::StringMap;
using sfz::range;
using std::map;
using std::unique_ptr;

namespace macroman = sfz::macroman;

namespace rezin {

ResourceFork::ResourceFork(const pn::data_view& data, const Options& options) {
    // Resource header.
    pn::file header = data.slice(0, 16).open();
    uint32_t data_offset;
    uint32_t map_offset;
    uint32_t data_length;
    uint32_t map_length;
    header.read(&data_offset, &map_offset, &data_length, &map_length);

    pn::data_view map_data  = data.slice(map_offset, map_length);
    pn::data_view data_data = data.slice(data_offset, data_length);

    // Map header.
    pn::file map = map_data.slice(16, 14).open();
    uint16_t type_offset;
    uint16_t name_offset;
    uint16_t type_count;
    map.read(pn::pad(8), &type_offset, &name_offset, &type_count);
    ++type_count;

    pn::data_view type_data = map_data.slice(type_offset);
    pn::data_view name_data = map_data.slice(name_offset);

    for (uint16_t i : range(type_count)) {
        unique_ptr<ResourceType> type(
                new ResourceType(type_data, i, name_data, data_data, options));
        _types[pn::string_view(type->code())] = std::move(type);
    }
}

ResourceFork::~ResourceFork() {}

const ResourceType& ResourceFork::at(const pn::string_view& code) const {
    StringMap<unique_ptr<ResourceType>>::const_iterator it = _types.find(code);
    if (it == _types.end()) {
        throw std::runtime_error(pn::format("no such resource type '{0}'", code).c_str());
    }
    return *it->second;
}

ResourceFork::const_iterator ResourceFork::begin() const { return const_iterator(_types.begin()); }

ResourceFork::const_iterator ResourceFork::end() const { return const_iterator(_types.end()); }

ResourceType::ResourceType(
        const pn::data_view& type_data, int index, const pn::data_view& name_data,
        const pn::data_view& data_data, const Options& options) {
    _code         = macroman::decode(type_data.slice(2 + index * 8, 4));
    pn::file type = type_data.slice(6 + index * 8, 4).open();
    uint16_t count;
    uint16_t offset;
    type.read(&count, &offset).check();
    ++count;

    pn::data_view entry_data = type_data.slice(offset);
    for (uint16_t i : range(count)) {
        unique_ptr<ResourceEntry> entry(
                new ResourceEntry(entry_data, i, name_data, data_data, options));
        _entries[entry->id()] = std::move(entry);
    }
}

ResourceType::~ResourceType() {}

const pn::string& ResourceType::code() const { return _code; }

const ResourceEntry& ResourceType::at(int16_t i) const {
    map<int16_t, unique_ptr<ResourceEntry>>::const_iterator it = _entries.find(i);
    if (it == _entries.end()) {
        throw std::runtime_error(pn::format("no such resource entry '{0}' {1}", _code, i).c_str());
    }
    return *it->second;
}

ResourceType::const_iterator ResourceType::begin() const {
    return const_iterator(_entries.begin());
}

ResourceType::const_iterator ResourceType::end() const { return const_iterator(_entries.end()); }

int16_t ResourceEntry::id() const { return _id; }

const pn::string& ResourceEntry::name() const { return _name; }

const pn::data_view& ResourceEntry::data() const { return _data; }

ResourceEntry::ResourceEntry(
        const pn::data_view& entry_data, int index, const pn::data_view& name_data,
        const pn::data_view& data_data, const Options& options) {
    pn::file entry_remainder = entry_data.slice(index * 12, 12).open();
    uint16_t name_offset;
    uint32_t data_offset;
    entry_remainder.read(&_id, &name_offset, &data_offset, pn::pad(4));
    data_offset &= 0x00FFFFFF;

    if (name_offset != (uint16_t)-1) {
        uint8_t name_size = name_data[name_offset];
        _name             = options.decode(name_data.slice(name_offset + 1, name_size));
    }

    pn::file data_remainder = data_data.slice(data_offset, 4).open();
    uint32_t data_size;
    data_remainder.read(&data_size).check();
    _data = data_data.slice(data_offset + 4, data_size);
}

}  // namespace rezin
