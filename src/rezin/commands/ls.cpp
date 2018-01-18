// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/commands/ls.hpp>

#include <rezin/resource.hpp>
#include <sfz/sfz.hpp>

namespace args = sfz::args;

namespace rezin {

LsCommand::LsCommand() = default;

bool LsCommand::argument(pn::string_view arg) {
    if (!_type.has_value()) {
        _type.emplace(arg.copy());
    } else if (!_id.has_value()) {
        args::integer_option(arg, &_id);
    } else {
        return false;
    }
    return true;
}

void LsCommand::run(const ResourceFork& rsrc, const Options& options) const {
    if (!_type.has_value()) {
        for (const ResourceType& type : rsrc) {
            pn::format(stdout, "{0}\n", type.code());
        }
        return;
    }

    const ResourceType& type = rsrc.at(*_type);
    if (!_id.has_value()) {
        for (const ResourceEntry& entry : type) {
            pn::format(stdout, "{0}\t{1}\n", entry.id(), entry.name());
        }
        return;
    }

    const ResourceEntry& entry = type.at(*_id);
    pn::format(stdout, "{0}\t{1}\n", entry.id(), entry.name());
}

}  // namespace rezin
