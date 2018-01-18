// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/primitives.hpp>

#include <rezin/bits-slice.hpp>
#include <rezin/clut.hpp>
#include <rezin/image.hpp>
#include <sfz/sfz.hpp>
#include <vector>

namespace rezin {

int16_t Rect::width() const { return right - left; }

int16_t Rect::height() const { return bottom - top; }

pn::value Rect::to_value() const {
    return pn::map{
            {"top", top}, {"left", left}, {"bottom", bottom}, {"right", right},
    };
}

bool operator==(const Rect& x, const Rect& y) {
    return (x.left == y.left) && (x.top == y.top) && (x.right == y.right) &&
           (x.bottom == y.bottom);
}

bool operator!=(const Rect& x, const Rect& y) { return !(x == y); }

void read_from(pn::file_view in, Rect* out) {
    in.read(&out->top, &out->left, &out->bottom, &out->right).check();
}

double fixed32_t::to_double() const { return int_value / 65536.0; }

pn::value fixed32_t::to_value() const { return to_double(); }

void read_from(pn::file_view in, fixed32_t* out) { in.read(&out->int_value).check(); }

namespace {

enum {
    INDEXED    = 0,
    RGB_DIRECT = 16,
};

enum {
    DEFAULT_PACKING      = 0,
    NO_PACKING           = 1,
    REMOVE_PAD_BYTE      = 2,
    CHUNK_RUN_LENGTH     = 3,
    COMPONENT_RUN_LENGTH = 4,
};

AlphaColor lookup(const ColorTable& clut, uint16_t index) {
    if (clut.table.find(index) == clut.table.end()) {
        return AlphaColor(0, 0, 0, 0);
    }
    const Color& color = clut.table.find(index)->second;
    return AlphaColor(color.red >> 8, color.green >> 8, color.blue >> 8);
}

}  // namespace

std::unique_ptr<RasterImage> PixMap::read_image(pn::file_view in, const ColorTable& clut) const {
    if (pixel_type == RGB_DIRECT) {
        return read_direct_image(in);
    }
    std::unique_ptr<RasterImage> image(new RasterImage(bounds));
    if (row_bytes == 0) {
        return image;
    }
    size_t   bytes_read = 0;
    pn::data d;
    d.resize(row_bytes);
    for (int y = 0; y < bounds.height(); ++y) {
        in.read(&d).check();
        bytes_read += d.size();
        BitsSlice bits(d);
        for (int x = 0; x < bounds.width(); ++x) {
            uint8_t value;
            bits.shift(&value, pixel_size);
            image->set(x, y, lookup(clut, value));
        }
    }
    if ((bytes_read % 2) != 0) {
        in.read(pn::pad(1)).check();
    }
    return image;
}

std::unique_ptr<RasterImage> PixMap::read_direct_image(pn::file_view in) const {
    if (pixel_type != RGB_DIRECT) {
        throw std::runtime_error("image is not direct");
    }
    if (pack_type != 4) {
        throw std::runtime_error(pn::format("unsupported pack_type {0}", pack_type).c_str());
    }
    std::unique_ptr<RasterImage> image(new RasterImage(bounds));
    if (row_bytes == 0) {
        return image;
    }
    size_t bytes_read = 0;
    for (int y = 0; y < bounds.height(); ++y) {
        pn::data d;
        if (row_bytes <= 250) {
            uint8_t size;
            in.read(&size).check();
            d.resize(size);
            bytes_read += 1;
        } else {
            uint16_t size;
            in.read(&size).check();
            d.resize(size);
            bytes_read += 2;
        }
        in.read(&d).check();
        bytes_read += d.size();

        pn::data components;
        pn::file remainder = d.open();
        while (true) {
            uint8_t header;
            if (remainder.read(&header).eof()) {
                break;
            }
            remainder.check();
            if (header >= 0x80) {
                uint8_t value;
                remainder.read(&value).check();
                for (int i = 0; i < (0x101 - header); ++i) {
                    components.append(&value, 1);
                }
            } else {
                pn::data d;
                d.resize(header + 1);
                remainder.read(&d).check();
                components += d;
            }
        }
        const int16_t       w     = bounds.width();
        const pn::data_view red   = components.slice((cmp_count - 3) * w, w);
        const pn::data_view green = components.slice((cmp_count - 2) * w, w);
        const pn::data_view blue  = components.slice((cmp_count - 1) * w);
        for (int x = 0; x < w; ++x) {
            image->set(x, y, AlphaColor(red[x], green[x], blue[x]));
        }
    }
    if ((bytes_read % 2) != 0) {
        in.read(pn::pad(1)).check();
    }
    return image;
}

std::unique_ptr<RasterImage> PixMap::read_packed_image(
        pn::file_view in, const ColorTable& clut) const {
    if (pixel_type != INDEXED) {
        throw std::runtime_error("image is not indexed");
    }
    std::unique_ptr<RasterImage> image(new RasterImage(bounds));
    if (row_bytes == 0) {
        return image;
    }
    size_t bytes_read = 0;
    for (int y = 0; y < bounds.height(); ++y) {
        pn::data d;
        if (row_bytes <= 250) {
            uint8_t size;
            in.read(&size).check();
            d.resize(size);
            bytes_read += 1;
        } else {
            uint16_t size;
            in.read(&size).check();
            d.resize(size);
            bytes_read += 2;
        }
        in.read(&d).check();
        bytes_read += d.size();

        int32_t  x         = 0;
        pn::file remainder = d.open();
        while (true) {
            uint8_t header;
            if (remainder.read(&header).eof()) {
                break;
            }
            remainder.check();
            if (header >= 0x80) {
                uint8_t value;
                remainder.read(&value).check();
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
                        uint8_t value;
                        remainder.read(&value);
                        image->set(x + bounds.left, y + bounds.top, lookup(clut, value));
                    }
                    ++x;
                }
            }
        }
    }
    if ((bytes_read % 2 == 1)) {
        in.read(pn::pad(1)).check();
    }
    return image;
}

