// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_COLOR_ICON_HPP_
#define REZIN_COLOR_ICON_HPP_

#include <rgos/rgos.hpp>
#include <sfz/sfz.hpp>

namespace rezin {

// Converts 'cicn' resource data into a JSON object.
//
// The returned data is in the following format:
//
// {
//   "color-table": ...,
//   "pixels": [
//     [ null, 0, 1, 0, null, ...],
//     [ 0, 1, 1, 1, 0, ...],
//     ...
//   ]
// }
//
// "color-table" is a color table in the format returned by the function read_clut().  "pixels" is
// an array of arrays: the outer array describes a grid; the inner arrays describe rows and are all
// of equal size; and the elements of the inner arrays describe pixels at particular columns.
// Pixels are either numeric, indicating an entry in "color-table" by "id", or `null`, indicating a
// transparent pixel.
//
// @param [in] in       The content of a 'cicn' resource.
// @returns             A JSON object representing the content of `in`.
// @throws Exception    If the 'cicn' data could not be read.
rgos::Json read_cicn(const sfz::BytesPiece& in);

// Converts a JSON image into a PNG image.
//
// JSON data is required to be as returned by read_cicn().  It would be possible for us to
// interpret a wider range of image data (e.g. direct pixel data, without a "color-table") but
// currently we observe the same restrictions on input as read_cicn() does on output.
//
// @param [out] out     PNG data is appended to this target.
// @param [in] image    Image data as returned by read_cicn().
// @throw Exception     If the JSON data could not be interpreted.
void write_png(sfz::WriteTarget out, const rgos::Json& image);

}  // namespace rezin

#endif // REZIN_COLOR_ICON_HPP_
