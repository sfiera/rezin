// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_CICN_HPP_
#define REZIN_CICN_HPP_

#include <sfz/sfz.hpp>

namespace rezin {

struct PngColorIcon;

struct ColorIcon {
    struct Rep;

    ColorIcon(sfz::BytesSlice in);
    ~ColorIcon();

    std::shared_ptr<Rep> rep;
};

PngColorIcon png(const ColorIcon& cicn);

struct PngColorIcon { const ColorIcon& cicn; };
void write_to(sfz::WriteTarget out, PngColorIcon png);

}  // namespace rezin

#endif // REZIN_CICN_HPP_
