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
            "src/rezin/apple-single.cpp",
            "src/rezin/bits-slice.cpp",
            "src/rezin/cicn.cpp",
            "src/rezin/clut.cpp",
            "src/rezin/options.cpp",
            "src/rezin/png.cpp",
            "src/rezin/primitives.cpp",
            "src/rezin/resource.cpp",
            "src/rezin/snd.cpp",
            "src/rezin/strl.cpp",
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
            "src/rezin/commands/cat.cpp",
            "src/rezin/commands/convert.cpp",
            "src/rezin/commands/ls.cpp",
            "src/rezin/sources/apple-single.cpp",
            "src/rezin/sources/file.cpp",
            "src/rezin/sources/zip.cpp",
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
