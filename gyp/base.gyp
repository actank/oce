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
         '<(BOOST_PATH)/include/boost-1_38',
         '..',
         ],
        },
      },
    ],
    ['OS=="win"', {'target_defaults': {},},],
  ],
  'targets':[
    {
      'target_name':'base',
      'type':'<(library)',
      'direct_dependent_settings': {
        'include_dirs': [
          '<(BOOST_PATH)/include/boost-1_38',
        ],
        'target_conditions': [
          ['_type=="executable"', {
            'conditions':[
              ['OS=="linux"',{
                'libraries': [
                  '-L<(BOOST_PATH)/lib',
                  '-lboost_regex-gcc41-mt', 
                  '-lboost_date_time-gcc41-mt', 
                  '-lboost_thread-gcc41-mt', 
                  '-lboost_system-gcc41-mt', 
                  '-lboost_filesystem-gcc41-mt', 
                  '-lpthread',
                ], 
              }],
            ],
          }],
        ],
      },
      'sources': [
        '../base/asyncall.cc',
        '../base/atomicops-internals-x86.cc',
        '../base/baseasync.cc',
        '../base/common.cc',
        '../base/doobs_hash.cc',
        '../base/jdbcurl.cc',
        '../base/logging.cc',
        '../base/md5.cc',
        '../base/messagequeue.cc',
        '../base/mkdirs.cc',
        '../base/network.cc',
        '../base/no_exception.cc',
        '../base/once.cc',
        '../base/pathops.cc',
        '../base/pcount.cc',
        '../base/signals.cc',
        '../base/startuplist.cc',
        '../base/stringdigest.cc',
        '../base/stringencode.cc',
        '../base/thread.cc',
        '../base/threadpool.cc',
      ],
    },
    {
      'target_name':'base_test',
      'type': 'executable',
      'dependencies': [
        'gtest.gyp:gtest',
        'gtest.gyp:gtest_main',
        'base.gyp:base',
      ],  
      'conditions':[
        ['OS=="linux"',{
          'link_settings': {
            'ldflags': [
              '-Wl,-rpath <(BOOST_PATH)/lib',
            ],
          },
        }],
      ],
      'sources': [
        '../base/test/string_md5_test.cc'
      ],
    },
  ],
}
