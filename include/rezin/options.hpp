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

    pn::string decode(const pn::data_view& bytes) const;
};

}  // namespace rezin

#endif  // REZIN_OPTIONS_HPP_
