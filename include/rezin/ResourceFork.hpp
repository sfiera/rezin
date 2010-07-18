// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_RESOURCE_FORK_HPP_
#define REZIN_RESOURCE_FORK_HPP_

#include <map>
#include <sfz/sfz.hpp>

namespace rezin {

class Options;
class ResourceType;

// Represents the resource fork of a file.
//
// The resource fork is presented roughly as a STL map from strings to ResourceType objects: it
// supports indexing via the at() method; and iteration via the begin() and end() methods.  Access
// is read-only.
//
// The format of the resource fork is documented in "Inside Macintosh: More Macintosh Toolbox", pp.
// 1-121 to 1-125.
class ResourceFork {
  public:
    // Reads the resource fork from a block of memory.
    //
    // @param [in] data     A block of memory containing the resource fork of a file.
    // @param [in] options  Miscellaneous options.
    ResourceFork(const sfz::BytesPiece& data, const Options& options);

    ~ResourceFork();

    // Gets an individual resource type, e.g. "PICT" or "STR#".
    //
    // @param [in] code     The 4-character code of the resource type to get.
    // @throws Exception    If the resource fork does not contain the given type.
    const ResourceType& at(const sfz::StringPiece& code) const;

    // STL-like iterator type.
    class const_iterator {
      public:
        typedef ResourceFork container_type;
        typedef const ResourceType* pointer;
        typedef const ResourceType* const_pointer;
        typedef const ResourceType& reference;
        typedef const ResourceType& const_reference;

        const_reference operator*() const { return *_it->second; }
        const_pointer operator->() const { return _it->second; }
        const_iterator& operator++() { ++_it; return *this; }
        const_iterator operator++(int) { return iterator(_it++); }
        bool operator==(const_iterator it) { return _it == it._it; }
        bool operator!=(const_iterator it) { return _it != it._it; }

      private:
        friend class ResourceFork;
        const_iterator(std::map<sfz::StringKey, ResourceType*>::const_iterator it) : _it(it) { }
        std::map<sfz::StringKey, ResourceType*>::const_iterator _it;
    };
    typedef const_iterator iterator;

    // STL-like iterators over the types in the resource fork.
    const_iterator begin() const;
    const_iterator end() const;

  private:
    // The map represented by this object.
    std::map<sfz::StringKey, ResourceType*> _types;

    DISALLOW_COPY_AND_ASSIGN(ResourceFork);
};

}  // namespace rezin

#endif  // REZIN_RESOURCE_FORK_HPP_
