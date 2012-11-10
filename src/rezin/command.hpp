// Copyright (c) 2010 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef REZIN_COMMAND_HPP_
#define REZIN_COMMAND_HPP_

namespace rezin {

struct Options;
class ResourceFork;

class Command {
  public:
    virtual ~Command() { }
    virtual void run(const ResourceFork& rsrc, const Options& options) const = 0;
};

}  // namespace rezin

#endif  // REZIN_COMMAND_HPP_
