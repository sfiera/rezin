// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/commands/convert.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <rezin/rezin.hpp>

using sfz::Bytes;
using sfz::BytesSlice;
using sfz::Exception;
using sfz::Json;
using sfz::String;
using sfz::StringSlice;
using sfz::args::store;
using sfz::args::store_const;
using sfz::format;
using sfz::string_to_int;
using std::vector;

namespace args = sfz::args;
namespace io   = sfz::io;
namespace utf8 = sfz::utf8;

namespace rezin {

ConvertCommand::ConvertCommand(args::Parser& parser, Command*& command) {
    args::Parser& convert =
            parser.add_subparser("convert", "convert resource", store_const(command, this));
    convert.add_argument("type", store(_type)).required();
    convert.add_argument("id", store(_id)).required();
}

void ConvertCommand::run(const ResourceFork& rsrc, const Options& options) const {
    const ResourceEntry& entry = rsrc.at(_type).at(_id);
    BytesSlice           data  = entry.data();
    Bytes                converted;

    if (_type == "snd ") {
        Sound snd(data);
        write(converted, aiff(snd));
    } else if (_type == "TEXT") {
        String string(options.decode(data));
        converted.assign(utf8::encode(string));
    } else if (_type == "STR#") {
        StringList string_list(data, options);
        Json       list = json(string_list);
        String     decoded_string(pretty_print(list));
        converted.assign(utf8::encode(decoded_string));
    } else if (_type == "cicn") {
        ColorIcon cicn(data);
        write(converted, png(cicn));
    } else if (_type == "clut") {
        ColorTable clut(data);
        Json       list = json(clut);
        String     decoded_string(pretty_print(list));
        converted.assign(utf8::encode(decoded_string));
    } else if (_type == "PICT") {
        Picture pict(data);
        write(converted, png(pict));
    } else {
        print(io::err,
              format("warning: printing unknown resource type {0} as raw data.\n", quote(_type)));
        converted.assign(data.data(), data.size());
    }
    if (write(1, converted.data(), converted.size()) < 0) {
        // TODO(sfiera): handle result properly.
    }
}

}  // namespace rezin
