// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_SOUND_HPP_
#define REZIN_SOUND_HPP_

#include <sfz/sfz.hpp>

namespace rezin {

struct AiffSound;

// Reads 'snd ' resource data object.
//
// "format" specifies whether the source was a version 1 or version 2 'snd ' resource.  "channels"
// gives the number of sound channels; currently only "1" (mono) is supported.  "sample_bits" gives
// the number of bits per sample, which is always 8.  "sample_rate" is the sample rate for the
// samples, usually 44100, 22050, or 11025.  "samples" is an array of integral samples in the range
// [0, 255].
//
// @param [in] in       The content of a 'snd ' resource.
// @throws Exception    If the 'snd ' data could not be read.
struct Sound {
    Sound(sfz::BytesSlice in);

    uint16_t fmt;
    uint32_t channels;
    uint32_t sample_bits;
    double sample_rate;
    std::vector<uint8_t> samples;
};

// Converts a Sound into AIFF data.
//
// Sound data is required to be as returned by read_snd().  It would be possible for us to
// interpret a wider range of sound data (e.g. "sample_bits" of 16) but currently we observe the
// same restrictions on input as read_snd() does on output.
AiffSound aiff(const Sound& sound);

struct AiffSound { const Sound& sound; };
void write_to(sfz::WriteTarget out, AiffSound aiff);

}  // namespace rezin

#endif // REZIN_SOUND_HPP_
