// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_RESOURCE_ENTRY_HPP_
#define REZIN_RESOURCE_ENTRY_HPP_

#include <stdint.h>
#include "sfz/Bytes.hpp"
#include "sfz/Macros.hpp"
#include "sfz/String.hpp"

namespace rezin {

// Represents an individual entry within the resource fork.
//
// A ResourceType object is presented roughly as a STL map from 16-bit integers to ResourceEntry
// objects: it supports indexing via the at() method; and iteration via the begin() and end()
// methods.  Access is read-only.
class ResourceEntry {
  public:
    // @returns             The ID of this entry.
    int16_t id() const;

    // @returns             The name of this entry (if any).  If the resource does not have a name,
    //                      returns the empty string.
    const sfz::String& name() const;

    // @returns             The block of data corresponding to this entry.
    const sfz::BytesPiece& data() const;

  private:
    friend class ResourceType;

    // Create a resource entry from the appropriate segments of the resource fork.
    //
    // @param [in] entry_data A block of data containing all resource entries for a type.
    // @param [in] index    The index of the particular entry to read in.
    // @param [in] name_data The block of data containing all resource names.
    // @param [in] data_data The block of data containing all resource data.
    ResourceEntry(
            const sfz::BytesPiece& entry_data, int index, const sfz::BytesPiece& name_data,
            const sfz::BytesPiece& data_data);

    // The ID of this resource entry.
    int16_t _id;

    // The name of this entry (if any).
    sfz::String _name;

    // The block of data corresponding to this entry.
    sfz::BytesPiece _data;

    DISALLOW_COPY_AND_ASSIGN(ResourceEntry);
};

}  // namespace rezin

#endif  // REZIN_RESOURCE_ENTRY_HPP_
