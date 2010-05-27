// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/Sound.hpp"

#include "rgos/rgos.hpp"
#include "sfz/sfz.hpp"

using rgos::Json;
using rgos::JsonDefaultVisitor;
using sfz::Bytes;
using sfz::BytesPiece;
using sfz::Exception;
using sfz::ReadSource;
using sfz::StringKey;
using sfz::StringPiece;
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
    read(in, &synthesizer_count);
    if (synthesizer_count != 1) {
        throw Exception(format("can only handle 1 synthesizer; {0} found", synthesizer_count));
    }

    uint16_t type;
    read(in, &type);
    if (type != 5) {
        throw Exception(format("can only handle sampledSynth; {0} found", type));
    }

    uint32_t options;
    read(in, &options);
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
void read_snd_command(ReadSource in, uint16_t* command, uint16_t* param1, uint32_t* param2) {
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
void read_snd_data_table(ReadSource in, uint32_t* pointer, uint32_t* size, double* rate) {
    uint32_t fixed_sample_rate;
    uint32_t loop_start;
    uint32_t loop_end;
    uint8_t encoding;
    uint8_t base_frequency;

    read(in, pointer);
    read(in, size);
    read(in, &fixed_sample_rate);
    *rate = fixed_sample_rate / 65536.0;
    read(in, &loop_start);
    read(in, &loop_end);
    read(in, &encoding);
    read(in, &base_frequency);
}

// Reads the samples for a sound into a JSON array.
//
// @param [in] in       The ReadSource to read from.
// @param [in] sample_count The number of samples to read.
// @param [out] samples The array to read the samples into.
void read_snd_samples(ReadSource in, int sample_count, vector<Json>* samples) {
    foreach (i, range(sample_count)) {
        uint8_t sample;
        read(in, &sample);
        samples->push_back(Json::number(sample));
    }
}

}  // namespace

Json read_snd(const BytesPiece& in) {
    map<StringKey, Json> result;
    BytesPiece remainder(in);
    uint16_t fmt;
    read(&remainder, &fmt);
    if (fmt == 1) {
        read_snd_format_1_header(&remainder);
    } else if (fmt == 2) {
        read_snd_format_2_header(&remainder);
    } else {
        throw Exception(format("unknown 'snd ' format '{0}'", fmt));
    }
    result.insert(make_pair("format", Json::number(fmt)));

    uint16_t command_count;
    read(&remainder, &command_count);
    if (command_count != 1) {
        throw Exception(format("can only handle 1 command; {0} found", command_count));
    }

    uint16_t command;
    uint16_t zero;
    uint32_t offset;
    read_snd_command(&remainder, &command, &zero, &offset);
    if (command != 0x8051) {
        throw Exception(format("can only handle bufferCmd; 0x{0} found", hex(command, 4)));
    }
    if (zero != 0) {
        throw Exception(format("param1 must be zero; {0} found", zero));
    }

    BytesPiece sound(in.substr(offset));
    uint32_t pointer;
    uint32_t sample_count;
    double sample_rate;
    read_snd_data_table(&sound, &pointer, &sample_count, &sample_rate);
    result.insert(make_pair("channels", Json::number(1)));
    result.insert(make_pair("sample_bits", Json::number(8)));
    result.insert(make_pair("sample_rate", Json::number(sample_rate)));

    vector<Json> samples;
    BytesPiece sample(in.substr(offset + 22 + pointer, sample_count));
    read_snd_samples(&sample, sample_count, &samples);
    result.insert(make_pair("samples", Json::array(samples)));
    return Json::object(result);
}

namespace {

// A JsonVisitor which gets the numeric value of a visited value.
//
// If it visits a numeric value, sets the provided value accordingly; otherwise, throws.
template <typename T>
class GetNumberVisitor : public JsonDefaultVisitor {
  public:
    GetNumberVisitor(T* out)
            : _out(out) { }

    void visit_number(double value) { *_out = value; }

  private:
    virtual void visit_default(const char* type) {
        throw Exception(format("need number, found {0}", type));
    }

