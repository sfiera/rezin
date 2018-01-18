// Copyright (c) 2018 Chris Pickel <sfiera@twotaled.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_SOURCE_HPP_
#define REZIN_SOURCE_HPP_

#include <pn/fwd>

namespace rezin {

struct Options;
class ResourceFork;

class Source {
  public:
    virtual ~Source() {}
    virtual void          load()       = 0;
    virtual pn::data_view data() const = 0;
};

}  // namespace rezin

#endif  // REZIN_SOURCE_HPP_
