// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_APPLE_SINGLE_HPP_
#define REZIN_APPLE_SINGLE_HPP_

#include <stdint.h>
#include <map>
#include <sfz/sfz.hpp>

namespace rezin {

// Reads the contents of an AppleSingle- or AppleDouble-encoded file.
//
// The format of AppleSingle and AppleDouble files is documented in "AppleSingle/AppleDouble
// Formats for Foreign Files Developer's Note".  On OS X, it is possible to create an
// AppleSingle-encoded version of a file with the `applesingle encode` command.  Additionally,
// AppleDouble files are stored as part of any zip archive created with OS X metadata.
//
// This class will happily accept either AppleSingle- or AppleDouble-encoded data as input.
// However, when dealing with AppleDouble files, it is recommended to use the AppleDouble typedef
// to clarify intent.
class AppleSingle {
  public:
    // Chunk identifiers reserved by Apple in the AppleSingle/AppleDouble specification.
    //
    // '0' is not a valid chunk identifier, and the DATA_FORK chunk will only be present in
    // AppleSingle files.  Otherwise, the file may contain chunks corresponding to any integer;
    // these are simply a few well-known ones.
    enum PredefinedChunk {
        DATA_FORK           = 1,
        RESOURCE_FORK       = 2,
        REAL_NAME           = 3,
        COMMENT             = 4,
        ICON_BW             = 5,
        ICON_COLOR          = 6,
        FILE_DATES_INFO     = 8,
        FINDER_INFO         = 9,
        MACINTOSH_FILE_INFO = 10,
        PRO_DOS_FILE_INFO   = 11,
        MS_DOS_FILE_INFO    = 12,
        SHORT_NAME          = 13,
        AFP_FILE_INFO       = 14,
        DIRECTORY_ID        = 15,
    };

    // Initializes this object with the contents of `data`.
    //
    // @param [in] data     A block of memory containing the contents of an AppleSingle- or
    //                      AppleDouble-encoded file.  The block of memory must continue to remain
    //                      valid for the lifetime of this object; it is not copied.
    // @throws Exception    If the contents of `data` could not be parsed.
    AppleSingle(const sfz::BytesSlice& data);

    // Returns the chunk of data corresponding to the given identifier.  Identifiers 1-0x7FFFFFFF
    // are reserved by Apple in the specification, and several such well-known chunk identifiers
    // are listed here in the `PredefinedChunk` enum.
    //
    // @param [in] data     The identifier of the chunk to fetch.
    // @returns             The requested chunk of data.
    // @throws Exception    If there is no such chunk in the file.
    const sfz::BytesSlice& at(uint32_t id);

  private:
    // Map from chunk identifiers to chunk data blocks.
    std::map<uint32_t, sfz::BytesSlice> _entries;

    DISALLOW_COPY_AND_ASSIGN(AppleSingle);
};

// The structure of AppleSingle and AppleDouble files is identical; the one difference is that
// AppleDouble files never contain the DATA_FORK segment.  To make it more readable to use the
// AppleSingle class when AppleDouble files are being used, we provide a typedef.
typedef AppleSingle AppleDouble;

}  // namespace rezin

#endif  // REZIN_APPLE_SINGLE_HPP_
