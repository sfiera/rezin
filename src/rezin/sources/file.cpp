// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/sources/file.hpp>

#include <sfz/sfz.hpp>

using sfz::BytesSlice;
using sfz::MappedFile;
using sfz::PrintTarget;
using sfz::StringSlice;

namespace rezin {

FlatFileSource::FlatFileSource() { }

void FlatFileSource::load(StringSlice path) {
    _file.reset(new MappedFile(path));
}

BytesSlice FlatFileSource::data() const {
    return _file->data();
}

void swap(FlatFileSource& x, FlatFileSource& y) {
    swap(x._file, y._file);
}

bool store_argument(FlatFileSource& to, sfz::StringSlice value, PrintTarget error) {
    to.load(value);
    return true;
}

}  // namespace rezin
