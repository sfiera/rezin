// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/sources/ZipSource.hpp>

#include <rezin/AppleSingle.hpp>

using sfz::BytesSlice;
using sfz::Exception;
using sfz::PrintTarget;
using sfz::String;
using sfz::StringSlice;
using sfz::format;
using zipxx::ZipArchive;
using zipxx::ZipFileReader;

namespace rezin {

ZipSource::ZipSource() { }

ZipSource::~ZipSource() { }

struct ZipSource::Contents {
    Contents(sfz::StringSlice zip_file, sfz::StringSlice file_path);

    zipxx::ZipArchive archive;
    zipxx::ZipFileReader file;
    AppleSingle apple_single;
};

ZipSource::Contents::Contents(sfz::StringSlice zip_file, sfz::StringSlice file_path):
        archive(zip_file, 0),
        file(archive, file_path),
        apple_single(file.data()) { }

void ZipSource::load(sfz::StringSlice zip_file, sfz::StringSlice file_path) {
    _contents.reset(new Contents(zip_file, file_path));
}

BytesSlice ZipSource::data() const {
    return _contents->apple_single.at(AppleDouble::RESOURCE_FORK);
}

void swap(ZipSource& x, ZipSource& y) {
    swap(x._contents, y._contents);
}

bool store_argument(ZipSource& to, sfz::StringSlice value, PrintTarget error) {
    StringSlice zip_file;
    if (!partition(zip_file, ",", value)) {
        print(error, format("invalid format {0}.", quote(value)));
    }

    String file_path("__MACOSX/");
    file_path.append(value);
    size_t loc = file_path.rfind('/');
    file_path.replace(loc, 1, "/._");

    to.load(zip_file, file_path);
    return true;
}

}  // namespace rezin
