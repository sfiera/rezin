// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/BasicTypes.hpp>

#include <vector>
#include <rezin/BitsSlice.hpp>
#include <rgos/rgos.hpp>
#include <sfz/sfz.hpp>

using rgos::Json;
using rgos::StringMap;
using sfz::Bytes;
using sfz::BytesSlice;
using sfz::Exception;
using sfz::ReadSource;
using sfz::format;
using sfz::read;
using std::vector;

namespace rezin {

int16_t Rect::width() const {
    return right - left;
}

int16_t Rect::height() const {
    return bottom - top;
}

Json Rect::to_json() const {
    StringMap<Json> result;
    result["top"]       = Json::number(top);
    result["left"]      = Json::number(left);
    result["bottom"]    = Json::number(bottom);
    result["right"]     = Json::number(right);
    return Json::object(result);
}

void read_from(ReadSource in, Rect* out) {
    read(in, &out->top);
    read(in, &out->left);
    read(in, &out->bottom);
    read(in, &out->right);
}

double fixed32_t::to_double() const {
    return int_value / 65536.0;
}

Json fixed32_t::to_json() const {
    return Json::number(to_double());
}

void read_from(ReadSource in, fixed32_t* out) {
    read(in, &out->int_value);
}

Json PixMap::to_json() const {
    StringMap<Json> result;
    result["bounds"] = bounds.to_json();
    return Json::object(result);
}

void PixMap::read_pixels(ReadSource in, vector<uint8_t>* out) const {
    if (row_bytes == 0) {
        return;
    }

    size_t size = row_bytes * bounds.height();
    Bytes bytes(size, '\0');
    in.shift(bytes.data(), size);

    BytesSlice remainder = bytes;
    for (int i = 0; i < bounds.height(); ++i) {
        BitsSlice bits(remainder.slice(0, row_bytes));
        for (int j = 0; j < bounds.width(); ++j) {
            uint8_t value;
            bits.shift(&value, pixel_size);
            out->push_back(value);
        }
        remainder.shift(row_bytes);
    }
}

void read_from(ReadSource in, PixMap* out) {
    read(in, &out->base_addr);
    read(in, &out->row_bytes);
    out->row_bytes &= 0x3fff;
    read(in, &out->bounds);
    read(in, &out->pm_version);
    read(in, &out->pack_type);
    read(in, &out->pack_size);
    read(in, &out->h_res);
    read(in, &out->v_res);
    read(in, &out->pixel_type);
    read(in, &out->pixel_size);
    read(in, &out->cmp_count);
    read(in, &out->cmp_size);
    read(in, &out->plane_bytes);
    read(in, &out->pm_table);
    read(in, &out->pm_reserved);

    if ((out->pixel_type != 0) || (out->pack_type != 0) || (out->pack_size != 0)
            || (out->cmp_count != 1) || (out->cmp_size != out->pixel_size)) {
        throw Exception("only indexed pixels are supported");
    }
    switch (out->pixel_size) {
      case 1:
      case 2:
      case 4:
      case 8:
        break;

      default:
        throw Exception(format("indexed pixels may not have size {0}", out->pixel_size));
    }
    if (out->plane_bytes != 0) {
        throw Exception("PixMap::plane_bytes must be 0");
    }
    if (out->base_addr != 0) {
        throw Exception("PixMap::base_addr must be 0");
    }
    if (out->pm_table != 0) {
        throw Exception("PixMap::pm_table must be 0");
    }
    if (out->pm_reserved != 0) {
        throw Exception("PixMap::pm_reserved must be 0");
    }
}

Json BitMap::to_json() const {
    StringMap<Json> result;
    result["bounds"] = bounds.to_json();
    return Json::object(result);
}

void BitMap::read_pixels(ReadSource in, vector<uint8_t>* out) const {
    if (row_bytes == 0) {
        return;
    }

    size_t size = row_bytes * bounds.height();
    Bytes bytes(size, '\0');
    in.shift(bytes.data(), size);

    BytesSlice remainder = bytes;
    for (int i = 0; i < bounds.height(); ++i) {
        BitsSlice bits(remainder.slice(0, row_bytes));
        for (int j = 0; j < bounds.width(); ++j) {
            uint8_t value;
            bits.shift(&value, 1);
            out->push_back(value);
        }
        remainder.shift(row_bytes);
    }
}

void read_from(ReadSource in, BitMap* out) {
    read(in, &out->base_addr);
    read(in, &out->row_bytes);
    read(in, &out->bounds);

    if (out->base_addr != 0) {
        throw Exception("PixMap::base_addr must be 0");
    }
}

}  // namespace rezin
