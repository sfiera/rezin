// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_COMMANDS_CONVERT_COMMAND_HPP_
#define REZIN_COMMANDS_CONVERT_COMMAND_HPP_

#include <vector>
#include <rezin/Command.hpp>
#include <rezin/Options.hpp>
#include <sfz/sfz.hpp>

namespace rezin {

class Options;
class ResourceFork;

class ConvertCommand : public Command {
  public:
    ConvertCommand(const std::vector<sfz::StringPiece>& args, const Options& options);

    virtual void run(const ResourceFork& rsrc);

  private:
    sfz::String _code;
    int16_t _id;
    Options _options;

    DISALLOW_COPY_AND_ASSIGN(ConvertCommand);
};

}  // namespace rezin

#endif  // REZIN_COMMANDS_CONVERT_COMMAND_HPP_
