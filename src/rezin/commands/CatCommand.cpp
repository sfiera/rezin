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
using sfz::string_to_int;
using std::vector;

namespace rezin {

CatCommand::CatCommand(const vector<StringSlice>& args) {
    if (args.size() != 3) {
        throw Exception(format("wrong number of arguments to command \"cat\"."));
    }
    _code.assign(args[1]);
    if (!string_to_int(args[2], _id)) {
        throw Exception(format("invalid resource ID {0}.", quote(args[2])));
    }
}

void CatCommand::run(const ResourceFork& rsrc) {
    const ResourceEntry& entry = rsrc.at(_code).at(_id);
    BytesSlice data = entry.data();
    write(1, data.data(), data.size());
}

}  // namespace rezin
