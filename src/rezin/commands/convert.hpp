// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_COMMANDS_CONVERT_HPP_
#define REZIN_COMMANDS_CONVERT_HPP_

#include <rezin/command.hpp>
#include <sfz/sfz.hpp>
#include <vector>

namespace rezin {

class ResourceFork;

class ConvertCommand : public Command {
  public:
    ConvertCommand();

    virtual bool argument(pn::string_view arg);
    virtual void run(const ResourceFork& rsrc, const Options& options) const;

  private:
    sfz::optional<pn::string> _type;
    sfz::optional<int16_t>    _id;

    ConvertCommand(const ConvertCommand&) = delete;
    ConvertCommand& operator=(const ConvertCommand&) = delete;
};

}  // namespace rezin

#endif  // REZIN_COMMANDS_CONVERT_HPP_
