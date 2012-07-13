{
  'variables': {
   'PROTOC_PATH':'/usr/local/distcache-dev/bin/protoc',
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
         '../OceProto/proto',
         ],
        },
      },
    ],
    ['OS=="win"', {'target_defaults': {},},],
  ],
  'targets': [
  {
    'target_name': 'distsearchcache_proto',
    'type': 'none',
    'sources': [
      '../OceProto/proto/DistSearchCache.proto',
    ],
    'rules': [
    {
      'rule_name':'genproto',
      'extension':'proto',
      'inputs': [],
      'variables': {
        'rule_input_relpath':'../OceProto/proto',
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
    'target_name': 'search',
    'type': 'executable',
    'include_dirs':[
      '../OceProto/proto/',
    '../IceExt/src',
    '../UtilCxx/src',
    '../UtilSlice/slice',
    '../OceSlice/slice',
    '../DbCxxPool/src',
    '/usr/local/distcache-util/include',
    '/usr/local/xce/include/boost-1_38',
    ],
    'dependencies':[
      'base.gyp:base',
    'cwf.gyp:cwf',
    'cwf.gyp:cwfmain',
    'fcgi.gyp:fcgi',
    'dbdesc.gyp:dbpool',
    'oceadapter.gyp:utiladapter',
    'oceadapter.gyp:oceadapter',
    ],
    'conditions':[
      ['OS=="linux"', {
        'libraries': ['-lboost_regex-gcc41-mt',],
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
    '../search/FriendOfFriendSearch2/adapter/include/ExternalSearchManagerAdapter.h',
    '../search/FriendOfFriendSearch2/adapter/src/ExternalSearchManagerAdapter.cpp',
    '../search/FriendOfFriendSearch2/adapter/include/ExternalPeopleSearchManagerAdapter.h',
    '../search/FriendOfFriendSearch2/adapter/src/ExternalPeopleSearchManagerAdapter.cpp',
    '../search/FriendOfFriendSearch2/adapter/include/PeopleSearchManagerAdapter.h',
    '../search/FriendOfFriendSearch2/adapter/src/PeopleSearchManagerAdapter.cpp',
    '../search/FriendOfFriendSearch2/adapter/include/SearchManagerAdapter.h',
    '../search/FriendOfFriendSearch2/adapter/src/SearchManagerAdapter.cpp',
    '../search/FriendOfFriendSearch2/adapter/include/PageSearchManagerAdapter.h',
    '../search/FriendOfFriendSearch2/adapter/src/PageSearchManagerAdapter.cpp',
    '../search/FriendOfFriendSearch2/adapter/include/AppSearchManagerAdapter.h',
    '../search/FriendOfFriendSearch2/adapter/src/AppSearchManagerAdapter.cpp',
    '../search/SearchCache2/src/DistSearchCacheLogic/DbOperation.h',
    '../search/SearchCache2/src/DistSearchCacheLogic/DbOperation.cpp',
    '../search/SearchCache2/src/DistSearchCacheLogic/DistSearchCacheAdapter.h',
    '../search/SearchCache2/src/DistSearchCacheLogic/DistSearchCacheAdapter.cpp',
    '../search/CwfSearch/Antispam/UserActivityClient.cpp',    
    '../search/CwfSearch/Antispam/FilterQuery.cpp',    
    '../search/CwfSearch/Antispam/FilterAnswer.cpp',    
    '../search/CwfSearch/Antispam/CheckResult.cpp',    
    '../search/CwfSearch/Antispam/OzeinFilterAdapter.cpp',    
    '../search/CwfSearch/action.cc',    
    '../search/CwfSearch/register.cc',    
    '../search/CwfSearch/SearchViewHelper/SearchViewHelper.cc',   
    ],
  },
  ],
}

