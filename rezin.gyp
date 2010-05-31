{
    'target_defaults': {
        'include_dirs': [
            'include',
            '<(DEPTH)/ext/libsfz/include',
            '<(DEPTH)/ext/librgos/include',
            '<(DEPTH)/ext/libzipxx/include',
        ],
        'xcode_settings': {
            'GCC_TREAT_WARNINGS_AS_ERRORS': 'YES',
            'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
            'SDKROOT': 'macosx10.4',
            'GCC_VERSION': '4.0',
            'ARCHS': 'ppc x86_64 i386',
            'WARNING_CFLAGS': [
                '-Wall',
                '-Wendif-labels',
            ],
        },
    },
    'targets': [
        {
            'target_name': 'check-deps',
            'type': 'none',
            'actions': [
                {
                    'action_name': 'check-deps',
                    'inputs': [ ],
                    'outputs': [ ],
                    'action': [
                        './scripts/check-deps.sh',
                        '<(DEPTH)',
                    ],
                },
            ],
        },
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
