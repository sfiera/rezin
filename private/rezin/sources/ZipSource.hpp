// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_SOURCES_ZIP_SOURCE_HPP_
#define REZIN_SOURCES_ZIP_SOURCE_HPP_

#include <sfz/sfz.hpp>
#include <rezin/Source.hpp>

namespace zipxx { class ZipArchive; }
namespace zipxx { class ZipFileReader; }

namespace rezin {

class AppleSingle;

class ZipSource : public Source {
  public:
    ZipSource(const sfz::StringPiece& arg);
    ~ZipSource();

    virtual sfz::BytesPiece load();

  private:
    sfz::String _archive_path;
    sfz::String _file_path;
    sfz::scoped_ptr<zipxx::ZipArchive> _archive;
    sfz::scoped_ptr<zipxx::ZipFileReader> _file;
    sfz::scoped_ptr<AppleSingle> _apple_single;

    DISALLOW_COPY_AND_ASSIGN(ZipSource);
};

}  // namespace rezin

#endif  // REZIN_SOURCES_ZIP_SOURCE_HPP_
