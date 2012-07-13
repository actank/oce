{
  'variables': {
   'ICE_PATH':'<!(echo $ICE_HOME)',
   'SLICE2CPP_PATH':'<(ICE_PATH)/bin/slice2cpp',
  },
 'conditions':[
    ['OS=="linux"', {
     'target_defaults': {
       'cflags':['-fPIC', '-g', '-O2'],
       'defines':['OS_LINUX', 'POSIX', 'NEWARCH'],
       'include_dirs':[
         '..',
         '<(ICE_PATH)/include',
         '/usr/include/mysql',
         '/usr/local/xce/include', # for mysql++
         ],
        },
      },
    ],
    ['OS=="win"', {'target_defaults': {},},],
  ],
  'targets': [
   {
   'target_name': 'dbpool',
   'type': '<(library)',
   'include_dirs':[
      '../IceExt/src',
      '../UtilCxx/src',
      '../UtilSlice/slice',
      '../OceSlice/slice',
      '../DbCxxPool/src',
   ],
   'dependencies': [
     'base.gyp:base'
   ],
   'direct_dependent_settings': {
     'target_conditions': [
       ['_type=="executable"', {
         'conditions':[
           ['OS=="linux"',{
             'libraries': [
               '-L/usr/lib64/mysql/', '-lmysqlclient',
               '-L/usr/local/xce/lib', '-lmysqlpp',
             ], 
           }],
         ],
       }],
     ],
   },
   'sources': [
     '../dbpool/connectionpoolmanager.cpp',
     '../dbpool/connectionholder.cpp',
     '../dbpool/connectionpool.cpp',
     '../dbpool/connectionquery.cpp',
     '../dbpool/queryrunner.cpp',
     '../dbpool/connectionpool.h',
     '../dbpool/connectionpoolmanager.h',
     '../dbpool/queryrunner.h',
     '../dbpool/connectionholder.h',
     '../dbpool/connectionquery.h',
    ],
   },
  ]
}

