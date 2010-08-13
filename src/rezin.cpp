// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <getopt.h>
#include <exception>
#include <vector>
#include <rezin/Command.hpp>
#include <rezin/Options.hpp>
#include <rezin/ResourceFork.hpp>
#include <rezin/Source.hpp>
#include <rezin/commands/CatCommand.hpp>
#include <rezin/commands/ConvertCommand.hpp>
#include <rezin/commands/LsCommand.hpp>
#include <rezin/sources/AppleSingleSource.hpp>
#include <rezin/sources/FlatFileSource.hpp>
#include <rezin/sources/ZipSource.hpp>

using sfz::Exception;
using sfz::String;
using sfz::StringPiece;
using sfz::format;
using sfz::path::basename;
using sfz::scoped_ptr;
using std::vector;

namespace utf8 = sfz::utf8;
namespace io = sfz::io;

namespace rezin {
namespace {

int main(int argc, char** argv) {
    const String binary_name(utf8::decode(argv[0]));
    scoped_ptr<Command> command;
    scoped_ptr<Source> source;
    Options options;

    try {
        opterr = 0;
        const option longopts[] = {
            { "apple-single",   required_argument,  NULL,   'a' },
            { "flat-file",      required_argument,  NULL,   'f' },
            { "zip-file",       required_argument,  NULL,   'z' },
            { "line-ending",    required_argument,  NULL,   'l' },
            { NULL,             0,                  NULL,   0 }
        };

        while (true) {
            const char ch = getopt_long(argc, argv, "a:f:z:l:", longopts, NULL);
            if (ch == -1) {
                break;
            }

            String arg;
            if (optarg) {
                arg.assign(utf8::decode(optarg));
            }
            switch (ch) {
              case 'a':
                if (source.get() != NULL) {
                    throw Exception(format("more than one source specified."));
                }
                source.reset(new AppleSingleSource(arg));
                break;

              case 'f':
                if (source.get() != NULL) {
                    throw Exception(format("more than one source specified."));
                }
                source.reset(new FlatFileSource(arg));
                break;

              case 'z':
                if (source.get() != NULL) {
                    throw Exception(format("more than one source specified."));
                }
                source.reset(new ZipSource(arg));
                break;

              case 'l':
                if (arg == "cr") {
                    options.set_line_ending(Options::CR);
                } else if (arg == "nl") {
                    options.set_line_ending(Options::NL);
                } else if (arg == "crnl") {
                    options.set_line_ending(Options::CRNL);
                } else {
                    throw Exception(format("unrecognized line ending {0}.", quote(arg)));
                }
                break;

              default:
                {
                    String opt;
                    if (optopt == '\0') {
                        opt.assign(utf8::decode(argv[optind - 1]));
                    } else {
                        opt.assign(format("-{0}", char(optopt)));
                    }
                    throw Exception(format("unrecognized option {0}.", quote(opt)));
                }
                break;
            }
        }

        argc -= optind;
        argv += optind;
        if (argc < 1) {
            throw Exception(format("missing command."));
        }
        if (source.get() == NULL) {
            throw Exception(format("must provide a source."));
        }

        String storage;
        vector<StringPiece> args;
        for (int i = 0; i < argc; ++i) {
            String arg(utf8::decode(argv[i]));
            storage.append(arg);
            args.push_back(StringPiece(storage).substr(storage.size() - arg.size()));
        }

        if (args[0] == "ls") {
            command.reset(new LsCommand(args));
        } else if (args[0] == "cat") {
            command.reset(new CatCommand(args));
        } else if (args[0] == "convert") {
            command.reset(new ConvertCommand(args, options));
        } else {
            throw Exception(format("unknown command '{0}'.", args[0]));
        }
    } catch (Exception& e) {
        print(io::err, format(
                    "{0}: {1}\n"
                    "usage: {0} [OPTIONS] ls [TYPE [ID]]\n"
                    "       {0} [OPTIONS] cat TYPE ID\n"
                    "       {0} [OPTIONS] convert TYPE ID\n"
                    "options:\n"
                    "    -a|--apple-single=FILE\n"
                    "    -f|--flat-file=FILE\n"
                    "    -z|--zip-file=ZIP,FILE\n"
                    "    -l|--line-ending=cr|nl|crnl\n",
                    basename(binary_name), e.message()));
        return 1;
    }

    try {
        ResourceFork rsrc(source->load(), options);
        command->run(rsrc);
    } catch (Exception& e) {
        print(io::err, format("{0}: {1}\n", binary_name, e.message()));
        return 1;
    }

    return 0;
}

}  // namespace
}  // namespace rezin

int main(int argc, char** argv) {
    return rezin::main(argc, argv);
}
