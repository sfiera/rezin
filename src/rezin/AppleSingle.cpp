// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/AppleSingle.hpp"

#include <libkern/OSByteOrder.h>
#include "sfz/sfz.hpp"

using sfz::BytesPiece;
using sfz::Exception;
using sfz::format;
using sfz::hex;
using sfz::range;
using sfz::read;

namespace rezin {

namespace {

// Magic constants, one of which should be at the start of the file.
//
// The spec specifies the values of APPLE_SINGLE_MAGIC and APPLE_DOUBLE_MAGIC below as being
// appropriate for AppleSingle and AppleDouble files respectively.  It is also very clear that the
// byte order in the file should be big-endian, and not little-endian, but AppleSingle files
// encoded by `applesingle encode` on a i386 machine running OS X 10.6 clearly use little-endian
// byte order, so we support the "CIGAM" constants below as an extension.
enum AppleMagic {
    APPLE_SINGLE_MAGIC = 0x00051600,
    APPLE_SINGLE_CIGAM = 0x00160500,
    APPLE_DOUBLE_MAGIC = 0x00051607,
    APPLE_DOUBLE_CIGAM = 0x07160500,
};

// Versions of the AppleSingle specification, one of which should follow the magic number.
//
// We only support version 2 of the specification.
enum AppleSingleVersion {
    APPLE_SINGLE_VERSION_2 = 0x00020000,
};

}  // namespace

AppleSingle::AppleSingle(const BytesPiece& data) {
    bool little_endian = false;
    BytesPiece remainder(data);
    uint32_t magic;
    read(&remainder, &magic);
    switch (magic) {
      case APPLE_SINGLE_MAGIC:
      case APPLE_DOUBLE_MAGIC:
        break;

      case APPLE_SINGLE_CIGAM:
      case APPLE_DOUBLE_CIGAM:
        little_endian = true;
        break;

      default:
        throw Exception(format("invalid magic number 0x{0}.", hex(magic, 8)));
    }

    uint32_t version;
    read(&remainder, &version);
    if (little_endian) {
        version = OSSwapInt32(version);
    }

    switch (version) {
      case APPLE_SINGLE_VERSION_2:
        {
            remainder.shift(16);

            uint16_t entry_count;
            read(&remainder, &entry_count);
            if (little_endian) {
                entry_count = OSSwapInt16(entry_count);
            }

            foreach (i, range(entry_count)) {
                uint32_t id;
                uint32_t offset;
                uint32_t length;
                read(&remainder, &id);
                read(&remainder, &offset);
                read(&remainder, &length);

                if (little_endian) {
                    id = OSSwapInt32(id);
                    offset = OSSwapInt32(offset);
                    length = OSSwapInt32(length);
                }

                _entries.insert(std::make_pair(id, data.substr(offset, length)));
            }
        }
        break;

      default:
        throw Exception(format("unknown version {0}.", version / 65536.0));
    }
}

const BytesPiece& AppleSingle::at(uint32_t id) {
    std::map<uint32_t, BytesPiece>::const_iterator it = _entries.find(id);
    if (it == _entries.end()) {
        throw Exception(format("no such id '{0}'", id));
    }
    return it->second;
}

}  // namespace rezin