void read_from(pn::file_view in, PixMap* out) {
    in.read(&out->row_bytes).check();
    out->row_bytes &= 0x3fff;
    read_from(in, &out->bounds);
    in.read(&out->pm_version, &out->pack_type, &out->pack_size).check();
    read_from(in, &out->h_res);
    read_from(in, &out->v_res);
    in.read(&out->pixel_type, &out->pixel_size, &out->cmp_count, &out->cmp_size, &out->plane_bytes,
            &out->pm_table, &out->pm_reserved)
            .check();

    if (out->pm_reserved != 0) {
        throw std::runtime_error("PixMap::pm_reserved must be 0");
    }

    switch (out->pixel_type) {
        case INDEXED: {
            switch (out->pixel_size) {
                case 1:
                case 2:
                case 4:
                case 8: break;

                default:
                    throw std::runtime_error(
                            pn::format("indexed pixels may not have size {0}", out->pixel_size)
                                    .c_str());
            }
            if ((out->pack_type != 0) || (out->pack_size != 0)) {
                throw std::runtime_error("indexed pixels may not be packed");
            }
            if ((out->cmp_count != 1) || (out->cmp_size != out->pixel_size)) {
                throw std::runtime_error("indexed pixels must have one component");
            }
            break;
        }

        case RGB_DIRECT: {
            switch (out->pixel_size) {
                case 16: {
                    throw std::runtime_error(
                            pn::format("unsupported pixel_size {0}", out->pixel_size).c_str());
                    break;
                }

                case 32: {
                    if (out->cmp_size != 8) {
                        throw std::runtime_error("32-bit direct pixels must have cmp_size 8");
                    }
                    break;
                }

                default: {
                    throw std::runtime_error(
                            pn::format("direct pixels may not have size {0}", out->pixel_size)
                                    .c_str());
                }
            }
            if ((out->cmp_count != 3) && (out->cmp_count != 4)) {
                throw std::runtime_error("direct pixels must have three or four components");
            }
            break;
        }

        default: {
            throw std::runtime_error(
                    pn::format("illegal PixMap pixel_type {0}", out->pixel_type).c_str());
        }
    }
}

void read_from(pn::file_view in, AddressedPixMap* out) {
    in.read(&out->base_addr).check();
    PixMap* parent = out;
    read_from(in, parent);
}

pn::value BitMap::to_value() const { return pn::map{{"bounds", bounds.to_value()}}; }

std::unique_ptr<RasterImage> BitMap::read_image(
        pn::file_view in, AlphaColor on, AlphaColor off) const {
    std::unique_ptr<RasterImage> image(new RasterImage(bounds));
    if (row_bytes == 0) {
        return image;
    }
    pn::data d;
    d.resize(row_bytes);
    for (int y = 0; y < bounds.height(); ++y) {
        in.read(&d).check();
        BitsSlice bits(d);
        for (int x = 0; x < bounds.width(); ++x) {
            uint8_t value;
            bits.shift(&value, 1);
            image->set(x + bounds.left, y + bounds.top, value ? on : off);
        }
    }
    return image;
}

void read_from(pn::file_view in, BitMap* out) {
    in.read(&out->base_addr, &out->row_bytes).check();
    read_from(in, &out->bounds);

    if (out->base_addr != 0) {
        throw std::runtime_error("PixMap::base_addr must be 0");
    }
}

}  // namespace rezin
