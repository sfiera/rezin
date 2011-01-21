// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#ifndef REZIN_SOUND_HPP_
#define REZIN_SOUND_HPP_

#include <rgos/rgos.hpp>
#include <sfz/sfz.hpp>

namespace rezin {

// Converts 'snd ' resource data into a JSON object.
//
// The returned data is in the following format:
//
// {
//   "format": 1,
//   "channels": 1,
//   "sample_bits": 8,
//   "sample_rate": 44100,
//   "samples": [0, 1, 2, ...]
// }
//
// "format" specifies whether the source was a version 1 or version 2 'snd ' resource.  "channels"
// gives the number of sound channels; currently only "1" (mono) is supported.  "sample_bits" gives
// the number of bits per sample, which is always 8.  "sample_rate" is the sample rate for the
// samples, usually 44100, 22050, or 11025.  "samples" is an array of integral samples in the range
// [0, 255].
//
// @param [in] in       The content of a 'snd ' resource.
// @returns             A JSON object representing the content of `in`.
// @throws Exception    If the 'snd ' data could not be read.
rgos::Json read_snd(const sfz::BytesSlice& in);

// Converts JSON sound data into AIFF data.
//
// JSON data is required to be as returned by read_snd().  It would be possible for us to interpret
// a wider range of sound data (e.g. "sample_bits" of 16) but currently we observe the same
// restrictions on input as read_snd() does on output.
//
// @param [in] sound    Sound data as returned by read_snd().
// @param [out] out     AIFF data is appended to this target.
// @throws Exception    If the JSON data could not be interpreted.
void write_aiff(sfz::WriteTarget out, const rgos::Json& sound);

}  // namespace rezin

#endif // REZIN_SOUND_HPP_
