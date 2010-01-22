// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_STRING_LIST_HPP_
#define REZIN_STRING_LIST_HPP_

#include "rgos/Json.hpp"
#include "sfz/Bytes.hpp"

namespace rezin {

// Converts 'STR#' resource data into a caller-owned JSON object.
//
// The returned data is an array of strings.
//
// @param [in] data     The content of a 'STR#' resource.
// @returns             A JSON object representing the content of `in`.
rgos::Json read_string_list(const sfz::BytesPiece& data);

}  // namespace rezin

#endif // REZIN_STRING_LIST_HPP_
