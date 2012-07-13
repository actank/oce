{
   'targets': [
    {
      'target_name': 'gid_test',
      'type': 'executable',
      'cflags': [ '-g' ],
      'defines': ['OS_LINUX'],
      'msvs_guid': 'D8EC280E-CD33-4634-AE71-8A181A43BBBB',
      'include_dirs': ['.', '../arch/testing/gtest/include'],
      'dependencies': [
        '../arch/testing/gtest.gyp:gtestmain'
      ],
      'libraries': ['-lpthread'],
      'sources': [
        'gid_test.cc',
      ]
    }
  ]
}
