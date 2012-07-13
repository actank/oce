
# Hack Scons?
# import SCons.Script
# exec "Slice = SCons.Script._SConscript.DefaultEnvironmentCall(repr(Slice))"

import os

env=Environment(tools=['default', 'protoc', 'slicec'])

try:
  BOOST_ROOT = os.environ["BOOST_ROOT"]
except:
  BOOST_ROOT = '/usr/local'

try:
  ICE_HOME = os.environ["ICE_HOME"]
except:
  ICE_HOME = '/opt/Ice-3.3'

Posix = ("linux", "darwin", "sunos", "aix", "hpux", "irix", "sunos", 'posix')
Export('Posix')

if env["PLATFORM"] in Posix: # ---------------------------------
  env['OBJ_SUFFIX'] = 'o'
  
  baseflags = "-Wall -g -O2 -Wno-deprecated -fPIC -DNEWARCH"
  env.AppendUnique(CFLAGS=baseflags.split())
  env.AppendUnique(CXXFLAGS=baseflags.split())
  
  env.AppendUnique(CPPDEFINES=["POSIX"])
  
  env.Append(CPPPATH=[
    os.path.join(BOOST_ROOT, 'include'),
    os.path.join(ICE_HOME, 'include'),
    '#/',
    '/usr/local/include',
    '/usr/include/mysql', # TODO: use configure
    '/usr/local/xce/include',
    '/usr/local/xce-dev/include',
    '/usr/local/distcache/include',
		'/usr/local/distcache-dev/include',
		#'/data/home/xiang.ye/programs/jsoncpp-src-0.5.0/include', #app/activemotionserver need json
    '#/OceSlice/slice',   # TODO: ugly
    '#/OceProto/proto',
    '#/UtilSlice/slice',   # TODO: ugly
    '#/IceExt/src', 
    '#/UtilCxx/src',
    '#/DbCxxPool/src' ,
    '#/XceAlert/src',
		'#/DbDescriptor/src',
		'#/ReplicaCluster/SubjectObserver/src/client/cxx',
    '#/ReplicaCluster/ClusterState/src/client/cxx',
    '#/ReplicaCluster/ServerState/src/client/cxx/',
		'#/DistCache/distributedcache/slice', 
		'#/DistCache/distributedcache/src/main/cpp',
		'#/OceCxxAdapter/src',
  ])

  env.Append(SLICECSLICEPATH=os.path.join(ICE_HOME, 'slice'))

  LibPath = [
    os.path.join(ICE_HOME, 'lib'),
    os.path.join(BOOST_ROOT, 'lib'),
    '/usr/local/xce/lib64/',
    '/usr/local/distcache-dev/lib',
    '/usr/local/xce-dev/lib64',
    '/usr/local/distcache/lib',
    #'/data/home/xiang.ye/programs/jsoncpp-src-0.5.0/libs/linux-gcc-4.1.2', #app/activemotionserver need json
    '/usr/lib64/mysql' # TODO: /usr/lib/mysql or /usr/lib64/mysql/
  ]
  
  env.Append(RPATH=LibPath)
  env.Append(LIBPATH=LibPath)
  
  boost_libs = ['boost_system', 'boost_thread', 'boost_regex', 'boost_date_time', 'boost_program_options']
  db_libs = ['mysqlclient', 'mysqlpp']
  ice_libs = ['Ice', 'IceGrid', 'IceBox', 'IceUtil']
  other_libs = ['pthread']
  optional_libs = ['tcmalloc_and_profiler', 'breakpad']
  all_libs = boost_libs + ice_libs + db_libs + other_libs + optional_libs

  libs_export = {
    'boost_libs' : boost_libs,
    'db_libs'    : db_libs,
    'ice_libs'   : ice_libs,
    'other_libs' : other_libs,
    'optional_libs' : optional_libs, 
    'all_libs'   : all_libs,
    }
  Export(libs_export)
  
# ---------------------------------
# gtest begin
env.Append(CPPPATH=[
  '#/', 
#  'c:/boost_1_40_0',
#  'c:/Ice-3.3.1/cpp/include',
  '#/third-party/protobuf/gtest/include'
  ])

#gtest_export = env.SConscript('third-party/protobuf/gtest/scons/SConscript')
#Export(gtest_export)
#GtestBinary = gtest_export['GtestBinary']
#gtest_main = gtest_export['gtest_main']
# gtest end

# ---------------------------------
cpp_ = {}
  
dirs = [
	  		'OceCxxAdapter/src', 
        'base', 'arch', 'dbpool',
        'cwf/libfcgi',
        'cwf',
        #'app/activemotionserver', #to compile app/activemotionserver open this line, and json in 'LibPATH' & 'CPPPATH'
        'socialgraph/socialgraphproto', 'socialgraph/socialgraphutil',
        'socialgraph/socialgraphd'
        #'socialgraph/test',
				#'socialgraph/friendcluster/compute',
			 ]

for d in dirs:
  d = SConscript('%s/SConscript' % d
    , exports=['env', 'cpp_']
    , variant_dir='build/'+ d
    )
  if d:
    cpp_.update(d)
