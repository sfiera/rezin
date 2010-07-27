// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_SOURCE_HPP_
#define REZIN_SOURCE_HPP_

#include <sfz/sfz.hpp>

namespace rezin {

class Source {
  public:
    virtual ~Source() { }
    virtual sfz::BytesPiece load() = 0;
};

}  // namespace rezin

#endif  // REZIN_SOURCE_HPP_
