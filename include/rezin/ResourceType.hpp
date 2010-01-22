// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_RESOURCE_TYPE_HPP_
#define REZIN_RESOURCE_TYPE_HPP_

#include <map>
#include <stdint.h>
#include "sfz/Bytes.hpp"
#include "sfz/Exception.hpp"
#include "sfz/Macros.hpp"
#include "sfz/String.hpp"

namespace rezin {

class ResourceEntry;

// Represents a collection of resources with the same type within the resource fork.
//
// A ResourceType object is presented roughly as a STL map from 16-bit integers to ResourceEntry
// objects: it supports indexing via the at() method; and iteration via the begin() and end()
// methods.  Access is read-only.
class ResourceType {
  public:
    ~ResourceType();

    // @returns             The four-character code of this resource type.  Is UTF-8 encoded; see
    //                      documentation for 'ResourceFork::at()' for ramifications.
    const sfz::String& code() const;

    // Gets an individual resource entry, e.g. "PICT 128" or "STR# 500".
    //
    // @param [in] id       The id of the resource entry to get.
    // @returns             A ResourceEntry object corresponding to `code`.
    // @throws Exception    If the resource type does not contain the given ID.
    const ResourceEntry& at(int16_t id) const;

    // STL-like iterator type.
    class const_iterator {
      public:
        typedef ResourceType container_type;
        typedef const ResourceEntry* pointer;
        typedef const ResourceEntry* const_pointer;
        typedef const ResourceEntry& reference;
        typedef const ResourceEntry& const_reference;

        const ResourceEntry& operator*() const { return *_it->second; }
        const ResourceEntry* operator->() const { return _it->second; }
        const_iterator& operator++() { ++_it; return *this; }
        const_iterator operator++(int) { return iterator(_it++); }
        bool operator==(const_iterator it) { return _it == it._it; }
        bool operator!=(const_iterator it) { return _it != it._it; }

      private:
        friend class ResourceType;
        const_iterator(std::map<int16_t, ResourceEntry*>::const_iterator it) : _it(it) { }
        std::map<int16_t, ResourceEntry*>::const_iterator _it;
    };
    typedef const_iterator iterator;

    // STL-like iterators over the entries in the resource type.
    const_iterator begin() const;
    const_iterator end() const;

  private:
    friend class ResourceFork;

    // Create a resource type from the appropriate segments of the resource fork.
    //
    // @param [in] type_data A block of data containing all resource types.
    // @param [in] index    The index of the particular type to read in.
    // @param [in] name_data The block of data containing all resource names.
    // @param [in] data_data The block of data containing all resource data.
    ResourceType(
            const sfz::BytesPiece& type_data, int index, const sfz::BytesPiece& name_data,
            const sfz::BytesPiece& data_data);

    // The 4-character code of this resource type.
    sfz::String _code;

    // The map represented by this object.
    std::map<int16_t, ResourceEntry*> _entries;

    DISALLOW_COPY_AND_ASSIGN(ResourceType);
};

}  // namespace rezin

#endif  // REZIN_RESOURCE_TYPE_HPP_
