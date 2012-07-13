{
  'variables': {
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
        'target_name':'activemotionserver',
        'type':'executable',
        'include_dirs':[
		'..',
		'/usr/local/xce/include',
		'/opt/jsoncpp-src-0.5.0/include',
        ],
        'dependencies':[
            'fcgi.gyp:fcgi',
            'base.gyp:base',
            'cwf.gyp:cwfmain',
            'cwf.gyp:cwf',
        ],

	'conditions':[
		['OS=="linux"', {
			'libraries': [
				'-llog4cplus', 
				'-L/opt/jsoncpp-src-0.5.0/libs/linux-gcc-4.1.2', '-ljson_linux-gcc-4.1.2_libmt',
			],
			'link_settings': {
				'ldflags': [
				'-Wl,--rpath=/usr/local/xce/lib',
				'-Wl,--rpath=./libs',
				],
			},
		}],
	],

        'sources': [
            '../app/activemotionserver/worker.cc',
            '../app/activemotionserver/action.cc',
            '../app/activemotionserver/register.cc',
        ],
      },
  ],
}

