// Copyright (c) 2011 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_PICT_HPP_
#define REZIN_PICT_HPP_

#include <sfz/sfz.hpp>

namespace rezin {

struct PngPicture;

struct Picture {
    struct Rep;

    Picture(pn::data_view in);
    ~Picture();

    bool    is_raster() const;
    uint8_t version() const;

    std::shared_ptr<Rep> rep;
};

pn::data png(const Picture& pict);

}  // namespace rezin

#endif  // REZIN_PICT_HPP_
