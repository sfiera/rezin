// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_COMMANDS_CAT_HPP_
#define REZIN_COMMANDS_CAT_HPP_

#include <vector>
#include <rezin/command.hpp>
#include <sfz/sfz.hpp>

namespace rezin {

class ResourceFork;

class CatCommand : public Command {
  public:
    CatCommand(sfz::args::Parser& parser, Command*& command);

    virtual void run(const ResourceFork& rsrc, const Options& options) const;

  private:
    sfz::String _type;
    int16_t _id;

    DISALLOW_COPY_AND_ASSIGN(CatCommand);
};

}  // namespace rezin

#endif  // REZIN_COMMANDS_CAT_HPP_
