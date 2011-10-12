# -*- mode: python -*-

def common(ctx):
    ctx.load("compiler_cxx")
    ctx.load("core externals", "ext/waf-sfiera")
    ctx.external("libpng libsfz libzipxx")

def options(opt):
    common(opt)

def configure(cnf):
    common(cnf)

def build(bld):
    common(bld)

    bld.stlib(
        target="rezin/librezin",
        source=[
            "src/rezin/AppleSingle.cpp",
            "src/rezin/BasicTypes.cpp",
            "src/rezin/BitsSlice.cpp",
            "src/rezin/ColorIcon.cpp",
            "src/rezin/ColorTable.cpp",
            "src/rezin/Options.cpp",
            "src/rezin/ResourceEntry.cpp",
            "src/rezin/ResourceFork.cpp",
            "src/rezin/ResourceType.cpp",
            "src/rezin/Sound.cpp",
            "src/rezin/StringList.cpp",
        ],
        cxxflags="-Wall -Werror",
        includes="./include ./src",
        export_includes="./include",
        use=[
            "libpng/libpng",
            "libsfz/libsfz",
            "libzipxx/libzipxx",
        ],
    )

    bld.platform(
        target="rezin/librezin",
        platform="darwin",
        arch="x86_64 i386 ppc",
    )

    bld.program(
        target="rezin/rezin",
        source=[
            "src/rezin.cpp",
            "src/rezin/commands/CatCommand.cpp",
            "src/rezin/commands/ConvertCommand.cpp",
            "src/rezin/commands/LsCommand.cpp",
            "src/rezin/sources/AppleSingleSource.cpp",
            "src/rezin/sources/FlatFileSource.cpp",
            "src/rezin/sources/ZipSource.cpp",
        ],
        cxxflags="-Wall -Werror",
        includes="./src",
        use="rezin/librezin",
    )

    bld.platform(
        target="rezin/rezin",
        platform="darwin",
        arch="x86_64 i386 ppc",
    )
