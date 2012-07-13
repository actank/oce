{
  'variables': {
    'BOOST_PATH':'/usr/local/xce',
  },
 'conditions':[
    ['OS=="linux"', {
     'target_defaults': {
       'cflags':['-fPIC', '-g', '-O2'],
       'defines':['OS_LINUX', 'POSIX', 'NEWARCH'],
       'include_dirs':[
         '/usr/local/xce-dev/include', # for ctemplate
         '..',
         ],
        },
      },
    ],
    ['OS=="win"', {'target_defaults': {},},],
  ],
  'targets':[
    {
      'target_name':'cwf',
      'type':'<(library)',
      'dependencies':[
        'fcgi.gyp:fcgi',
        'base.gyp:base',
      ],
      'direct_dependent_settings': {
        'target_conditions': [
          ['_type=="executable"', {
            'conditions':[
              ['OS=="linux"',{
                'libraries': [
                  '-L/usr/local/xce-dev/lib64/', '-lctemplate',
                ], 
                'ldflags':[
                    '-Wl,--whole-archive out/Default/obj.target/libcwf.a -Wl,--no-whole-archive',
                ],
              }],
            ],
          }],
        ],
      },
      'export_dependent_settings': [
        'base.gyp:base',
      ],
      'sources': [
        '../cwf/connect.cc',
        '../cwf/cookie.cc',
        '../cwf/frame.cc',
        '../cwf/http.cc',
        '../cwf/stream.cc',
        '../cwf/tplaction.cc',
      ],
    },
    {
      'target_name':'cwfmain',
      'type':'<(library)',
      'include_dirs': [
        '/opt/google-breakpad/src',
      ],
      'dependencies':[
        'fcgi.gyp:fcgi',
        'cwf',
        'base.gyp:base',
      ],
      'direct_dependent_settings': {
        'target_conditions': [
          ['_type=="executable"', {
            'conditions':[
              ['OS=="linux"',{
                'libraries': [
                  '-lbreakpad',
                ], 
              }],
            ],
          }],
        ],
      },
      'sources': [
        '../cwf/cwfmain.cc',
      ],
    },
    {
      'target_name':'cwfd',
      'type':'executable',
      'include_dirs': [
      ],
      'dependencies':[
        'fcgi.gyp:fcgi',
        'cwf',
        'cwfmain',
        'base.gyp:base',
      ],
      'conditions':[
        ['OS=="linux"', {
          'link_settings': {
            'ldflags': [
              '-Wl,-rpath <(BOOST_PATH)/lib',
              '-Wl,-rpath /usr/local/xce/lib/',
            ],
          },
        }],
      ],
      'sources': [
        '../cwf/guideaction.cc',
      ],
    }
  ],
}
