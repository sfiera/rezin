// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/sources/FlatFileSource.hpp>

#include <sfz/sfz.hpp>

using sfz::BytesPiece;
using sfz::MappedFile;
using sfz::StringPiece;

namespace rezin {

FlatFileSource::FlatFileSource(const StringPiece& arg)
    : _path(arg) { }

BytesPiece FlatFileSource::load() {
    _file.reset(new MappedFile(_path));
    return _file->data();
}

}  // namespace rezin
