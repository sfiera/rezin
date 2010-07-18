// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_OPTIONS_HPP_
#define REZIN_OPTIONS_HPP_

#include <sfz/sfz.hpp>

namespace rezin {

class Options {
  public:
    Options();

    enum LineEnding { CR, NL, CRNL };
    LineEnding line_ending() const;
    void set_line_ending(LineEnding line_ending);

    struct EncodedString;
    EncodedString decode(const sfz::BytesPiece& bytes) const;

  private:
    LineEnding _line_ending;
};

struct Options::EncodedString {
    const sfz::BytesPiece& bytes;
    LineEnding line_ending;
};
void print_to(sfz::PrintTarget out, const Options::EncodedString& encoded);

}  // namespace rezin

#endif  // REZIN_OPTIONS_HPP_
