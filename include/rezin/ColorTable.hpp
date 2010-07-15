// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_COLOR_TABLE_HPP_
#define REZIN_COLOR_TABLE_HPP_

#include <exception>
#include <stdint.h>
#include <stdlib.h>
#include "rgos/rgos.hpp"
#include "sfz/sfz.hpp"

namespace rezin {

// Converts 'clut' resource data into a JSON object.
//
// The returned data is in the following format:
//
// [
//   {
//     "id": 0,
//     "red": 65535,
//     "green": 65535,
//     "blue": 65535
//   },
//   {
//     "id": 1,
//     "red": 65535,
//     "green": 39321,
//     "blue": 0
//   }
//   ...
// ]
//
// All "red", "green", and "blue" components are integers in the range 0..65535.  The "id"
// attribute is a integer in the range 0..65535.  In the common case, the "id" attribute will equal
// the index of the color within the table, but this will not always hold.
//
// @param [in] in       The content of a 'clut' resource.
// @returns             A JSON object representing the content of `in`.
// @throws Exception    If the 'clut' data could not be read.
rgos::Json read_clut(const sfz::BytesPiece& in);

}  // namespace rezin

#endif // REZIN_COLOR_TABLE_HPP_
