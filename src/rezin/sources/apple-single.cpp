// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/sources/apple-single.hpp>

#include <rezin/apple-single.hpp>
#include <sfz/sfz.hpp>

using sfz::BytesSlice;
using sfz::MappedFile;
using sfz::StringSlice;
using sfz::PrintTarget;

namespace rezin {

AppleSingleSource::AppleSingleSource() { }

AppleSingleSource::~AppleSingleSource() { }

void AppleSingleSource::load(sfz::StringSlice path) {
    _file.reset(new MappedFile(path));
    _apple_single.reset(new AppleSingle(_file->data()));
}

BytesSlice AppleSingleSource::data() const {
    return _apple_single->at(AppleSingle::RESOURCE_FORK);
}

void swap(AppleSingleSource& x, AppleSingleSource& y) {
    swap(x._file, y._file);
    swap(x._apple_single, y._apple_single);
}

bool store_argument(AppleSingleSource& to, sfz::StringSlice value, PrintTarget error) {
    to.load(value);
    return true;
}

}  // namespace rezin
