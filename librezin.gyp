{
    'target_defaults': {
        'include_dirs': [
            'include',
            '../libsfz/include',
            '../librgos/include',
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
            'target_name': 'librezin',
            'type': '<(library)',
            'sources': [
                'src/rezin/AppleSingle.cpp',
                'src/rezin/MacRoman.cpp',
                'src/rezin/ResourceEntry.cpp',
                'src/rezin/ResourceFork.cpp',
                'src/rezin/ResourceType.cpp',
                'src/rezin/Sound.cpp',
                'src/rezin/StringList.cpp',
            ],
            'dependencies': [
                '../libsfz/libsfz.gyp:libsfz',
                '../librgos/librgos.gyp:librgos',
            ],
        },
    ],
}
