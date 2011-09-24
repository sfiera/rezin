// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/commands/CatCommand.hpp>

#include <fcntl.h>
#include <rezin/ResourceEntry.hpp>
#include <rezin/ResourceFork.hpp>
#include <rezin/ResourceType.hpp>

using sfz::BytesSlice;
using sfz::Exception;
using sfz::StringSlice;
using sfz::format;
using sfz::args::store;
using sfz::args::store_const;
using sfz::string_to_int;
using std::vector;

namespace args = sfz::args;

namespace rezin {

CatCommand::CatCommand(args::Parser& parser, Command*& command) {
    args::Parser& cat = parser.add_subparser(
            "cat", "print binary resource data", store_const(command, this));
    cat.add_argument("type", store(_type))
        .required();
    cat.add_argument("id", store(_id))
        .required();
}

void CatCommand::run(const ResourceFork& rsrc, const Options& options) const {
    const ResourceEntry& entry = rsrc.at(_type).at(_id);
    BytesSlice data = entry.data();
    write(1, data.data(), data.size());
}

}  // namespace rezin
