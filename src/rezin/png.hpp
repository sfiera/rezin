// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_PNG_HPP_
#define REZIN_PNG_HPP_

#include <png.h>
#include <sfz/sfz.hpp>

namespace rezin {

class PngWriter {
  public:
    PngWriter(sfz::WriteTarget& out, int32_t width, int32_t height);
    ~PngWriter();

    // Appends a single pixel to out, with the given color components.
    // Must be called exactly ``width * height`` times.
    void append_pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

  private:
    sfz::WriteTarget& _out;
    const int32_t _width;
    const int32_t _height;

    png_struct* _png;
    png_info* _info;

    sfz::Bytes _row_data;
    int32_t _pixel_index;
};

}  // namespace rezin

#endif // REZIN_PNG_HPP_
