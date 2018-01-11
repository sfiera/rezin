// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_OPTIONS_HPP_
#define REZIN_OPTIONS_HPP_

#include <sfz/sfz.hpp>

namespace rezin {

struct Options {
    Options();

    enum LineEnding { CR, NL, CRNL };
    LineEnding line_ending;

    struct EncodedString;
    EncodedString decode(const sfz::BytesSlice& bytes) const;
};

struct Options::EncodedString {
    const sfz::BytesSlice& bytes;
    LineEnding             line_ending;
};
void print_to(sfz::PrintTarget out, const Options::EncodedString& encoded);

bool store_argument(Options::LineEnding& to, sfz::StringSlice value, sfz::PrintTarget error);

}  // namespace rezin

#endif  // REZIN_OPTIONS_HPP_
