// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_SOURCES_ZIP_HPP_
#define REZIN_SOURCES_ZIP_HPP_

#include <rezin/source.hpp>
#include <sfz/sfz.hpp>
#include <zipxx/zipxx.hpp>

namespace rezin {

class ZipSource : public Source {
  public:
    ZipSource(pn::string_view arg);
    ~ZipSource();

    void          load() override;
    pn::data_view data() const override;

  private:
    pn::string _zip_path;
    pn::string _file_path;
    struct Contents;
    std::unique_ptr<Contents> _contents;

    ZipSource(const ZipSource&) = delete;
    ZipSource& operator=(const ZipSource&) = delete;
};

}  // namespace rezin

#endif  // REZIN_SOURCES_ZIP_HPP_
