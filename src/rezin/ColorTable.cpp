// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/ColorTable.hpp>

#include <rezin/ColorTableInternal.hpp>
#include <rgos/rgos.hpp>
#include <sfz/sfz.hpp>

using rgos::Json;
using sfz::BytesPiece;
using sfz::Exception;
using sfz::format;
using sfz::read;

namespace rezin {

Json read_clut(const BytesPiece& in) {
    BytesPiece remainder(in);
    ColorTable clut;
    read(&remainder, &clut);
    if (!remainder.empty()) {
        throw Exception(format("{0} extra bytes at end of 'clut' resource.", remainder.size()));
    }
    return clut.to_json();
}

}  // namespace rezin
