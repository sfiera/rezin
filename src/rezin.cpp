// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <fcntl.h>
#include <getopt.h>
#include <exception>
#include <map>
#include <vector>
#include <rgos/rgos.hpp>
#include <sfz/sfz.hpp>
#include <zipxx/zipxx.hpp>

#include "rezin/rezin.hpp"

using rgos::Json;
using sfz::Bytes;
using sfz::BytesPiece;
using sfz::Exception;
using sfz::MappedFile;
using sfz::Rune;
using sfz::String;
using sfz::StringPiece;
using sfz::format;
using sfz::path::basename;
using sfz::quote;
using sfz::range;
using sfz::scoped_ptr;
using sfz::string_to_int;
using std::map;
using std::vector;
using zipxx::ZipArchive;
using zipxx::ZipFileReader;

namespace io = sfz::io;
namespace utf8 = sfz::utf8;

namespace rezin {
namespace {

class Command {
  public:
    virtual ~Command() { }
    virtual void run(const ResourceFork& rsrc) = 0;
};

class LsCommand : public Command {
  public:
    LsCommand(const vector<StringPiece>& args)
        : _argc(args.size()) {
        if (args.size() > 3) {
            throw Exception(format("too many arguments to command \"ls\"."));
        }
        if (args.size() > 2) {
            if (!string_to_int(args[2], &_id)) {
                throw Exception(format("invalid resource ID {0}.", quote(args[2])));
            }
        }
        if (args.size() > 1) {
            _code.assign(args[1]);
        }
    }

    virtual void run(const ResourceFork& rsrc) {
        if (_argc == 1) {
            foreach (it, rsrc) {
                print(io::out, format("{0}\n", it->code()));
            }
        } else if (_argc == 2) {
            const ResourceType& type = rsrc.at(_code);
            foreach (it, type) {
                print(io::out, format("{0}\t{1}\n", it->id(), it->name()));
            }
        } else if (_argc == 3) {
            const ResourceEntry& entry = rsrc.at(_code).at(_id);
            print(io::out, format("{0}\t{1}\n", entry.id(), entry.name()));
        }
    }

  private:
    int _argc;
    String _code;
    int16_t _id;
};

class CatCommand : public Command {
  public:
    CatCommand(const vector<StringPiece>& args) {
        if (args.size() != 3) {
            throw Exception(format("wrong number of arguments to command \"cat\"."));
        }
        _code.assign(args[1]);
        if (!string_to_int(args[2], &_id)) {
            throw Exception(format("invalid resource ID {0}.", quote(args[2])));
        }
    }

    virtual void run(const ResourceFork& rsrc) {
        const ResourceEntry& entry = rsrc.at(_code).at(_id);
        BytesPiece data = entry.data();
        write(1, data.data(), data.size());
    }

  private:
    String _code;
    int16_t _id;
};

class ConvertCommand : public Command {
  public:
    ConvertCommand(const vector<StringPiece>& args, const Options& options)
            : _options(options) {
        if (args.size() != 3) {
            throw Exception(format("wrong number of arguments to command \"convert\"."));
        }
        _code.assign(args[1]);
        if (!string_to_int(args[2], &_id)) {
            throw Exception(format("invalid resource ID {0}.", quote(args[2])));
        }
    }

    virtual void run(const ResourceFork& rsrc) {
        const ResourceEntry& entry = rsrc.at(_code).at(_id);
        BytesPiece data = entry.data();
        Bytes converted;

        if (_code == "snd ") {
            Json snd = read_snd(data);
            write_aiff(snd, &converted);
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

  private:
    String _code;
    int16_t _id;
    Options _options;
};

class Source {
  public:
    virtual ~Source() { }
    virtual BytesPiece load() = 0;
};

class ResourceForkSource : public Source {
  public:
    ResourceForkSource(const StringPiece& arg)
        : _path(arg) { }

    virtual BytesPiece load() {
        _file.reset(new MappedFile(_path));
        return _file->data();
    }

  private:
    const String _path;
    scoped_ptr<MappedFile> _file;
};

class AppleSingleSource : public Source {
  public:
    AppleSingleSource(const StringPiece& arg)
        : _path(arg) { }

    virtual BytesPiece load() {
        _file.reset(new MappedFile(_path));
        _apple_single.reset(new AppleSingle(_file->data()));
        return _apple_single->at(AppleSingle::RESOURCE_FORK);
    }

  private:
    const String _path;
    scoped_ptr<MappedFile> _file;
    scoped_ptr<AppleSingle> _apple_single;
};

class ZipSource : public Source {
  public:
    ZipSource(const StringPiece& arg) {
        String arg_string(arg);
        size_t comma = arg_string.find(',');
        if (comma == String::npos) {
            throw Exception(format("invalid format to --zip-file {0}.", quote(arg_string)));
        }
        _archive_path.assign(StringPiece(arg_string).substr(0, comma));

        _file_path.append("__MACOSX/");
        _file_path.append(StringPiece(arg_string).substr(comma + 1));
        size_t loc = _file_path.rfind('/');
        _file_path.replace(loc, 1, "/._");
    }

    virtual BytesPiece load() {
        _archive.reset(new ZipArchive(_archive_path, 0));
        _file.reset(new ZipFileReader(_archive.get(), _file_path));
        _apple_double.reset(new AppleDouble(_file->data()));
        return _apple_double->at(AppleDouble::RESOURCE_FORK);
    }

  private:
    String _archive_path;
    String _file_path;
    scoped_ptr<ZipArchive> _archive;
    scoped_ptr<ZipFileReader> _file;
    scoped_ptr<AppleDouble> _apple_double;
};

int main(int argc, char** argv) {
    const String binary_name(utf8::decode(argv[0]));
    scoped_ptr<Command> command;
    scoped_ptr<Source> source;
    Options options;

    try {
        opterr = 0;
        const option longopts[] = {
            { "apple-single",   required_argument,  NULL,   'a' },
            { "resource-fork",  required_argument,  NULL,   'r' },
            { "zip-file",       required_argument,  NULL,   'z' },
            { "line-ending",    required_argument,  NULL,   'l' },
            { NULL,             0,                  NULL,   0 }
        };

        while (true) {
            const char ch = getopt_long(argc, argv, "a:r:z:l:", longopts, NULL);
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

              case 'r':
                if (source.get() != NULL) {
                    throw Exception(format("more than one source specified."));
                }
                source.reset(new ResourceForkSource(arg));
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
                    "    -r|--resource-fork=FILE\n"
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
