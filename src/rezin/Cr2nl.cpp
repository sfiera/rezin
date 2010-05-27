// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/Cr2nl.hpp"

#include "sfz/sfz.hpp"

using sfz::String;

namespace rezin {

void cr2nl(String* string) {
    String result;
    foreach (it, *string) {
        if (*it == '\r') {
            result.append(1, '\n');
        } else {
            result.append(1, *it);
        }
    }
    result.swap(string);
}

}  // namespace rezin
