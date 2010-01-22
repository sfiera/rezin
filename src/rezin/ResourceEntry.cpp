// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/ResourceEntry.hpp"

#include <stdexcept>
#include "rezin/MacRoman.hpp"
#include "sfz/BinaryReader.hpp"

using sfz::BytesBinaryReader;
using sfz::BytesPiece;
using sfz::String;

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
        const BytesPiece& data_data) {
    BytesBinaryReader bin(entry_data.substr(index * 12, 12));
    bin.read(&_id);
    uint16_t name_offset;
    bin.read(&name_offset);
    uint32_t data_offset;
    bin.read(&data_offset);
    data_offset &= 0x00FFFFFF;
    bin.discard(4);

    if (name_offset != (uint16_t)-1) {
        uint8_t name_size = name_data.at(name_offset);
        _name.assign(name_data.substr(name_offset + 1, name_size), mac_roman_encoding());
        cr2nl(&_name);
    }

    BytesBinaryReader data_bin(data_data.substr(data_offset, 4));
    uint32_t data_size;
    data_bin.read(&data_size);
    _data = data_data.substr(data_offset + 4, data_size);
}

}  // namespace rezin
