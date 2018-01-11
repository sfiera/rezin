// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_STRL_HPP_
#define REZIN_STRL_HPP_

#include <sfz/sfz.hpp>
#include <vector>

namespace rezin {

struct Options;

struct StringList {
    StringList(sfz::BytesSlice in, const Options& options);

    std::vector<std::shared_ptr<const sfz::String>> strings;
};

sfz::Json json(const StringList& strings);

}  // namespace rezin

#endif  // REZIN_STRL_HPP_
