// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_MAC_ROMAN_HPP_
#define REZIN_MAC_ROMAN_HPP_

namespace sfz { class Encoding; }
namespace sfz { class String; }

namespace rezin {

// MacRoman text encoding.
//
// This encoding can represent code points in the range [U+00, U+7F], plus 128 additional code
// points taken from the basic multilingual plane.  Each code point is represented by a 1-byte
// value.
//
// A StringPiece in MacRoman encoding makes the following time guarantees:
//
//   construction: linear time.
//   size(): constant time.
//   at(): constant time.
//   empty(): constant time.
//   substr(): constant time.
//   iteration: linear time.
//
// @returns             An Encoding object for MacRoman.
const sfz::Encoding& mac_roman_encoding();

// Converts carriage returns to newlines.
//
// On classic Macintosh systems, a carriage return ('\r') was used to indicate line breaks, rather
// than a newline ('\n'), as is the case on Unix systems (including Mac OS X).  Since preserving
// the carriage return is probably undesirable on modern systems, this function is provided to
// convert all carriage returns in a string into newlines.
//
// @param [in,out] string The string to convert.
void cr2nl(sfz::String* string);

}  // namespace rezin

#endif  // REZIN_MAC_ROMAN_HPP_
