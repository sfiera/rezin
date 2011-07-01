// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/Sound.hpp>

#include <sfz/sfz.hpp>

using sfz::Bytes;
using sfz::BytesSlice;
using sfz::Exception;
using sfz::Json;
using sfz::JsonDefaultVisitor;
using sfz::ReadSource;
using sfz::StringMap;
using sfz::StringSlice;
using sfz::WriteTarget;
using sfz::format;
using sfz::hex;
using sfz::range;
using sfz::read;
using sfz::write;
using std::make_pair;
using std::map;
using std::vector;

namespace rezin {

namespace {

// Read the header of a format 1 'snd ' resource.
//
// @param [in] in       The ReadSource to read from.
void read_snd_format_1_header(ReadSource in) {
    uint16_t synthesizer_count;
    read(in, synthesizer_count);
    if (synthesizer_count != 1) {
        throw Exception(format("can only handle 1 synthesizer; {0} found", synthesizer_count));
    }

    uint16_t type;
    read(in, type);
    if (type != 5) {
        throw Exception(format("can only handle sampledSynth; {0} found", type));
    }

    uint32_t options;
    read(in, options);
    if ((options & 0x00F0) != options) {
        throw Exception(format("can only handle initMono and initStereo, not 0x{0}", hex(options)));
    }
}

// Read the header of a format 2 'snd ' resource.
//
// @param [in] in       The ReadSource to read from.
void read_snd_format_2_header(ReadSource in) {
    in.shift(2);
}

// Read an individual command from a 'snd ' resource.
//
// @param [in] in       The ReadSource to read from.
// @param [out] command The sound command.
// @param [out] param1  The first parameter to the sound command.
// @param [out] param2  The second parameter to the sound command.
void read_snd_command(ReadSource in, uint16_t& command, uint16_t& param1, uint32_t& param2) {
    read(in, command);
    read(in, param1);
    read(in, param2);
}

// Read the definition for a block of sampled sound data.
//
// Much of the data from the table is discarded: loop_start; loop_end; encoding; and
// base_frequency.  Hopefully this information is not important.
//
// @param [in] in       The ReadSource to read from.
// @param [out] pointer Where to look for the block of sampled sound data.
// @param [out] size    The size of the sampled sound data.
// @param [out] rate    The sample rate of the sound in Hz, e.g. 44100, 22050, 11025.
void read_snd_data_table(ReadSource in, uint32_t& pointer, uint32_t& size, double& rate) {
    uint32_t fixed_sample_rate;
    uint32_t loop_start;
    uint32_t loop_end;
    uint8_t encoding;
    uint8_t base_frequency;

    read(in, pointer);
    read(in, size);
    read(in, fixed_sample_rate);
    rate = fixed_sample_rate / 65536.0;
    read(in, loop_start);
    read(in, loop_end);
    read(in, encoding);
    read(in, base_frequency);
}

// Reads the samples for a sound into a JSON array.
//
// @param [in] in       The ReadSource to read from.
// @param [in] sample_count The number of samples to read.
// @param [out] samples The array to read the samples into.
void read_snd_samples(ReadSource in, uint32_t sample_count, vector<uint8_t>* samples) {
    SFZ_FOREACH(uint32_t i, range(sample_count), {
        uint8_t sample;
        read(in, sample);
        samples->push_back(sample);
    });
}

}  // namespace

Sound::Sound(BytesSlice in) {
    BytesSlice header(in);
    read(header, fmt);
    if (fmt == 1) {
        read_snd_format_1_header(header);
    } else if (fmt == 2) {
        read_snd_format_2_header(header);
    } else {
        throw Exception(format("unknown 'snd ' format '{0}'", fmt));
    }

    uint16_t command_count;
    read(header, command_count);
    if (command_count != 1) {
        throw Exception(format("can only handle 1 command; {0} found", command_count));
    }

    uint16_t command;
    uint16_t zero;
    uint32_t offset;
    read_snd_command(header, command, zero, offset);
    if (command != 0x8051) {
        throw Exception(format("can only handle bufferCmd; 0x{0} found", hex(command, 4)));
    }
    if (zero != 0) {
        throw Exception(format("param1 must be zero; {0} found", zero));
    }

    BytesSlice sound(in.slice(offset));
    uint32_t pointer;
    uint32_t sample_count;
    read_snd_data_table(sound, pointer, sample_count, sample_rate);
    channels = 1;
    sample_bits = 8;

    BytesSlice sample(in.slice(offset + 22 + pointer, sample_count));
    read_snd_samples(sample, sample_count, &samples);
}

namespace {

// Write `d` as an IEEE 754 80-bit floating point (extended precision) number.
//
// Not verified for special values (positive and negative zero and infinity, NaN) correctly,
// because those aren't valid sample rates, and that's the reason this function was written.
//
// @param [out] out     The WriteTarget to write the value to.
// @param [in] d        A double-precision floating point value.
void write_float80(WriteTarget out, double d) {
    // Reinterpret `d` as uint64_t, since we will be manupulating bits.
    uint64_t sample_rate_bits = *reinterpret_cast<uint64_t*>(&d);

    // Deconstruct the double:
    //   * the sign, 1 bit.
    //   * the exponent, 11 bits with a bias of 1023.
    //   * the fraction, 52 bits, with an implicit '1' bit preceding.
    uint64_t sign = sample_rate_bits >> 63;
    uint64_t exponent = ((sample_rate_bits >> 52) & ((1 << 11) - 1)) - 1023;
    uint64_t fraction = (1ull << 52) | (sample_rate_bits & ((1ull << 52) - 1));

    // Reconstruct the 80-bit float:
    //   * the sign, 1 bit.
    //   * the exponent, 15 bits with a bias of 16383.
    //   * the fraction, 64 bits, with no implicit '1' bit.
    write<uint16_t>(out, (sign << 15) | ((exponent + 16383) & 0x7FFF));
    write<uint64_t>(out, fraction << 11);
}

// Write an IFF chunk.
//
// @param [out] out     The WriteTarget to write the chunk to.
// @param [in] name     A four-byte chunk name.
// @param [in] data     The content of the chunk.
void write_chunk(WriteTarget out, const char* name, const BytesSlice& data) {
    write(out, name, 4);
    write<uint32_t>(out, data.size());
    write(out, data.data(), data.size());
}

// Write an AIFF "COMM" chunk.
//
// The "COMM" (common) chunk specifies how to interpret samples from the "SSND" chunk.
//
// @param [out] out     The WriteTarget to write the chunk to.
// @param [in] info     Contains the information to write.
void write_comm(WriteTarget out, const Sound& sound) {
    Bytes comm;
    write<int16_t>(comm, sound.channels);
    write<uint32_t>(comm, sound.samples.size());
    write<int16_t>(comm, sound.sample_bits);
    write_float80(comm, sound.sample_rate);

    write_chunk(out, "COMM", comm);
}

// Write an AIFF "SSND" chunk.
//
// The "SSND" (sampled sound) chunk is an array of samples from the sound.
//
// @param [out] out     The WriteTarget to write the chunk to.
// @param [in] sound    Contains the samples to write.
void write_ssnd(WriteTarget out, const Sound& sound) {
    Bytes ssnd;
    write<uint32_t>(ssnd, 0);
    write<uint32_t>(ssnd, 0);
    SFZ_FOREACH(uint8_t sample, sound.samples, {
        write<int8_t>(ssnd, sample - 0x80);
    });

    write_chunk(out, "SSND", ssnd);
}

// Write an AIFF file.
//
// IFF files (including AIFF files) are structured as a hierarchy of chunks.  Given `sound`, which
// contains sound data, this function writes the "FORM" chunk which is at the top level of every
// IFF file, which consists of the identifier "AIFF", followed by the two chunks, "COMM" and
// "SSND", which define the content of the sound.
//
// @param [out] out     The WriteTarget to write the sound data to.
// @param [in] sound    Contains the sound to write.
void write_form(WriteTarget out, const Sound& sound) {
    Bytes form;
    write(form, "AIFF", 4);
    write_comm(form, sound);
    write_ssnd(form, sound);

    write_chunk(out, "FORM", form);
}

}  // namespace

AiffSound aiff(const Sound& sound) {
    AiffSound aiff = {sound};
    return aiff;
}

void write_to(WriteTarget out, AiffSound aiff) {
    write_form(out, aiff.sound);
}

}  // namespace rezin
