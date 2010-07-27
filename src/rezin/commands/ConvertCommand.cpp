// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/commands/ConvertCommand.hpp>

#include <fcntl.h>
#include <rezin/rezin.hpp>
#include <rgos/rgos.hpp>

using rgos::Json;
using sfz::Bytes;
using sfz::BytesPiece;
using sfz::Exception;
using sfz::String;
using sfz::StringPiece;
using sfz::format;
using sfz::string_to_int;
using std::vector;

namespace io = sfz::io;
namespace utf8 = sfz::utf8;

namespace rezin {

ConvertCommand::ConvertCommand(const vector<StringPiece>& args, const Options& options)
        : _options(options) {
    if (args.size() != 3) {
        throw Exception(format("wrong number of arguments to command \"convert\"."));
    }
    _code.assign(args[1]);
    if (!string_to_int(args[2], &_id)) {
        throw Exception(format("invalid resource ID {0}.", quote(args[2])));
    }
}

void ConvertCommand::run(const ResourceFork& rsrc) {
    const ResourceEntry& entry = rsrc.at(_code).at(_id);
    BytesPiece data = entry.data();
    Bytes converted;

    if (_code == "snd ") {
        Json snd = read_snd(data);
        write_aiff(&converted, snd);
    } else if (_code == "TEXT") {
        String string(_options.decode(data));
        converted.assign(utf8::encode(string));
    } else if (_code == "STR#") {
        Json list = read_string_list(data, _options);
        String decoded_string;
        print_to(&decoded_string, pretty_print(list));
        converted.assign(utf8::encode(decoded_string));
    } else if (_code == "cicn") {
        Json cicn = read_cicn(data);
        write_png(&converted, cicn);
    } else if (_code == "clut") {
        Json list = read_clut(data);
        String decoded_string;
        print_to(&decoded_string, pretty_print(list));
        converted.assign(utf8::encode(decoded_string));
    } else {
        print(io::err, format("warning: printing unknown resource type {0} as raw data.\n",
                    quote(_code)));
        converted.assign(data.data(), data.size());
    }
    write(1, converted.data(), converted.size());
}

}  // namespace rezin
