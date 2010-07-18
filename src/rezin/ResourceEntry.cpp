// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/ResourceEntry.hpp"

#include <stdexcept>
#include <sfz/sfz.hpp>
#include "rezin/Options.hpp"

using sfz::BytesPiece;
using sfz::String;
using sfz::read;

namespace rezin {

int16_t ResourceEntry::id() const {
    return _id;
}

const String& ResourceEntry::name() const {
    return _name;
}

const BytesPiece& ResourceEntry::data() const {
    return _data;
}

ResourceEntry::ResourceEntry(
        const BytesPiece& entry_data, int index, const BytesPiece& name_data,
        const BytesPiece& data_data, const Options& options) {
    BytesPiece entry_remainder(entry_data.substr(index * 12, 12));
    read(&entry_remainder, &_id);
    uint16_t name_offset;
    read(&entry_remainder, &name_offset);
    uint32_t data_offset;
    read(&entry_remainder, &data_offset);
    data_offset &= 0x00FFFFFF;
    entry_remainder.shift(4);

    if (name_offset != (uint16_t)-1) {
        uint8_t name_size = name_data.at(name_offset);
        _name.assign(options.decode(name_data.substr(name_offset + 1, name_size)));
    }

    BytesPiece data_remainder(data_data.substr(data_offset, 4));
    uint32_t data_size;
    read(&data_remainder, &data_size);
    _data = data_data.substr(data_offset + 4, data_size);
}

}  // namespace rezin
