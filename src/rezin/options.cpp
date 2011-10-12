// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/options.hpp>

using sfz::BytesSlice;
using sfz::PrintTarget;
using sfz::Rune;
using sfz::String;
using sfz::StringSlice;
namespace macroman = sfz::macroman;

namespace rezin {

namespace {

void convert_cr(String* string, const StringSlice& replacement) {
    String result;
    SFZ_FOREACH(Rune r, *string, {
        if (r == '\r') {
            result.append(replacement);
        } else {
            result.append(1, r);
        }
    });
    swap(result, *string);
}

}  // namespace

Options::Options()
        : line_ending(NL) { }

Options::EncodedString Options::decode(const sfz::BytesSlice& bytes) const {
    EncodedString result = { bytes, line_ending };
    return result;
}

void print_to(PrintTarget out, const Options::EncodedString& encoded) {
    String result;
    switch (encoded.line_ending) {
      case Options::CR:
        result.assign(macroman::decode(encoded.bytes));
        break;
      case Options::NL:
        result.assign(macroman::decode(encoded.bytes));
        convert_cr(&result, "\n");
        break;
      case Options::CRNL:
        result.assign(macroman::decode(encoded.bytes));
        convert_cr(&result, "\r\n");
        break;
    }
    out.push(result);
}

bool store_argument(Options::LineEnding& to, StringSlice value, PrintTarget error) {
    if (value == "cr") {
        to = Options::CR;
    } else if (value == "nl") {
        to = Options::NL;
    } else if (value == "crnl") {
        to = Options::CRNL;
    } else {
        print(error, format(
                    "invalid line ending {0}: must be one of cr, nl, or crnl", quote(value)));
        return false;
    }
    return true;
}

}  // namespace rezin