    T* _out;
};

// A JsonVisitor which gets an array of uint8_t objects.
//
// If it visits an array of numbers, sets the provided value accordingly; otherwise, throws.
class GetSamplesVisitor : public JsonDefaultVisitor {
  public:
    GetSamplesVisitor(vector<uint8_t>* out)
            : _out(out) { }

    virtual void visit_array(const vector<Json>& value) {
        foreach (it, value) {
            _out->push_back(0);
            GetNumberVisitor<uint8_t> visitor(&_out->back());
            it->accept(&visitor);
        }
    }

  public:
    virtual void visit_default(const char* type) {
        throw Exception(format("need array, found {0}", type));
    }

    vector<uint8_t>* _out;
};

// Sound data extracted from a JSON sound object.
struct SoundInfo {
    int channels;
    int sample_bits;
    double sample_rate;
    vector<uint8_t> samples;
};

// A JsonVisitor which converts a JSON sound object into a SoundInfo object.
//
// If it visits an object it can interpret, sets the provided value accordingly; otherwise, throws.
class SoundInfoVisitor : public JsonDefaultVisitor {
  public:
    SoundInfoVisitor(SoundInfo* out)
            : _out(out) { }

    void visit_object(const map<StringKey, Json>& value) {
        GetNumberVisitor<int> get_channels(&_out->channels);
        GetNumberVisitor<int> get_sample_bits(&_out->sample_bits);
        GetNumberVisitor<double> get_sample_rate(&_out->sample_rate);
        GetSamplesVisitor get_samples(&_out->samples);
        checked_get(value, "channels").accept(&get_channels);
        checked_get(value, "sample_bits").accept(&get_sample_bits);
        checked_get(value, "sample_rate").accept(&get_sample_rate);
        checked_get(value, "samples").accept(&get_samples);
    }

  private:
    const Json& checked_get(const map<StringKey, Json>& value, const char* key) {
        StringKey string_key(key);
        map<StringKey, Json>::const_iterator it = value.find(string_key);
        if (it == value.end()) {
            throw Exception(format("missing key '{0}'", key));
        }
        return it->second;
    }

    virtual void visit_default(const char* type) {
        throw Exception(format("need object, found {0}", type));
    }

    SoundInfo* _out;
};

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
void write_chunk(WriteTarget out, const char* name, const BytesPiece& data) {
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
void write_comm(WriteTarget out, const SoundInfo& info) {
    Bytes comm;
    write<int16_t>(&comm, info.channels);
    write<uint32_t>(&comm, info.samples.size());
    write<int16_t>(&comm, info.sample_bits);
    write_float80(&comm, info.sample_rate);

    write_chunk(out, "COMM", comm);
}

// Write an AIFF "SSND" chunk.
//
// The "SSND" (sampled sound) chunk is an array of samples from the sound.
//
// @param [out] out     The WriteTarget to write the chunk to.
// @param [in] info     Contains the samples to write.
void write_ssnd(WriteTarget out, const SoundInfo& info) {
    Bytes ssnd;
    write<uint32_t>(&ssnd, 0);
    write<uint32_t>(&ssnd, 0);
    foreach (it, info.samples) {
        write<int8_t>(&ssnd, *it - 0x80);
    }

    write_chunk(out, "SSND", ssnd);
}

// Write an AIFF file.
//
// IFF files (including AIFF files) are structured as a hierarchy of chunks.  Given `info`, which
// contains sound data, this function writes the "FORM" chunk which is at the top level of every
// IFF file, which consists of the identifier "AIFF", followed by the two chunks, "COMM" and
// "SSND", which define the content of the sound.
//
// @param [out] out     The WriteTarget to write the sound data to.
// @param [in] info     Contains the sound to write.
void write_form(WriteTarget out, const SoundInfo& info) {
    Bytes form;
    write(&form, "AIFF", 4);
    write_comm(&form, info);
    write_ssnd(&form, info);

    write_chunk(out, "FORM", form);
}

}  // namespace

void write_aiff(const Json& sound, Bytes* out) {
    SoundInfo info;
    SoundInfoVisitor visitor(&info);
    sound.accept(&visitor);

    write_form(out, info);
}

}  // namespace rezin
