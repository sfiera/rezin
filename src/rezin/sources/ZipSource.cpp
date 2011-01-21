// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/sources/ZipSource.hpp>

#include <rezin/AppleSingle.hpp>
#include <zipxx/zipxx.hpp>

using sfz::BytesSlice;
using sfz::Exception;
using sfz::String;
using sfz::StringSlice;
using sfz::format;
using zipxx::ZipArchive;
using zipxx::ZipFileReader;

namespace rezin {

ZipSource::ZipSource(const StringSlice& arg) {
    String arg_string(arg);
    size_t comma = arg_string.find(',');
    if (comma == String::npos) {
        throw Exception(format("invalid format to --zip-file {0}.", quote(arg_string)));
    }
    _archive_path.assign(arg_string.slice(0, comma));

    _file_path.append("__MACOSX/");
    _file_path.append(arg_string.slice(comma + 1));
    size_t loc = _file_path.rfind('/');
    _file_path.replace(loc, 1, "/._");
}

ZipSource::~ZipSource() { }

BytesSlice ZipSource::load() {
    _archive.reset(new ZipArchive(_archive_path, 0));
    _file.reset(new ZipFileReader(_archive.get(), _file_path));
    _apple_single.reset(new AppleDouble(_file->data()));
    return _apple_single->at(AppleDouble::RESOURCE_FORK);
}

}  // namespace rezin
