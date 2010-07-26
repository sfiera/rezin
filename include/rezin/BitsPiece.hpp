// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_BITS_PIECE_HPP_
#define REZIN_BITS_PIECE_HPP_

#include <sfz/sfz.hpp>

namespace rezin {

class BitsPiece {
  public:
    explicit BitsPiece(const sfz::BytesPiece& bytes);

    void shift(int size);
    void shift(uint8_t* data, size_t size);

  private:
    sfz::BytesPiece _bytes;
    int _bit_index;
};

}  // namespace rezin

#endif // REZIN_BITS_PIECE_HPP_
