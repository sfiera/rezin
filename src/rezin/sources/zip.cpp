// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/sources/zip.hpp>

#include <rezin/apple-single.hpp>

using zipxx::ZipArchive;
using zipxx::ZipFileReader;

namespace rezin {

ZipSource::ZipSource(pn::string_view arg) {
    pn::string_view zip_path;
    if (!partition(zip_path, ",", arg)) {
        throw std::runtime_error(
                pn::format("invalid format {0}.", pn::dump(arg, pn::dump_short)).c_str());
    }
    _zip_path = zip_path.copy();

    _file_path = "__MACOSX/";
    _file_path += arg;
    size_t loc = _file_path.rfind(pn::rune{'/'});
    _file_path.replace(loc, 1, "/._");
}

ZipSource::~ZipSource() {}

struct ZipSource::Contents {
    Contents(pn::string_view zip_file, pn::string_view file_path);

    zipxx::ZipArchive    archive;
    zipxx::ZipFileReader file;
    AppleSingle          apple_single;
};

ZipSource::Contents::Contents(pn::string_view zip_file, pn::string_view file_path)
        : archive(zip_file, 0), file(archive, file_path), apple_single(file.data()) {}

void ZipSource::load() { _contents.reset(new Contents(_zip_path, _file_path)); }

pn::data_view ZipSource::data() const {
    return _contents->apple_single.at(AppleDouble::RESOURCE_FORK);
}

}  // namespace rezin
