// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_SOURCES_FILE_HPP_
#define REZIN_SOURCES_FILE_HPP_

#include <sfz/sfz.hpp>

namespace rezin {

class FlatFileSource {
  public:
    FlatFileSource();

    void load(sfz::StringSlice path);
    sfz::BytesSlice data() const;

  private:
    friend void swap(FlatFileSource& x, FlatFileSource& y);

    sfz::scoped_ptr<sfz::MappedFile> _file;

    DISALLOW_COPY_AND_ASSIGN(FlatFileSource);
};

void swap(FlatFileSource& x, FlatFileSource& y);
bool store_argument(FlatFileSource& to, sfz::StringSlice value, sfz::PrintTarget error);

}  // namespace rezin

#endif  // REZIN_SOURCES_FILE_HPP_
