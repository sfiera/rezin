{
    'targets': [
        {
            'target_name': 'librezin',
            'type': '<(library)',
            'sources': [
                'src/rezin/AppleSingle.cpp',
                'src/rezin/BasicTypes.cpp',
                'src/rezin/BitsPiece.cpp',
                'src/rezin/ColorIcon.cpp',
                'src/rezin/ColorTable.cpp',
                'src/rezin/ColorTableInternal.cpp',
                'src/rezin/Options.cpp',
                'src/rezin/ResourceEntry.cpp',
                'src/rezin/ResourceFork.cpp',
                'src/rezin/ResourceType.cpp',
                'src/rezin/Sound.cpp',
                'src/rezin/StringList.cpp',
            ],
            'dependencies': [
                '<(DEPTH)/ext/libsfz/libsfz.gyp:libsfz',
                '<(DEPTH)/ext/librgos/librgos.gyp:librgos',
                '<(DEPTH)/ext/libpng/libpng.gyp:libpng',
            ],
            'include_dirs': [
                'public',
                'private',
            ],
            'direct_dependent_settings': {
                'include_dirs': [
                    'public',
                ],
            },
            'export_dependent_settings': [
                '<(DEPTH)/ext/libsfz/libsfz.gyp:libsfz',
                '<(DEPTH)/ext/librgos/librgos.gyp:librgos',
                '<(DEPTH)/ext/libpng/libpng.gyp:libpng',
            ],
        },
        {
            'target_name': 'rezin',
            'type': 'executable',
            'sources': [
                'src/rezin.cpp',
                'src/rezin/commands/CatCommand.cpp',
                'src/rezin/commands/ConvertCommand.cpp',
                'src/rezin/commands/LsCommand.cpp',
                'src/rezin/sources/AppleSingleSource.cpp',
                'src/rezin/sources/FlatFileSource.cpp',
                'src/rezin/sources/ZipSource.cpp',
            ],
            'dependencies': [
                ':librezin',
                '<(DEPTH)/ext/libsfz/libsfz.gyp:libsfz',
                '<(DEPTH)/ext/librgos/librgos.gyp:librgos',
                '<(DEPTH)/ext/libzipxx/libzipxx.gyp:libzipxx',
            ],
            'include_dirs': [
                'public',
                'private',
            ],
        },
    ],
}
