// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/options.hpp>

namespace macroman = sfz::macroman;

namespace rezin {

namespace {

void convert_cr(pn::string* string, const pn::string_view& replacement) {
    pn::string result;
    for (pn::rune r : *string) {
        if (r == pn::rune{'\r'}) {
            result += replacement;
        } else {
            result += pn::rune{r};
        }
    }
    result.swap(*string);
}

}  // namespace

Options::Options() : line_ending(NL) {}

pn::string Options::decode(const pn::data_view& d) const {
    pn::string result = macroman::decode(d);
    switch (line_ending) {
        case Options::CR: break;
        case Options::NL: convert_cr(&result, "\n"); break;
        case Options::CRNL: convert_cr(&result, "\r\n"); break;
    }
    return result;
}

/*
bool store_argument(Options::LineEnding& to, pn::string_view value, PrintTarget error) {
    if (value == "cr") {
        to = Options::CR;
    } else if (value == "nl") {
        to = Options::NL;
    } else if (value == "crnl") {
        to = Options::CRNL;
    } else {
        print(error,
              format("invalid line ending {0}: must be one of cr, nl, or crnl", quote(value)));
        return false;
    }
    return true;
}
*/

}  // namespace rezin
