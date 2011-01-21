// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/sources/FlatFileSource.hpp>

#include <sfz/sfz.hpp>

using sfz::BytesSlice;
using sfz::MappedFile;
using sfz::StringSlice;

namespace rezin {

FlatFileSource::FlatFileSource(const StringSlice& arg)
    : _path(arg) { }

BytesSlice FlatFileSource::load() {
    _file.reset(new MappedFile(_path));
    return _file->data();
}

}  // namespace rezin
