// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_SOURCES_ZIP_HPP_
#define REZIN_SOURCES_ZIP_HPP_

#include <sfz/sfz.hpp>
#include <zipxx/zipxx.hpp>

namespace rezin {

class ZipSource {
  public:
    ZipSource();
    ~ZipSource();

    void load(sfz::StringSlice zip_file, sfz::StringSlice file_path);
    sfz::BytesSlice data() const;

  private:
    friend void swap(ZipSource& x, ZipSource& y);

    struct Contents;
    std::unique_ptr<Contents> _contents;

    DISALLOW_COPY_AND_ASSIGN(ZipSource);
};

void swap(ZipSource& x, ZipSource& y);
bool store_argument(ZipSource& to, sfz::StringSlice value, sfz::PrintTarget error);

}  // namespace rezin

#endif  // REZIN_SOURCES_ZIP_HPP_
