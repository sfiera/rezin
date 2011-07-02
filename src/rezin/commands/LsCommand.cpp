// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/commands/LsCommand.hpp>

#include <rezin/ResourceEntry.hpp>
#include <rezin/ResourceFork.hpp>
#include <rezin/ResourceType.hpp>
#include <sfz/sfz.hpp>

using sfz::Exception;
using sfz::StringSlice;
using sfz::format;
using sfz::print;
using std::vector;

namespace io = sfz::io;

namespace rezin {

LsCommand::LsCommand(const vector<StringSlice>& args)
        : _argc(args.size()) {
    if (args.size() > 3) {
        throw Exception(format("too many arguments to command \"ls\"."));
    }
    if (args.size() > 2) {
        if (!string_to_int(args[2], _id)) {
            throw Exception(format("invalid resource ID {0}.", quote(args[2])));
        }
    }
    if (args.size() > 1) {
        _code.assign(args[1]);
    }
}

void LsCommand::run(const ResourceFork& rsrc) {
    if (_argc == 1) {
        SFZ_FOREACH(const ResourceType& type, rsrc, {
            print(io::out, format("{0}\n", type.code()));
        });
    } else if (_argc == 2) {
        const ResourceType& type = rsrc.at(_code);
        SFZ_FOREACH(const ResourceEntry& entry, type, {
            print(io::out, format("{0}\t{1}\n", entry.id(), entry.name()));
        });
    } else if (_argc == 3) {
        const ResourceEntry& entry = rsrc.at(_code).at(_id);
        print(io::out, format("{0}\t{1}\n", entry.id(), entry.name()));
    }
}

}  // namespace
