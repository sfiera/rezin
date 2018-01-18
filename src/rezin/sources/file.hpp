// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_SOURCES_FILE_HPP_
#define REZIN_SOURCES_FILE_HPP_

#include <rezin/source.hpp>
#include <sfz/sfz.hpp>

namespace rezin {

class FlatFileSource : public Source {
  public:
    FlatFileSource(pn::string_view path);

    void          load() override;
    pn::data_view data() const override;

  private:
    const pn::string                  _path;
    std::unique_ptr<sfz::mapped_file> _file;

    FlatFileSource(const FlatFileSource&) = delete;
    FlatFileSource& operator=(const FlatFileSource&) = delete;
};

}  // namespace rezin

#endif  // REZIN_SOURCES_FILE_HPP_
