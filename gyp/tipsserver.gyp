{
  'variables': {
   'PROTOC_PATH':'<!(dirname ~/share/bin/protoc)/protoc',
   # 'PROTOC_PATH':'<!(which protoc)',
   'ICE_PATH':'<!(echo $ICE_HOME)',
   'SLICE2CPP_PATH':'<(ICE_PATH)/bin/slice2cpp',
   'BOOST_PATH':'<!(echo $BOOST_ROOT)',
  },
  'conditions':[
    ['OS=="linux"', {
      'target_defaults': {
        'cflags':['-fPIC', '-g', '-O2'],
        'defines':['OS_LINUX', 'POSIX'],
      },
    },
    ],
    ['OS=="win"', {
      'target_defaults': {},
        },
    ],
  ],
  'targets':[
      {
        'target_name':'tipsserver',
        'type':'executable',
        'include_dirs':[
          '..',
         '<(BOOST_PATH)/include',

            '/usr/local/xce-dev/include',
            '/usr/local/xce/include', # mysql++
            '/usr/include/mysql',
            '/opt/Ice-3.3.1/include',
            '/usr/local/distcache-dev/include', # for zookeeper
        ],
        'dependencies':[
            'fcgi.gyp:fcgi',
            'base.gyp:base',
            'oceadapter.gyp:utiladapter',
            'oceadapter.gyp:oceadapter',
            'oceadapter.gyp:distcache',
            'cwf.gyp:cwfmain',
            'cwf.gyp:cwf',
        ],

	'conditions':[
		['OS=="linux"', {
			'libraries': [
          '-L<(BOOST_PATH)/lib', 

			'-L/usr/lib64/mysql/', '-lmysqlclient',
			'-L/usr/local/xce/lib', '-lmysqlpp',
			'-L/usr/local/xce-dev/lib64/', '-lctemplate',
			'-L/usr/local/distcache-dev/lib/','-lzookeeper_mt',  '-lprotobuf',
			'-L<(ICE_PATH)/lib64', '-lIceUtil', '-lIce', '-lIceGrid', '-lIceBox',
			'-lrt', '-lboost_regex', '-lboost_date_time', '-lboost_thread', '-lboost_system', '-lpthread',
			'-llog4cplus', 
			'-L/usr/local/distcache/lib', '-lmemcached'
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
            '../app/Tips/tipsserver/action.cc',
            '../app/Tips/tipsserver/register.cc',
        ],
      },
  ],
}

