// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/commands/cat.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <rezin/resource.hpp>

namespace args = sfz::args;

namespace rezin {

CatCommand::CatCommand() = default;

bool CatCommand::argument(pn::string_view arg) {
    if (!_type.has_value()) {
        _type.emplace(arg.copy());
    } else if (!_id.has_value()) {
        args::integer_option(arg, &_id);
    } else {
        return false;
    }
    return true;
}

void CatCommand::run(const ResourceFork& rsrc, const Options& options) const {
    const ResourceEntry& entry = rsrc.at(*_type).at(*_id);
    pn::data_view        data  = entry.data();
    pn::file_view{stdout}.write(data).check();
}

}  // namespace rezin
