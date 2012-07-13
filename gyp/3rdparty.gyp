{
  'variables': {
   'PROTOC_PATH':'/usr/local/distcache-dev/bin/protoc',
   'MAKE_TPL_VARNAMES_PATH':'/usr/local/xce-dev/bin/make_tpl_varnames_h',
   'ICE_PATH':'<!(echo $ICE_HOME)',
   'SLICE2CPP_PATH':'<(ICE_PATH)/bin/slice2cpp',
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
         '<(ICE_PATH)/include',
         '/usr/include/mysql',
         '/usr/local/xce/include',
         '/usr/local/distcache/include',
         '/usr/local/xce-dev/include',
         '/usr/local/distcache-dev/include',    # TODO: maybe wrong?
         '/usr/local/distcache-util/include',
         '/opt/mongo-cxx-driver/include',
         ],
        },
      },
    ],
    ['OS=="win"', {'target_defaults': {},},],
  ],
  'targets': [
   {
     'target_name': 'mongoc',
     'type': '<(library)',
     'defines': ['MONGO_HAVE_STDINT'],
     'include_dirs':[
        '../third-party/mongo-c-driver/',
     ],
     'sources': [
       '../third-party/mongo-c-driver/bson.c',
       '../third-party/mongo-c-driver/bson.h',
       '../third-party/mongo-c-driver/gridfs.c',
       '../third-party/mongo-c-driver/gridfs.h',
       '../third-party/mongo-c-driver/md5.c',
       '../third-party/mongo-c-driver/md5.h',
       '../third-party/mongo-c-driver/mongo.c',
       '../third-party/mongo-c-driver/mongo_except.h',
       '../third-party/mongo-c-driver/mongo.h',
       '../third-party/mongo-c-driver/numbers.c',
       '../third-party/mongo-c-driver/platform_hacks.h',
      ],
   },
  ]
}
