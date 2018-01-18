// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/sources/file.hpp>

#include <sfz/sfz.hpp>

namespace rezin {

FlatFileSource::FlatFileSource(pn::string_view path) : _path(path.copy()) {}

void FlatFileSource::load() { _file.reset(new sfz::mapped_file(_path)); }

pn::data_view FlatFileSource::data() const { return _file->data(); }

}  // namespace rezin
