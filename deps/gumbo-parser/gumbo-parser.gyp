{
  'variables': { 'target_arch%': 'ia32' },
  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 1, # static debug
          },
        },
      },
      'Release': {
        'defines': [ 'NDEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 0, # static release
          },
        },
      }
    },
    'msvs_settings': {
      'VCLinkerTool': {
        'GenerateDebugInformation': 'true',
      },
    },
  },
  'include_dirs': [
    # platform and arch-specific headers
    'config/<(OS)/<(target_arch)'
  ],
  'targets': [
     {
       'target_name': 'gumbo',
       'product_prefix': 'lib',
       'type': 'static_library',
       'sources': [
            'src/attribute.c',
            'src/attribute.h',
            'src/char_ref.c',
            'src/char_ref.h',
            'src/error.c',
            'src/error.h',
            'src/gumbo.h',
            'src/insertion_mode.h',
            'src/parser.c',
            'src/parser.h',
            'src/string_buffer.c',
            'src/string_buffer.h',
            'src/string_piece.c',
            'src/string_piece.h',
            'src/tag.c',
            'src/token_type.h',
            'src/tokenizer.c',
            'src/tokenizer.h',
            'src/tokenizer_states.h',
            'src/utf8.c',
            'src/utf8.h',
            'src/util.c',
            'src/util.h',
            'src/vector.c',
            'src/vector.h',
        ],
       'conditions': [
           ['OS=="linux"', {
             'cflags': ['-std=gnu99']
           }, {}]
       ],
    },


  ]
}
