// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_CR2NL_HPP_
#define REZIN_CR2NL_HPP_

#include <sfz/sfz.hpp>

namespace rezin {

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

#endif  // REZIN_CR2NL_HPP_
