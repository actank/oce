{
  'conditions':[
    ['OS=="linux"', {
        'target_defaults': {
          'cflags':['-fPIC', '-g', '-O2'],
          'defines':['OS_LINUX', 'POSIX'],
        },
        'sources':[
          'os_unix.c',
        ], 
      },
    ],
    ['OS=="win"', {
        'target_defaults': {
          
        },
        'sources':[
          'os_win32.c',
        ],
      },
    ],
  ],
  'targets':[
    {
      'target_name':'fcgi',
      'type':'<(library)',
      'include_dirs':[
        '../cwf/libfcgi',
      ],
      'sources': [
        '../cwf/libfcgi/fcgi_stdio.c',
        '../cwf/libfcgi/fcgiapp.c',
        '../cwf/libfcgi/fcgio.cpp',
        '../cwf/libfcgi/os_unix.c',
      ],
    },
  ],
}
