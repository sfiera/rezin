// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_RESOURCE_HPP_
#define REZIN_RESOURCE_HPP_

#include <sfz/sfz.hpp>

namespace rezin {

class Options;
class ResourceEntry;
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
    ResourceFork(const sfz::BytesSlice& data, const Options& options);

    ~ResourceFork();

    // Gets an individual resource type, e.g. "PICT" or "STR#".
    //
    // @param [in] code     The 4-character code of the resource type to get.
    // @throws Exception    If the resource fork does not contain the given type.
    const ResourceType& at(const sfz::StringSlice& code) const;

    // STL-like iterator type.
    class const_iterator {
      public:
        typedef ResourceFork container_type;
        typedef const ResourceType* pointer;
        typedef const ResourceType* const_pointer;
        typedef const ResourceType& reference;
        typedef const ResourceType& const_reference;

        const_reference operator*() const { return *_it->second; }
        const_pointer operator->() const { return _it->second.get(); }
        const_iterator& operator++() { ++_it; return *this; }
        const_iterator operator++(int) { const_iterator old = *this; ++_it; return old; }
        bool operator==(const_iterator it) { return _it == it._it; }
        bool operator!=(const_iterator it) { return _it != it._it; }

      private:
        friend class ResourceFork;
        const_iterator(sfz::StringMap<sfz::linked_ptr<ResourceType> >::const_iterator it) : _it(it) { }
        sfz::StringMap<sfz::linked_ptr<ResourceType> >::const_iterator _it;
    };
    typedef const_iterator iterator;

    // STL-like iterators over the types in the resource fork.
    const_iterator begin() const;
    const_iterator end() const;

  private:
    // The map represented by this object.
    sfz::StringMap<sfz::linked_ptr<ResourceType> > _types;

    DISALLOW_COPY_AND_ASSIGN(ResourceFork);
};

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
        const ResourceEntry* operator->() const { return _it->second.get(); }
        const_iterator& operator++() { ++_it; return *this; }
        const_iterator operator++(int) { return iterator(_it++); }
        bool operator==(const_iterator it) { return _it == it._it; }
        bool operator!=(const_iterator it) { return _it != it._it; }

      private:
        friend class ResourceType;
        const_iterator(std::map<int16_t, sfz::linked_ptr<ResourceEntry> >::const_iterator it) : _it(it) { }
        std::map<int16_t, sfz::linked_ptr<ResourceEntry> >::const_iterator _it;
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
    // @param [in] options  Miscellaneous options.
    ResourceType(
            const sfz::BytesSlice& type_data, int index, const sfz::BytesSlice& name_data,
            const sfz::BytesSlice& data_data, const Options& options);

    // The 4-character code of this resource type.
    sfz::String _code;

    // The map represented by this object.
    std::map<int16_t, sfz::linked_ptr<ResourceEntry> > _entries;

    DISALLOW_COPY_AND_ASSIGN(ResourceType);
};

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
    const sfz::BytesSlice& data() const;

  private:
    friend class ResourceType;

    // Create a resource entry from the appropriate segments of the resource fork.
    //
    // @param [in] entry_data A block of data containing all resource entries for a type.
    // @param [in] index    The index of the particular entry to read in.
    // @param [in] name_data The block of data containing all resource names.
    // @param [in] data_data The block of data containing all resource data.
    // @param [in] options  Miscellaneous options.
    ResourceEntry(
            const sfz::BytesSlice& entry_data, int index, const sfz::BytesSlice& name_data,
            const sfz::BytesSlice& data_data, const Options& options);

    // The ID of this resource entry.
    int16_t _id;

    // The name of this entry (if any).
    sfz::String _name;

    // The block of data corresponding to this entry.
    sfz::BytesSlice _data;

    DISALLOW_COPY_AND_ASSIGN(ResourceEntry);
};

}  // namespace rezin

#endif  // REZIN_RESOURCE_HPP_
