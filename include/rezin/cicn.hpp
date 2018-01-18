// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_CICN_HPP_
#define REZIN_CICN_HPP_

#include <memory>
#include <pn/fwd>

namespace rezin {

struct PngColorIcon;

struct ColorIcon {
    struct Rep;

    ColorIcon(pn::data_view in);
    ~ColorIcon();

    std::unique_ptr<Rep> rep;
};

pn::data png(const ColorIcon& cicn);

}  // namespace rezin

#endif  // REZIN_CICN_HPP_
