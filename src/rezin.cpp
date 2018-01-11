// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <getopt.h>
#include <exception>
#include <rezin/commands/cat.hpp>
#include <rezin/commands/convert.hpp>
#include <rezin/commands/ls.hpp>
#include <rezin/options.hpp>
#include <rezin/resource.hpp>
#include <rezin/sources/apple-single.hpp>
#include <rezin/sources/file.hpp>
#include <rezin/sources/zip.hpp>
#include <vector>

using sfz::Exception;
using sfz::Optional;
using sfz::String;
using sfz::StringSlice;
using sfz::args::store;
using sfz::format;
using sfz::path::basename;
using sfz::print;
using std::vector;

namespace args = sfz::args;
namespace utf8 = sfz::utf8;
namespace io   = sfz::io;

namespace rezin {
namespace {

template <typename Source>
void RunCommand(const Source& source, const Options& options, const Command& command) {
    ResourceFork rsrc(source.data(), options);
    command.run(rsrc, options);
}

void main(int argc, char** argv) {
    args::Parser parser(
            argv[0], "rezin is a tool for extracting data from the resource fork of legacy files");

    Optional<AppleSingleSource> apple_single;
    parser.add_argument("-a", "--apple-single", store(apple_single))
            .metavar("FILE")
            .help("read from an AppleSingle- or AppleDouble-formatted file");

    Optional<FlatFileSource> flat_file;
    parser.add_argument("-f", "--flat-file", store(flat_file))
            .metavar("FILE")
            .help("read from a flat file");

    Optional<ZipSource> zip_file;
    parser.add_argument("-z", "--zip-file", store(zip_file))
            .metavar("ZIP,FILE")
            .help("read from a file enclosed in a zip archive");

    Options options;
    parser.add_argument("-l", "--line-ending", store(options.line_ending))
            .metavar("cr|nl|crnl")
            .help("convert carriage returns to this");

    Command*       command = NULL;
    LsCommand      ls_command(parser, command);
    CatCommand     cat_command(parser, command);
    ConvertCommand convert_command(parser, command);

    try {
        String error;
        if (!parser.parse_args(argc - 1, argv + 1, error)) {
            print(io::err, format("{0}: {1}\n", parser.name(), error));
            exit(1);
        }
        if (command == NULL) {
            print(io::err, parser.help());
            exit(1);
        }
        if (apple_single.has()) {
            RunCommand(*apple_single, options, *command);
        } else if (flat_file.has()) {
            RunCommand(*flat_file, options, *command);
        } else if (zip_file.has()) {
            RunCommand(*zip_file, options, *command);
        } else {
            print(io::err, format("{0}: no source specified\n", parser.name()));
            exit(1);
        }
    } catch (Exception& e) {
        print(io::err, format("{0}: {1}\n", parser.name(), e.message()));
        exit(1);
    }
}

}  // namespace
}  // namespace rezin

int main(int argc, char** argv) {
    rezin::main(argc, argv);
    return 0;
}
