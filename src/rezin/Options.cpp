// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/Options.hpp"

using sfz::BytesPiece;
using sfz::PrintTarget;
using sfz::String;
using sfz::StringPiece;
namespace macroman = sfz::macroman;

namespace rezin {

namespace {

void convert_cr(String* string, const StringPiece& replacement) {
    String result;
    foreach (it, *string) {
        if (*it == '\r') {
            result.append(replacement);
        } else {
            result.append(1, *it);
        }
    }
    result.swap(string);
}

}  // namespace

Options::Options()
        : _line_ending(NL) { }

Options::LineEnding Options::line_ending() const {
    return _line_ending;
}

void Options::set_line_ending(LineEnding line_ending) {
    _line_ending = line_ending;
}

Options::EncodedString Options::decode(const sfz::BytesPiece& bytes) const {
    EncodedString result = { bytes, _line_ending };
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
    out.append(result);
}

}  // namespace rezin