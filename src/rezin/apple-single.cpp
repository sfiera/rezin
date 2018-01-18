// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/apple-single.hpp>

#include <arpa/inet.h>
#include <sfz/sfz.hpp>

using sfz::hex;
using sfz::range;

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

AppleSingle::AppleSingle(const pn::data_view& data) {
    bool     little_endian = false;
    pn::file remainder     = data.open();
    uint32_t magic;
    remainder.read(&magic).check();
    switch (magic) {
        case APPLE_SINGLE_MAGIC:
        case APPLE_DOUBLE_MAGIC: break;

        case APPLE_SINGLE_CIGAM:
        case APPLE_DOUBLE_CIGAM: little_endian = true; break;

        default:
            throw std::runtime_error(
                    pn::format("invalid magic number 0x{0}.", hex(magic, 8)).c_str());
    }

    uint32_t version;
    remainder.read(&version).check();
    if (little_endian) {
        version = htonl(version);
    }

    switch (version) {
        case APPLE_SINGLE_VERSION_2: {
            uint16_t entry_count;
            remainder.read(pn::pad(16), &entry_count).check();
            if (little_endian) {
                entry_count = htons(entry_count);
            }

            for (uint16_t i : range(entry_count)) {
                static_cast<void>(i);
                uint32_t id;
                uint32_t offset;
                uint32_t length;
                remainder.read(&id, &offset, &length).check();

                if (little_endian) {
                    id     = htonl(id);
                    offset = htonl(offset);
                    length = htonl(length);
                }

                _entries.insert(std::make_pair(id, data.slice(offset, length)));
            }
        } break;

        default:
            throw std::runtime_error(
                    pn::format("unknown version {0}.", version / 65536.0).c_str());
    }
}

const pn::data_view& AppleSingle::at(uint32_t id) {
    std::map<uint32_t, pn::data_view>::const_iterator it = _entries.find(id);
    if (it == _entries.end()) {
        throw std::runtime_error(pn::format("no such id '{0}'", id).c_str());
    }
    return it->second;
}

}  // namespace rezin
