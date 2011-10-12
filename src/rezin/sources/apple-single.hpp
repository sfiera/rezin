// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_SOURCES_APPLE_SINGLE_HPP_
#define REZIN_SOURCES_APPLE_SINGLE_HPP_

#include <sfz/sfz.hpp>

namespace rezin {

class AppleSingle;

class AppleSingleSource {
  public:
    AppleSingleSource();
    ~AppleSingleSource();

    void load(sfz::StringSlice path);
    sfz::BytesSlice data() const;

  private:
    friend void swap(AppleSingleSource& x, AppleSingleSource& y);

    sfz::scoped_ptr<sfz::MappedFile> _file;
    sfz::scoped_ptr<AppleSingle> _apple_single;

    DISALLOW_COPY_AND_ASSIGN(AppleSingleSource);
};

void swap(AppleSingleSource& x, AppleSingleSource& y);
bool store_argument(AppleSingleSource& to, sfz::StringSlice value, sfz::PrintTarget error);

}  // namespace rezin

#endif  // REZIN_SOURCES_APPLE_SINGLE_HPP_
