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
using sfz::args::store;
using sfz::args::store_const;
using sfz::format;
using sfz::print;
using std::vector;

namespace args = sfz::args;

namespace io = sfz::io;

namespace rezin {

LsCommand::LsCommand(args::Parser& parser, Command*& command) {
    args::Parser& ls = parser.add_subparser(
            "ls", "list resources", store_const(command, this));
    ls.add_argument("type", store(_type));
    ls.add_argument("id", store(_id));
}

void LsCommand::run(const ResourceFork& rsrc, const Options& options) const {
    if (!_type.has()) {
        SFZ_FOREACH(const ResourceType& type, rsrc, {
            print(io::out, format("{0}\n", type.code()));
        });
        return;
    }

    const ResourceType& type = rsrc.at(*_type);
    if (!_id.has()) {
        SFZ_FOREACH(const ResourceEntry& entry, type, {
            print(io::out, format("{0}\t{1}\n", entry.id(), entry.name()));
        });
        return;
    }

    const ResourceEntry& entry = type.at(*_id);
    print(io::out, format("{0}\t{1}\n", entry.id(), entry.name()));
}

}  // namespace
