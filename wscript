# -*- mode: python -*-

def common(ctx):
    ctx.load("compiler_cxx")
    ctx.load("externals", "ext/waf-sfiera")
    ctx.load("platforms", "ext/waf-sfiera")
    ctx.external([
        "libpng",
        "libsfz",
        "libzipxx",
    ])

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
            "src/rezin/ColorTableInternal.cpp",
            "src/rezin/Options.cpp",
            "src/rezin/ResourceEntry.cpp",
            "src/rezin/ResourceFork.cpp",
            "src/rezin/ResourceType.cpp",
            "src/rezin/Sound.cpp",
            "src/rezin/StringList.cpp",
        ],
        cxxflags="-Wall -Werror",
        arch="x86_64 i386 ppc",
        includes="./public ./private",
        export_includes="./public",
        use=[
            "libpng/libpng",
            "libsfz/libsfz",
            "libzipxx/libzipxx",
        ],
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
        arch="x86_64 i386 ppc",
        includes="./private",
        use="rezin/librezin",
    )
