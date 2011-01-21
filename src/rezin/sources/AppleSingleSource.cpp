// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/sources/AppleSingleSource.hpp>

#include <rezin/AppleSingle.hpp>
#include <sfz/sfz.hpp>

using sfz::BytesSlice;
using sfz::MappedFile;
using sfz::StringSlice;

namespace rezin {

AppleSingleSource::AppleSingleSource(const StringSlice& arg)
        : _path(arg) { }

AppleSingleSource::~AppleSingleSource() { }

BytesSlice AppleSingleSource::load() {
    _file.reset(new MappedFile(_path));
    _apple_single.reset(new AppleSingle(_file->data()));
    return _apple_single->at(AppleSingle::RESOURCE_FORK);
}

}  // namespace rezin
