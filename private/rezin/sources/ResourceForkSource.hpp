// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_SOURCES_RESOURCE_FORK_SOURCE_HPP_
#define REZIN_SOURCES_RESOURCE_FORK_SOURCE_HPP_

#include <sfz/sfz.hpp>
#include <rezin/Source.hpp>

namespace rezin {

class ResourceForkSource : public Source {
  public:
    ResourceForkSource(const sfz::StringPiece& arg);

    virtual sfz::BytesPiece load();

  private:
    const sfz::String _path;
    sfz::scoped_ptr<sfz::MappedFile> _file;

    DISALLOW_COPY_AND_ASSIGN(ResourceForkSource);
};

}  // namespace rezin

#endif  // REZIN_SOURCES_RESOURCE_FORK_SOURCE_HPP_
