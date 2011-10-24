// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/primitives.hpp>

#include <vector>
#include <rezin/bits-slice.hpp>
#include <rezin/clut.hpp>
#include <rezin/image.hpp>
#include <sfz/sfz.hpp>

using sfz::Bytes;
using sfz::BytesSlice;
using sfz::Exception;
using sfz::Json;
using sfz::ReadSource;
using sfz::StringMap;
using sfz::format;
using sfz::read;
using sfz::scoped_ptr;
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

bool operator==(const Rect& x, const Rect& y) {
    return (x.left == y.left) && (x.top == y.top)
        && (x.right == y.right) && (x.bottom == y.bottom);
}

bool operator!=(const Rect& x, const Rect& y) {
    return !(x == y);
}

void read_from(ReadSource in, Rect& out) {
    read(in, out.top);
    read(in, out.left);
    read(in, out.bottom);
    read(in, out.right);
}

double fixed32_t::to_double() const {
    return int_value / 65536.0;
}

Json fixed32_t::to_json() const {
    return Json::number(to_double());
}

void read_from(ReadSource in, fixed32_t& out) {
    read(in, out.int_value);
}

namespace {

enum {
    INDEXED = 0,
    RGB_DIRECT = 16,
};

enum {
    DEFAULT_PACKING = 0,
    NO_PACKING = 1,
    REMOVE_PAD_BYTE = 2,
    CHUNK_RUN_LENGTH = 3,
    COMPONENT_RUN_LENGTH = 4,
};

AlphaColor lookup(const ColorTable& clut, uint16_t index) {
    if (clut.table.find(index) == clut.table.end()) {
        throw Exception("invalid color index");
    }
    const Color& color = clut.table.find(index)->second;
    return AlphaColor(color.red >> 8, color.green >> 8, color.blue >> 8);
}

}  // namespace

void PixMap::read_image(
        sfz::ReadSource in, const ColorTable& clut, sfz::scoped_ptr<RasterImage>& image) const {
    if (pixel_type == RGB_DIRECT) {
        return read_direct_image(in, image);
    }
    image.reset(new RasterImage(bounds));
    if (row_bytes == 0) {
        return;
    }
    size_t bytes_read = 0;
    Bytes bytes(row_bytes, '\0');
    for (int y = 0; y < bounds.height(); ++y) {
        in.shift(bytes.data(), bytes.size());
        bytes_read += bytes.size();
        BitsSlice bits(bytes);
        for (int x = 0; x < bounds.width(); ++x) {
            uint8_t value;
            bits.shift(&value, pixel_size);
            image->set(x, y, lookup(clut, value));
        }
    }
    if ((bytes_read % 2) != 0) {
        in.shift(1);
    }
}

void PixMap::read_direct_image(ReadSource in, sfz::scoped_ptr<RasterImage>& image) const {
    if (pixel_type != RGB_DIRECT) {
        throw Exception("image is not direct");
    }
    if (pack_type != 4) {
        throw Exception(format("unsupported pack_type {0}", pack_type));
    }
    image.reset(new RasterImage(bounds));
    if (row_bytes == 0) {
        return;
    }
    size_t bytes_read = 0;
    for (int y = 0; y < bounds.height(); ++y) {
        Bytes bytes;
        if (row_bytes <= 250) {
            bytes.resize(read<uint8_t>(in));
            bytes_read += 1;
        } else {
            bytes.resize(read<uint16_t>(in));
            bytes_read += 2;
        }
        in.shift(bytes.data(), bytes.size());
        bytes_read += bytes.size();

        BytesSlice remainder(bytes);
        Bytes components;
        while (!remainder.empty()) {
            uint8_t header = read<uint8_t>(remainder);
            if (header >= 0x80) {
                components.push(0x101 - header, read<uint8_t>(remainder));
            } else {
                size_t size = header + 1;
                components.push(size, '\0');
                uint8_t* data = components.data() + components.size() - size;
                remainder.shift(data, size);
            }
        }
        const int16_t w = bounds.width();
        const BytesSlice red = components.slice((cmp_count - 3) * w, w);
        const BytesSlice green = components.slice((cmp_count - 2) * w, w);
        const BytesSlice blue = components.slice((cmp_count - 1) * w);
        for (int x = 0; x < w; ++x) {
            image->set(x, y, AlphaColor(red.at(x), green.at(x), blue.at(x)));
        }
    }
    if ((bytes_read % 2) != 0) {
        in.shift(1);
    }
}

