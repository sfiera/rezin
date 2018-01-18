// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/bits-slice.hpp>

#include <sfz/sfz.hpp>

namespace rezin {

namespace {

uint8_t bits(uint8_t byte, int begin, int end) {
    const int     size  = end - begin;
    const uint8_t mask  = (1 << size) - 1;
    const int     shift = 8 - end;
    return (byte >> shift) & mask;
}

}  // namespace

BitsSlice::BitsSlice(const pn::data_view& data) : _data(data), _bit_index(0) {}

void BitsSlice::shift(int size) {
    _bit_index += size;
    _data.shift(_bit_index / 8);
    _bit_index = _bit_index % 8;
}

void BitsSlice::shift(uint8_t* data, size_t size) {
    if (size == 0) {
        return;
    } else if (size + _bit_index > 8) {
        throw std::runtime_error(
                pn::format("unhandled case ({0} + {1})", size, _bit_index).c_str());
    }

    uint8_t byte = _data[0];
    *data        = bits(byte, _bit_index, _bit_index + size);

    _bit_index += size;
    if (_bit_index == 8) {
        _data.shift(1);
        _bit_index = 0;
    }
}

}  // namespace rezin
