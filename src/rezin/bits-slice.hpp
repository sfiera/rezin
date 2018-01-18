// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_BITS_PIECE_HPP_
#define REZIN_BITS_PIECE_HPP_

#include <sfz/sfz.hpp>

namespace rezin {

class BitsSlice {
  public:
    explicit BitsSlice(const pn::data_view& data);

    void shift(int size);
    void shift(uint8_t* data, size_t size);

  private:
    pn::data_view _data;
    int           _bit_index;
};

}  // namespace rezin

#endif  // REZIN_BITS_PIECE_HPP_
