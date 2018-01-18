// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/snd.hpp>

#include <string.h>
#include <sfz/sfz.hpp>

using sfz::StringMap;
using sfz::hex;
using sfz::range;
using std::make_pair;
using std::map;
using std::vector;

namespace rezin {

namespace {

// Read the header of a format 1 'snd ' resource.
//
// @param [in] in       The pn::file_view to read from.
void read_snd_format_1_header(pn::file_view in) {
    uint16_t synthesizer_count;
    in.read(&synthesizer_count).check();
    if (synthesizer_count != 1) {
        throw std::runtime_error(
                pn::format("can only handle 1 synthesizer; {0} found", synthesizer_count).c_str());
    }

    uint16_t type;
    in.read(&type).check();
    if (type != 5) {
        throw std::runtime_error(
                pn::format("can only handle sampledSynth; {0} found", type).c_str());
    }

    uint32_t options;
    in.read(&options).check();
    if ((options & 0x00F0) != options) {
        throw std::runtime_error(
                pn::format("can only handle initMono and initStereo, not 0x{0}", hex(options))
                        .c_str());
    }
}

// Read the header of a format 2 'snd ' resource.
//
// @param [in] in       The pn::file_view to read from.
void read_snd_format_2_header(pn::file_view in) { in.read(pn::pad(2)).check(); }

// Read an individual command from a 'snd ' resource.
//
// @param [in] in       The pn::file_view to read from.
// @param [out] command The sound command.
// @param [out] param1  The first parameter to the sound command.
// @param [out] param2  The second parameter to the sound command.
void read_snd_command(pn::file_view in, uint16_t* command, uint16_t* param1, uint32_t* param2) {
    in.read(command, param1, param2).check();
}

// Read the definition for a block of sampled sound data.
//
// Much of the data from the table is discarded: loop_start; loop_end; encoding; and
// base_frequency.  Hopefully this information is not important.
//
// @param [in] in       The pn::file_view to read from.
// @param [out] pointer Where to look for the block of sampled sound data.
// @param [out] size    The size of the sampled sound data.
// @param [out] rate    The sample rate of the sound in Hz, e.g. 44100, 22050, 11025.
void read_snd_data_table(pn::file_view in, uint32_t* pointer, uint32_t* size, double* rate) {
    uint32_t fixed_sample_rate;
    uint32_t loop_start;
    uint32_t loop_end;
    uint8_t  encoding;
    uint8_t  base_frequency;

    in.read(pointer, size, &fixed_sample_rate, &loop_start, &loop_end, &encoding, &base_frequency)
            .check();
    *rate = fixed_sample_rate / 65536.0;
}

// Reads the samples for a sound into a JSON array.
//
// @param [in] in       The pn::file_view to read from.
// @param [in] sample_count The number of samples to read.
// @param [out] samples The array to read the samples into.
void read_snd_samples(pn::file_view in, uint32_t sample_count, vector<uint8_t>* samples) {
    for (uint32_t i : range(sample_count)) {
        static_cast<void>(i);
        uint8_t sample;
        in.read(&sample).check();
        samples->push_back(sample);
    }
}

}  // namespace

Sound::Sound(pn::data_view in) {
    pn::file header = in.open();
    header.read(&fmt).check();
    if (fmt == 1) {
        read_snd_format_1_header(header);
    } else if (fmt == 2) {
        read_snd_format_2_header(header);
    } else {
        throw std::runtime_error(pn::format("unknown 'snd ' format '{0}'", fmt).c_str());
    }

    uint16_t command_count;
    header.read(&command_count).check();
    if (command_count != 1) {
        throw std::runtime_error(
                pn::format("can only handle 1 command; {0} found", command_count).c_str());
    }

    uint16_t command;
    uint16_t zero;
    uint32_t offset;
    read_snd_command(header, &command, &zero, &offset);
    if (command != 0x8051) {
        throw std::runtime_error(
                pn::format("can only handle bufferCmd; 0x{0} found", hex(command, 4)).c_str());
    }
    if (zero != 0) {
        throw std::runtime_error(pn::format("param1 must be zero; {0} found", zero).c_str());
    }

    pn::file sound = in.slice(offset).open();
    uint32_t pointer;
    uint32_t sample_count;
    read_snd_data_table(sound, &pointer, &sample_count, &sample_rate);
    channels    = 1;
    sample_bits = 8;

    pn::file sample = in.slice(offset + 22 + pointer, sample_count).open();
    read_snd_samples(sample, sample_count, &samples);
}

namespace {

// Write `d` as an IEEE 754 80-bit floating point (extended precision) number.
//
// Not verified for special values (positive and negative zero and infinity, NaN) correctly,
// because those aren't valid sample rates, and that's the reason this function was written.
//
// @param [out] out     The pn::file_view to write the value to.
// @param [in] d        A double-precision floating point value.
void write_float80(pn::file_view out, double d) {
    // Reinterpret `d` as uint64_t, since we will be manupulating bits.
    uint64_t sample_rate_bits;
    memcpy(&sample_rate_bits, &d, sizeof(uint64_t));

    // Deconstruct the double:
    //   * the sign, 1 bit.
    //   * the exponent, 11 bits with a bias of 1023.
    //   * the fraction, 52 bits, with an implicit '1' bit preceding.
    uint64_t sign     = sample_rate_bits >> 63;
    uint64_t exponent = ((sample_rate_bits >> 52) & ((1 << 11) - 1)) - 1023;
    uint64_t fraction = (1ull << 52) | (sample_rate_bits & ((1ull << 52) - 1));

    // Reconstruct the 80-bit float:
    //   * the sign, 1 bit.
    //   * the exponent, 15 bits with a bias of 16383.
    //   * the fraction, 64 bits, with no implicit '1' bit.
    out.write<uint16_t, uint64_t>((sign << 15) | ((exponent + 16383) & 0x7FFF), fraction << 11)
            .check();
}

// Write an IFF chunk.
//
// @param [out] out     The pn::file_view to write the chunk to.
// @param [in] name     A four-byte chunk name.
// @param [in] data     The content of the chunk.
void write_chunk(pn::file_view out, const char* name, const pn::data_view& data) {
    out.write<pn::string_view, uint32_t, pn::data_view>({name, 4}, data.size(), data).check();
}

// Write an AIFF "COMM" chunk.
//
// The "COMM" (common) chunk specifies how to interpret samples from the "SSND" chunk.
//
// @param [out] out     The pn::file_view to write the chunk to.
// @param [in] info     Contains the information to write.
void write_comm(pn::file_view out, const Sound& sound) {
    pn::data comm;
    pn::file f = comm.open("w");
    f.write<int16_t, uint32_t, int16_t>(sound.channels, sound.samples.size(), sound.sample_bits)
            .check();
    write_float80(f, sound.sample_rate);

    write_chunk(out, "COMM", comm);
}

// Write an AIFF "SSND" chunk.
//
// The "SSND" (sampled sound) chunk is an array of samples from the sound.
//
// @param [out] out     The pn::file_view to write the chunk to.
// @param [in] sound    Contains the samples to write.
void write_ssnd(pn::file_view out, const Sound& sound) {
    pn::data ssnd;
    pn::file f = ssnd.open("w");
    f.write<uint32_t, uint32_t>(0, 0).check();
    for (uint8_t sample : sound.samples) {
        f.write<int8_t>(sample - 0x80).check();
    }

    write_chunk(out, "SSND", ssnd);
}

// Write an AIFF file.
//
// IFF files (including AIFF files) are structured as a hierarchy of chunks.  Given `sound`, which
// contains sound data, this function writes the "FORM" chunk which is at the top level of every
// IFF file, which consists of the identifier "AIFF", followed by the two chunks, "COMM" and
// "SSND", which define the content of the sound.
//
// @param [out] out     The pn::file_view to write the sound data to.
// @param [in] sound    Contains the sound to write.
void write_form(pn::file_view out, const Sound& sound) {
    pn::data form;
    pn::file f = form.open("w");
    f.write(pn::string_view{"AIFF", 4}).check();
    write_comm(f, sound);
    write_ssnd(f, sound);

    write_chunk(out, "FORM", form);
}

}  // namespace

pn::data aiff(const Sound& sound) {
    pn::data d;
    write_form(d.open("w"), sound);
    return d;
}

}  // namespace rezin
