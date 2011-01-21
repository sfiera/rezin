// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_SOURCES_APPLE_SINGLE_SOURCE_HPP_
#define REZIN_SOURCES_APPLE_SINGLE_SOURCE_HPP_

#include <sfz/sfz.hpp>
#include <rezin/Source.hpp>

namespace rezin {

class AppleSingle;

class AppleSingleSource : public Source {
  public:
    AppleSingleSource(const sfz::StringSlice& arg);
    ~AppleSingleSource();

    virtual sfz::BytesSlice load();

  private:
    const sfz::String _path;
    sfz::scoped_ptr<sfz::MappedFile> _file;
    sfz::scoped_ptr<AppleSingle> _apple_single;

    DISALLOW_COPY_AND_ASSIGN(AppleSingleSource);
};

}  // namespace rezin

#endif  // REZIN_SOURCES_APPLE_SINGLE_SOURCE_HPP_
