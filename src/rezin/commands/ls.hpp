// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_COMMANDS_LS_HPP_
#define REZIN_COMMANDS_LS_HPP_

#include <rezin/command.hpp>
#include <sfz/sfz.hpp>
#include <vector>

namespace rezin {

class ResourceFork;

class LsCommand : public Command {
  public:
    LsCommand(sfz::args::Parser& parser, Command*& command);

    virtual void run(const ResourceFork& rsrc, const Options& options) const;

  private:
    sfz::Optional<sfz::String> _type;
    sfz::Optional<int16_t>     _id;

    DISALLOW_COPY_AND_ASSIGN(LsCommand);
};

}  // namespace rezin

#endif  // REZIN_COMMANDS_LS_HPP_
