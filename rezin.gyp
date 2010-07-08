{
    'target_defaults': {
        'include_dirs': [
            'include',
        ],
    },
    'targets': [
        {
            'target_name': 'librezin',
            'type': '<(library)',
            'sources': [
                'src/rezin/AppleSingle.cpp',
                'src/rezin/Cr2nl.cpp',
                'src/rezin/ResourceEntry.cpp',
                'src/rezin/ResourceFork.cpp',
                'src/rezin/ResourceType.cpp',
                'src/rezin/Sound.cpp',
                'src/rezin/StringList.cpp',
            ],
            'dependencies': [
                ':check-deps',
                '<(DEPTH)/ext/libsfz/libsfz.gyp:libsfz',
                '<(DEPTH)/ext/librgos/librgos.gyp:librgos',
            ],
            'direct_dependent_settings': {
                'include_dirs': [
                    'include',
                ],
            },
            'export_dependent_settings': [
                '<(DEPTH)/ext/libsfz/libsfz.gyp:libsfz',
                '<(DEPTH)/ext/librgos/librgos.gyp:librgos',
            ],
        },
        {
            'target_name': 'rezin',
            'type': 'executable',
            'sources': [
                'src/rezin.cpp',
            ],
            'dependencies': [
                ':check-deps',
                ':librezin',
                '<(DEPTH)/ext/libsfz/libsfz.gyp:libsfz',
                '<(DEPTH)/ext/librgos/librgos.gyp:librgos',
                '<(DEPTH)/ext/libzipxx/libzipxx.gyp:libzipxx',
            ],
        },
    ],
}
