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
    StringList(pn::data_view in, const Options& options);

    std::vector<pn::string> strings;
};

pn::value value(const StringList& strings);

}  // namespace rezin

#endif  // REZIN_STRL_HPP_