void PixMap::read_packed_image(
        sfz::ReadSource in, const ColorTable& clut, sfz::scoped_ptr<RasterImage>& image) const {
    if (pixel_type != INDEXED) {
        throw Exception("image is not indexed");
    }
    image.reset(new RasterImage(bounds));
    if (row_bytes == 0) {
        return;
    }
    size_t bytes_read = 0;
    for (int y = 0; y < bounds.height(); ++y) {
        Bytes bytes;
        if (row_bytes <= 250) {
            bytes.resize(read<uint8_t>(in));
            bytes_read += 1;
        } else {
            bytes.resize(read<uint16_t>(in));
            bytes_read += 2;
        }
        in.shift(bytes.data(), bytes.size());
        bytes_read += bytes.size();

        int32_t x = 0;
        BytesSlice remainder = bytes;
        while (!remainder.empty()) {
            uint8_t header = read<uint8_t>(remainder);
            if (header >= 0x80) {
                uint8_t value = read<uint8_t>(remainder);
                uint8_t size = 0x101 - header;
                for (int j = 0; j < size; ++j) {
                    if (x < bounds.width()) {
                        image->set(x + bounds.left, y + bounds.top, lookup(clut, value));
                    }
                    ++x;
                }
            } else {
                uint8_t size = header + 1;
                for (int j = 0; j < size; ++j) {
                    if (x < bounds.width()) {
                        uint8_t value = read<uint8_t>(remainder);
                        image->set(x + bounds.left, y + bounds.top, lookup(clut, value));
                    }
                    ++x;
                }
            }
        }
    }
    if ((bytes_read % 2 == 1)) {
        in.shift(1);
    }
}

void read_from(ReadSource in, PixMap& out) {
    read(in, out.row_bytes);
    out.row_bytes &= 0x3fff;
    read(in, out.bounds);
    read(in, out.pm_version);
    read(in, out.pack_type);
    read(in, out.pack_size);
    read(in, out.h_res);
    read(in, out.v_res);
    read(in, out.pixel_type);
    read(in, out.pixel_size);
    read(in, out.cmp_count);
    read(in, out.cmp_size);
    read(in, out.plane_bytes);
    read(in, out.pm_table);
    read(in, out.pm_reserved);

    if (out.plane_bytes != 0) {
        throw Exception("PixMap::plane_bytes must be 0");
    }
    if (out.pm_reserved != 0) {
        throw Exception("PixMap::pm_reserved must be 0");
    }

    switch (out.pixel_type) {
        case INDEXED: {
            switch (out.pixel_size) {
              case 1:
              case 2:
              case 4:
              case 8:
                break;

              default:
                throw Exception(format("indexed pixels may not have size {0}", out.pixel_size));
            }
            if ((out.pack_type != 0) || (out.pack_size != 0)) {
                throw Exception("indexed pixels may not be packed");
            }
            if ((out.cmp_count != 1) || (out.cmp_size != out.pixel_size)) {
                throw Exception("indexed pixels must have one component");
            }
            break;
        }

        case RGB_DIRECT: {
            switch (out.pixel_size) {
                case 16: {
                    throw Exception(format("unsupported pixel_size {0}", out.pixel_size));
                    break;
                }

                case 32: {
                    if (out.cmp_size != 8) {
                        throw Exception("32-bit direct pixels must have cmp_size 8");
                    }
                    break;
                }

                default: {
                    throw Exception(format("direct pixels may not have size {0}", out.pixel_size));
                }
            }
            if ((out.cmp_count != 3) && (out.cmp_count != 4)) {
                throw Exception("direct pixels must have three or four components");
            }
            break;
        }

        default: {
            throw Exception(format("illegal PixMap pixel_type {0}", out.pixel_type));
        }
    }
}

void read_from(ReadSource in, AddressedPixMap& out) {
    read(in, out.base_addr);
    PixMap& parent = out;
    read(in, parent);
}

Json BitMap::to_json() const {
    StringMap<Json> result;
    result["bounds"] = bounds.to_json();
    return Json::object(result);
}

void BitMap::read_image(
        ReadSource in, AlphaColor on, AlphaColor off, scoped_ptr<RasterImage>& image) const {
    image.reset(new RasterImage(bounds));
    if (row_bytes == 0) {
        return;
    }
    Bytes bytes(row_bytes, '\0');
    for (int y = 0; y < bounds.height(); ++y) {
        in.shift(bytes.data(), bytes.size());
        BitsSlice bits(bytes);
        for (int x = 0; x < bounds.width(); ++x) {
            uint8_t value;
            bits.shift(&value, 1);
            image->set(x + bounds.left, y + bounds.top, value ? on : off);
        }
    }
}

void read_from(ReadSource in, BitMap& out) {
    read(in, out.base_addr);
    read(in, out.row_bytes);
    read(in, out.bounds);

    if (out.base_addr != 0) {
        throw Exception("PixMap::base_addr must be 0");
    }
}

}  // namespace rezin
