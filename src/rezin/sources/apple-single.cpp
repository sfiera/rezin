// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/sources/apple-single.hpp>

#include <rezin/apple-single.hpp>
#include <sfz/sfz.hpp>

namespace rezin {

AppleSingleSource::AppleSingleSource(pn::string_view path) : _path(path.copy()) {}

AppleSingleSource::~AppleSingleSource() {}

void AppleSingleSource::load() {
    _file.reset(new sfz::mapped_file(_path));
    _apple_single.reset(new AppleSingle(_file->data()));
}

pn::data_view AppleSingleSource::data() const {
    return _apple_single->at(AppleSingle::RESOURCE_FORK);
}

}  // namespace rezin
