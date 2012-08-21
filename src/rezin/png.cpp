// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/png.hpp>

#include <png.h>
#include <sfz/sfz.hpp>

using sfz::Bytes;
using sfz::Exception;
using sfz::WriteTarget;
using sfz::format;

namespace rezin {

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

}  // namespace

PngWriter::PngWriter(WriteTarget& out, int32_t width, int32_t height):
        _out(out),
        _width(width),
        _height(height),
        _png(NULL),
        _info(NULL),
        _pixel_index(0) {
    _png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, png_error, png_error);
    if (!_png) {
        throw Exception("couldn't create png_struct");
    }

    _info = png_create_info_struct(_png);
    if (!_info) {
        png_destroy_write_struct(&_png, NULL);
        throw Exception("couldn't create png_info");
    }

    png_set_write_fn(_png, &_out, png_write_data, png_flush_data);

    png_set_IHDR(_png, _info, width, height, 8, PNG_COLOR_TYPE_RGBA, 0, 0, 0);
    png_set_swap_alpha(_png);
    png_write_info(_png, _info);
}

PngWriter::~PngWriter() {
    png_destroy_write_struct(&_png, &_info);
}

void PngWriter::append_pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    // I'm not sure why we specify "RGBA"-formatted pixels, but write
    // them here as "ARGB".  I've tried this on both x86_64 and ppc, so
    // it's not an endianness issue; it just seems to be that
    // PNG_COLOR_TYPE_RGBA has a misleading name.
    uint8_t rgba[4] = {alpha, red, green, blue};
    _row_data.append(rgba, 4);
    ++_pixel_index;

    if ((_pixel_index % _width) == 0) {
        png_write_row(_png, _row_data.data());
        _row_data.clear();
        if ((_pixel_index / _width) == _height) {
            png_write_end(_png, _info);
        }
    }
}

}  // namespace rezin
