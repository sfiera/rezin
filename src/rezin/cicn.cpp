// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/cicn.hpp>

#include <algorithm>
#include <map>
#include <vector>
#include <png.h>
#include <rezin/bits-slice.hpp>
#include <rezin/clut.hpp>
#include <rezin/primitives.hpp>
#include <sfz/sfz.hpp>

using sfz::Bytes;
using sfz::BytesSlice;
using sfz::Exception;
using sfz::Json;
using sfz::JsonDefaultVisitor;
using sfz::ReadSource;
using sfz::StringMap;
using sfz::StringSlice;
using sfz::WriteTarget;
using sfz::format;
using sfz::range;
using sfz::read;
using std::make_pair;
using std::map;
using std::swap;
using std::vector;

namespace rezin {

struct ColorIcon::Rep {
    PixMap icon_pixmap;
    BitMap mask_bitmap;
    BitMap icon_bitmap;
    uint32_t icon_data;
    std::vector<uint8_t> mask_bitmap_pixels;
    std::vector<uint8_t> icon_bitmap_pixels;
    ColorTable color_table;
    std::vector<uint8_t> icon_pixmap_pixels;
};

void read_from(ReadSource in, ColorIcon::Rep& rep) {
    read(in, rep.icon_pixmap);
    read(in, rep.mask_bitmap);
    read(in, rep.icon_bitmap);
    read(in, rep.icon_data);

    rep.mask_bitmap.read_pixels(in, rep.mask_bitmap_pixels);
    rep.icon_bitmap.read_pixels(in, rep.icon_bitmap_pixels);
    read(in, rep.color_table);
    rep.icon_pixmap.read_pixels(in, rep.icon_pixmap_pixels);
}

ColorIcon::ColorIcon(BytesSlice in):
        rep(new Rep) {
    read(in, *rep);
    if (!in.empty()) {
        throw Exception("extra bytes at end of 'cicn' resource.");
    }
}

ColorIcon::~ColorIcon() { }

namespace {

void png_error(png_struct* png, png_const_charp error_msg) {
    throw Exception(format("png error: {0}", error_msg));
}

void png_write_data(png_struct* png, png_byte* data, png_size_t length) {
    WriteTarget* out = reinterpret_cast<WriteTarget*>(png_get_io_ptr(png));
    write(*out, data, length);
}

void png_flush_data(png_struct* png) {
    static_cast<void>(png);
}

struct PngWriter {
    PngWriter(WriteTarget& out):
            out(out),
            png(NULL),
            info(NULL) {
        png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, png_error, png_error);
        if (!png) {
            throw Exception("couldn't create png_struct");
        }

        info = png_create_info_struct(png);
        if (!info) {
            png_destroy_write_struct(&png, NULL);
            throw Exception("couldn't create png_info");
        }

        png_set_write_fn(png, &out, png_write_data, png_flush_data);
    }

    virtual ~PngWriter() {
        png_destroy_write_struct(&png, &info);
    }

    WriteTarget& out;
    png_struct* png;
    png_info* info;
};

// Appends a single pixel to out, with the given color components.
void append_pixel(Bytes& out, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    // I'm not sure why we specify "RGBA"-formatted pixels, but write them here as "ARGB".
    // I've tried this on both x86_64 and ppc, so it's not an endianness issue; it just seems
    // to be that PNG_COLOR_TYPE_RGBA has a misleading name.
    uint8_t rgba[4] = {alpha, red, green, blue};
    out.append(rgba, 4);
}

}  // namespace

PngColorIcon png(const ColorIcon& cicn) {
    PngColorIcon png = {cicn};
    return png;
}

void write_to(WriteTarget out, PngColorIcon png) {
    const ColorIcon::Rep& rep = *png.cicn.rep;
    int16_t width = rep.icon_pixmap.bounds.width();
    int16_t height = rep.icon_pixmap.bounds.height();

    PngWriter p(out);
    png_set_IHDR(p.png, p.info, width, height, 8, PNG_COLOR_TYPE_RGBA, NULL, NULL, NULL);
    png_set_swap_alpha(p.png);
    png_write_info(p.png, p.info);

    SFZ_FOREACH(int16_t row, range(height), {
        Bytes row_data;
        SFZ_FOREACH(int16_t col, range(width), {
            uint8_t index = rep.icon_pixmap_pixels[(row * height) + col];
            uint8_t mask = rep.mask_bitmap_pixels[(row * height) + col];
            if (mask == 0) {
                append_pixel(row_data, 0, 0, 0, 0);
            } else {
                if (rep.color_table.table.find(index) == rep.color_table.table.end()) {
                    throw Exception(format("No color with id {0}", index));
                }
                const Color& color = rep.color_table.table.find(index)->second;
                append_pixel(row_data, color.red, color.green, color.blue, 255);
            }
        });
        png_write_row(p.png, row_data.data());
    });

    png_write_end(p.png, p.info);
}

}  // namespace rezin
