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
            'src/char_ref.c',
            'src/error.c',
            'src/parser.c',
            'src/string_buffer.c',
            'src/string_piece.c',
            'src/tag.c',
            'src/tokenizer.c',
            'src/utf8.c',
            'src/util.c',
            'src/vector.c',
       ],
       'conditions': [
           ['OS=="linux"', {
             'cflags': ['-std=gnu99']
           }, {}]
       ],
    },


  ]
}
