// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_SOURCES_APPLE_SINGLE_HPP_
#define REZIN_SOURCES_APPLE_SINGLE_HPP_

#include <rezin/source.hpp>
#include <sfz/sfz.hpp>

namespace rezin {

class AppleSingle;

class AppleSingleSource : public Source {
  public:
    AppleSingleSource(pn::string_view path);
    ~AppleSingleSource();

    void          load() override;
    pn::data_view data() const override;

  private:
    const pn::string                  _path;
    std::unique_ptr<sfz::mapped_file> _file;
    std::unique_ptr<AppleSingle>      _apple_single;

    AppleSingleSource(const AppleSingleSource&) = delete;
    AppleSingleSource& operator=(const AppleSingleSource&) = delete;
};

}  // namespace rezin

#endif  // REZIN_SOURCES_APPLE_SINGLE_HPP_
