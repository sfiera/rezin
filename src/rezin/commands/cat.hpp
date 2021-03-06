// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_COMMANDS_CAT_HPP_
#define REZIN_COMMANDS_CAT_HPP_

#include <rezin/command.hpp>
#include <sfz/sfz.hpp>
#include <vector>

namespace rezin {

class ResourceFork;

class CatCommand : public Command {
  public:
    CatCommand();

    virtual bool argument(pn::string_view arg);
    virtual void run(const ResourceFork& rsrc, const Options& options) const;

  private:
    sfz::optional<pn::string> _type;
    sfz::optional<int16_t>    _id;

    CatCommand(const CatCommand&) = delete;
    CatCommand& operator=(const CatCommand&) = delete;
};

}  // namespace rezin

#endif  // REZIN_COMMANDS_CAT_HPP_
