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
    'target_name': 'socialgraph_proto',
    'type': 'none',
    'sources': [
       '../socialgraph/socialgraphproto/item.proto',
       '../socialgraph/socialgraphproto/friendcluster.proto',
       '../socialgraph/socialgraphproto/pukfriendrecommend.proto',
    ],
    'rules': [
        {
         'rule_name':'genproto',
         'extension':'proto',
         'inputs': [],
         'variables': {
            # The protoc compiler requires a proto_path argument with the
            # directory containing the .proto file.
            # There's no generator variable that corresponds to this, so fake it.
           'rule_input_relpath':'../socialgraph/socialgraphproto',
          },
         'outputs': [
           '<(rule_input_relpath)/<(RULE_INPUT_ROOT).pb.h',
           '<(rule_input_relpath)/<(RULE_INPUT_ROOT).pb.cc',
          ],
         'action': [
           '<(PROTOC_PATH)',
           '--proto_path=<(rule_input_relpath)',
           '<(rule_input_relpath)/<(RULE_INPUT_NAME)',
           '--cpp_out=<(rule_input_relpath)',
          ],
         'message':'Generating C++ code from <(RULE_INPUT_PATH)',
        },
      ],
   },
  {
    'target_name': 'socialgraph_tpl',
    'type': 'none',
    'sources': [
       '../socialgraph/socialgraphd/tpl/ffw.tpl',
       '../socialgraph/socialgraphd/tpl/ffucr.tpl',
       '../socialgraph/socialgraphd/tpl/ffwr.tpl',
       '../socialgraph/socialgraphd/tpl/ffncw.tpl',
       '../socialgraph/socialgraphd/tpl/rvideo.tpl',
       '../socialgraph/socialgraphd/tpl/rblog.tpl',
    ],
    'rules': [
        {
         'rule_name':'gentpl',
         'extension':'tpl',
         'inputs': [],
         'variables': {
            # The protoc compiler requires a proto_path argument with the
            # directory containing the .proto file.
            # There's no generator variable that corresponds to this, so fake it.
           'rule_input_relpath':'../socialgraph/socialgraphd/tpl',
          },
         'outputs': [
           '<(rule_input_relpath)/<(RULE_INPUT_ROOT).h',
          ],
         'action': [
           '<(MAKE_TPL_VARNAMES_PATH)',
           '--header_dir=<(rule_input_relpath)',
           '<(rule_input_relpath)/<(RULE_INPUT_NAME)',
          ],
         'message':'Generating C++ code from <(RULE_INPUT_PATH)',
        },
      ],
   },
   {
   'target_name': 'socialgraph_util',
   'type': '<(library)',
   'defines' : ['SGTEST', 'GYP_COMPILE'],
   'include_dirs':[
      '../OceProto/proto/',
      '../IceExt/src',
      '../UtilCxx/src',
      '../UtilSlice/slice',
      '../OceSlice/slice',
      '../DbCxxPool/src',
   ],
   'direct_dependent_settings': {
     'target_conditions': [
       ['_type=="executable"', {
         'conditions':[
           ['OS=="linux"',{
             'libraries': [
               '-L/usr/local/distcache-dev/lib/', '-lzookeeper_mt',
             ], 
             'defines' : ['GYP_COMPILE'],
           }],
         ],
       }],
     ],
   },
   'sources': [
			'../socialgraph/socialgraphutil/mcclient.h',
			'../socialgraph/socialgraphutil/mcclient.cc',
			'../socialgraph/socialgraphutil/algostuff.h',
			'../socialgraph/socialgraphutil/algostuff.cc',
			'../socialgraph/socialgraphutil/mc_adapter.h',
			'../socialgraph/socialgraphutil/mc_adapter.cc',
			'../socialgraph/socialgraphutil/dbutil.h',
			'../socialgraph/socialgraphutil/dbutil.cc',
			'../socialgraph/socialgraphutil/zkclient.h',
			'../socialgraph/socialgraphutil/zkclient.cc',
			'../socialgraph/socialgraphutil/constants.h',
			'../socialgraph/socialgraphutil/constants.cc',
			'../socialgraph/socialgraphutil/socialgraphworkerI.cc',
			'../socialgraph/socialgraphutil/socialgraphworkerI.h',
			'../socialgraph/socialgraphproto/item.pb.h',
			'../socialgraph/socialgraphproto/friendcluster.pb.h',
			'../socialgraph/socialgraphproto/item.pb.cc',
			'../socialgraph/socialgraphproto/friendcluster.pb.cc',
			'../socialgraph/socialgraphproto/pukfriendrecommend.pb.h',
			'../socialgraph/socialgraphproto/pukfriendrecommend.pb.cc',
    ],
   },
   {
   'target_name': 'socialgraphd',
   'type': 'executable',
   'include_dirs':[
      '../OceProto/proto/',
      '../IceExt/src',
      '../UtilCxx/src',
      '../UtilSlice/slice',
      '../OceSlice/slice',
      '../DbCxxPool/src',
   ],
   'dependencies':[
      'base.gyp:base',
      'cwf.gyp:cwf',
      'cwf.gyp:cwfmain',
      'fcgi.gyp:fcgi',
      'dbdesc.gyp:dbpool',
      'oceadapter.gyp:utiladapter',
      'oceadapter.gyp:oceadapter',
      'socialgraph_util',
      'socialgraph_tpl',
   ],
   'conditions':[
      ['OS=="linux"', {
        'libraries': [],
        'link_settings': {
          'ldflags': [
            '-Wl,-rpath <(BOOST_PATH)/lib',
            '-Wl,-rpath <(ICE_PATH)/lib64',
            '-Wl,-rpath /usr/local/xce-dev/lib64',
            '-Wl,-rpath /usr/local/distcache/lib',
            '-Wl,-rpath /usr/local/xce/lib/',
          ],
        },
     }],
   ],
   'sources': [
  		'../socialgraph/socialgraphd/headurl.cc', 
  		'../socialgraph/socialgraphd/base_action.cc', 
  		'../socialgraph/socialgraphd/layout.cc',
  		'../socialgraph/socialgraphd/action.cc',
  		'../socialgraph/socialgraphd/register.cc',
  		'../socialgraph/socialgraphd/base_action.h',
  		'../socialgraph/socialgraphd/socialgraphlogic_adapter.h',
  		'../socialgraph/socialgraphd/headurl.h',
  		'../socialgraph/socialgraphd/layout.h',
  		'../socialgraph/socialgraphd/action.h',
  		'../arch/site_xiaonei.cc', # ugly
    ],
   },
   {
   'target_name': 'ffucr_compute',
   'type': 'executable',
   'defines' : ['GYP_COMPILE'],
   'include_dirs':[
      '../OceProto/proto/',
      '../IceExt/src',
      '../UtilCxx/src',
      '../UtilSlice/slice',
      '../OceSlice/slice',
      '../DbCxxPool/src',
   ],
   'dependencies':[
      'base.gyp:base',
      'cwf.gyp:cwf',
      'cwf.gyp:cwfmain',
      'fcgi.gyp:fcgi',
      'dbdesc.gyp:dbpool',
      'oceadapter.gyp:utiladapter',
      'oceadapter.gyp:oceadapter',
      'socialgraph_util',
   ],
   'conditions':[
      ['OS=="linux"', {
        'libraries': [],
        'link_settings': {
          'ldflags': [
            '-Wl,-rpath <(BOOST_PATH)/lib',
            '-Wl,-rpath <(ICE_PATH)/lib64',
            '-Wl,-rpath /usr/local/xce-dev/lib64',
            '-Wl,-rpath /usr/local/distcache/lib',
            '-Wl,-rpath /usr/local/xce/lib/',
          ],
        },
     }],
   ],
   'sources': [
      '../socialgraph/friendcluster/compute/ffucr_compute.cc',
    ],
   },
   {
   'target_name':'socialgraph_test',
   'type': 'executable',
   'defines': ['MONGO_HAVE_STDINT'],
   'dependencies': [
      'gtest.gyp:gtest',
      'gtest.gyp:gtest_main',
      'base.gyp:base',
      'dbdesc.gyp:dbpool',
      'oceadapter.gyp:utiladapter',
      'oceadapter.gyp:oceadapter',
      'socialgraph_util',
      '3rdparty.gyp:mongoc',
      'socialgraph_proto',
   ],  
   'conditions':[
      ['OS=="linux"', {
        'libraries': [
          '-L/usr/local/distcache-util/lib', '-lrdc-client', #ugly
          '-L/opt/mongo-cxx-driver/lib', '-lmongoclient',
        ],
        'link_settings': {
          'ldflags': [
            '-Wl,-rpath <(BOOST_PATH)/lib',
            '-Wl,-rpath <(ICE_PATH)/lib64',
            '-Wl,-rpath /usr/local/xce-dev/lib64',
            '-Wl,-rpath /usr/local/distcache/lib',
            '-Wl,-rpath /usr/local/xce/lib/',
          ],
        },
     }],
   ],
   'sources': [
#      '../socialgraph/socialgraphd/video/worker/tree.h',
#      '../socialgraph/socialgraphd/video/worker/tree.cc',
#      '../socialgraph/socialgraphutil/test/mc_adapter_test.cc',
#      '../socialgraph/socialgraphutil/test/algostuff_test.cc',
#      '../socialgraph/socialgraphutil/test/db_test.cc',
#      '../socialgraph/friendcluster/test/friendclustercache_test.cc',
#      '../socialgraph/socialgraphd/test/xce_adapter_test.cc',
#      '../socialgraph/socialgraphd/test/mongo_test.cc',
#      '../socialgraph/socialgraphd/test/random_int_test.cc',
#      '../socialgraph/socialgraphd/video/test/tree_test.cc',
#      '../socialgraph/socialgraphd/test/xce_logger_test.cc',
      '../socialgraph/socialgraphd/test/active_track_test.cc',
     ],
   },
  ]
}

