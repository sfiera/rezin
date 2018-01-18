// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <getopt.h>
#include <exception>
#include <rezin/apple-single.hpp>
#include <rezin/commands/cat.hpp>
#include <rezin/commands/convert.hpp>
#include <rezin/commands/ls.hpp>
#include <rezin/options.hpp>
#include <rezin/resource.hpp>
#include <rezin/source.hpp>
#include <rezin/sources/apple-single.hpp>
#include <rezin/sources/file.hpp>
#include <rezin/sources/zip.hpp>
#include <vector>

namespace args = sfz::args;
namespace path = sfz::path;

namespace rezin {
namespace {

const char help[] =
        "usage: rezin [options] ls [type [id]]\n"
        "       rezin [options] cat type id\n"
        "       rezin [options] convert type id\n"
        "\n"
        "rezin is a tool for extracting data from the resource fork of legacy files.\n"
        "\n"
        "sources:\n"
        " -a, --apple-single=FILE     read from an AppleSingle/AppleDouble file\n"
        " -f, --flat-file=FILE        read from a flat file\n"
        " -z, --zip-file=ZIP,FILE     read from a file enclosed in a zip archive\n"
        "\n"
        "options:\n"
        " -l, --line-ending=CRNL      convert cr (\\r) to cr, nl, or crnl (default: nl)\n"
        "\n"
        "commands:\n"
        "     ls [type [id]]          list resource types or IDs\n"
        "     cat type id             print content of a resource as raw data\n"
        "     convert type id         print converted form of a resource if possible\n"
        "                             (if not, print the raw data with a warning)\n";

Options::LineEnding parse_line_ending(pn::string_view s) {
    if (s == "cr") {
        return Options::CR;
    } else if (s == "nl") {
        return Options::NL;
    } else if (s == "crnl") {
        return Options::CRNL;
    } else {
        throw std::runtime_error("must be one of cr|nl|crnl");
    }
}

void print_nested_exception(const std::exception& e) {
    pn::format(stderr, ": {0}", e.what());
    try {
        std::rethrow_if_nested(e);
    } catch (const std::exception& e) {
        print_nested_exception(e);
    }
}

void print_exception(pn::string_view progname, const std::exception& e) {
    pn::format(stderr, "{0}: {1}", progname, e.what());
    try {
        std::rethrow_if_nested(e);
    } catch (const std::exception& e) {
        print_nested_exception(e);
    }
    pn::format(stderr, "\n");
}

void main(int argc, char** argv) {
    std::unique_ptr<Command> command;
    std::unique_ptr<Source>  source;
    Options                  options;

    args::callbacks callbacks;

    callbacks.argument = [&command](pn::string_view arg) {
        if (command) {
            return command->argument(arg);
        } else if (arg == "convert") {
            command.reset(new ConvertCommand);
        } else if (arg == "cat") {
            command.reset(new CatCommand);
        } else if (arg == "ls") {
            command.reset(new LsCommand);
        } else {
            return false;
        }
        return true;
    };

    callbacks.short_option = [&source, &options](
                                     pn::rune opt, const args::callbacks::get_value_f& get_value) {
        switch (opt.value()) {
            case 'a': source.reset(new AppleSingleSource(get_value())); break;
            case 'f': source.reset(new FlatFileSource(get_value())); break;
            case 'z': source.reset(new ZipSource(get_value())); break;
            case 'l': options.line_ending = parse_line_ending(get_value()); break;
            default: return false;
        }
        return true;
    };

    callbacks.long_option =
            [&callbacks](pn::string_view opt, const args::callbacks::get_value_f& get_value) {
                if (opt == "--apple-single") {
                    return callbacks.short_option(pn::rune{'a'}, get_value);
                } else if (opt == "--flat-file") {
                    return callbacks.short_option(pn::rune{'f'}, get_value);
                } else if (opt == "--zip-file") {
                    return callbacks.short_option(pn::rune{'z'}, get_value);
                } else if (opt == "--line-ending") {
                    return callbacks.short_option(pn::rune{'l'}, get_value);
                } else {
                    return false;
                }
                return true;
            };

    try {
        pn::string error;
        args::parse(argc - 1, argv + 1, callbacks);
        if (!command) {
            pn::file_view{stderr}.write(help);
            exit(1);
        } else if (!source) {
            pn::format(stderr, "{0}: no source specified\n", argv[0]);
            exit(1);
        }
        source->load();
        ResourceFork fork(source->data(), options);
        command->run(fork, options);
    } catch (const std::exception& e) {
        print_exception(argv[0], e);
        exit(1);
    }
}

}  // namespace
}  // namespace rezin

int main(int argc, char** argv) {
    rezin::main(argc, argv);
    return 0;
}
