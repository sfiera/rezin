{ "target_defaults":
  { "cxxflags":
    [ "-Wall"
    , "-Werror"
    ]
  , "include_dirs": ["src"]
  }

, "targets":
  [ { "target_name": "librezin"
    , "product_prefix": ""
    , "type": "static_library"
    , "sources":
      [ "src/rezin/apple-single.cpp"
      , "src/rezin/bits-slice.cpp"
      , "src/rezin/cicn.cpp"
      , "src/rezin/clut.cpp"
      , "src/rezin/image.cpp"
      , "src/rezin/options.cpp"
      , "src/rezin/pict.cpp"
      , "src/rezin/png.cpp"
      , "src/rezin/primitives.cpp"
      , "src/rezin/resource.cpp"
      , "src/rezin/snd.cpp"
      , "src/rezin/strl.cpp"
      ]
    , "include_dirs": ["include"]
    , "dependencies":
      [ "<(DEPTH)/ext/libpng-gyp/libpng.gyp:libpng"
      , "<(DEPTH)/ext/libsfz/libsfz.gyp:libsfz"
      , "<(DEPTH)/ext/libzipxx/libzipxx.gyp:libzipxx"
      ]
    , "export_dependent_settings":
      [ "<(DEPTH)/ext/libpng-gyp/libpng.gyp:libpng"
      , "<(DEPTH)/ext/libsfz/libsfz.gyp:libsfz"
      , "<(DEPTH)/ext/libzipxx/libzipxx.gyp:libzipxx"
      ]
    , "direct_dependent_settings":
      { "include_dirs": ["include"]
      }
    }

  , { "target_name": "rezin"
    , "type": "executable"
    , "sources":
      [ "src/rezin.cpp"
      , "src/rezin/commands/cat.cpp"
      , "src/rezin/commands/convert.cpp"
      , "src/rezin/commands/ls.cpp"
      , "src/rezin/sources/apple-single.cpp"
      , "src/rezin/sources/file.cpp"
      , "src/rezin/sources/zip.cpp"
      ]
    , "dependencies": ["librezin"]
    }
  ]
}
# -*- mode: python; tab-width: 2 -*-
