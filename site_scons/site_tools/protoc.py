__author__ = "Scott Stafford"

import SCons.Action
import SCons.Builder
import SCons.Defaults
import SCons.Node.FS
import SCons.Util

from SCons.Script import File, Dir

import os.path

protocs = 'protoc'

ProtocAction = SCons.Action.Action('$PROTOCCOM', '$PROTOCCOMSTR')
def ProtocEmitter(target, source, env):
    # print 'enter emitter, source: ', [str(s) for s in source], 'target: ', [str(s) for s in target]
    dirOfCallingSConscript = Dir('.').srcnode()
    env.Prepend(PROTOCPROTOPATH = dirOfCallingSConscript.path)
    
    """
    source_with_corrected_path = []
    for src in source:
        commonprefix = os.path.commonprefix([dirOfCallingSConscript.path, src.srcnode().path])
        if len(commonprefix)>0:
            source_with_corrected_path.append( src.srcnode().path[len(commonprefix + os.sep):] )
        else:
            source_with_corrected_path.append( src.srcnode().path )
        
    source = source_with_corrected_path
    
    print "Corrected? SOURCE:", [str(s) for s in source]
    """
    if len(target):
      target.pop()
        
    for src in source:
        modulename = os.path.splitext(str(src))[0]

        if env['PROTOCOUTDIR']:            
            base = os.path.join(env['PROTOCOUTDIR'] , modulename)
            target.extend( [ base + '.pb.cc' ])#, base + '.pb.h' ] ) # 
        
        if env['PROTOCPYTHONOUTDIR']:
            base = os.path.join(env['PROTOCPYTHONOUTDIR'] , modulename)
            target.append( base + '_pb2.py' )

    try:
        target.append(env['PROTOCFDSOUT'])
    except KeyError:
        pass
    
    # print "PROTOC SOURCE:", [str(s) for s in source]
    # print "PROTOC TARGET:", [str(s) for s in target]

    return target, source

ProtocBuilder = SCons.Builder.Builder(action = ProtocAction,
                                   emitter = ProtocEmitter,
                                   srcsuffix = '$PROTOCSRCSUFFIX')

def generate(env):
    """Add Builders and construction variables for protoc to an Environment."""
    try:
        bld = env['BUILDERS']['Protoc']
    except KeyError:
        bld = ProtocBuilder
        env['BUILDERS']['Protoc'] = bld
        
    env['PROTOC'] = env.Detect(protocs) or '/usr/local/distcache-dev/bin/protoc'
    env['PROTOCFLAGS'] = SCons.Util.CLVar('')
    env['PROTOCPROTOPATH'] = SCons.Util.CLVar('')
    # File does not reside within any path specified using --proto_path (or -I).  
    # You must specify a --proto_path which encompasses this file.  Note that the 
    # proto_path must be an exact prefix of the .proto file names -- protoc is too
    # dumb to figure out when two paths (e.g. absolute and relative) are equivalent
    # (it's harder than you think).
    # env['PROTOCCOM'] = '$PROTOC ${["-I%s"%x for x in PROTOCPROTOPATH]} $PROTOCFLAGS --cpp_out=$PROTOCCPPOUTFLAGS$PROTOCOUTDIR ${PROTOCPYTHONOUTDIR and ("--python_out="+PROTOCPYTHONOUTDIR) or ""} ${PROTOCFDSOUT and ("-o"+PROTOCFDSOUT) or ""} ${SOURCES}'
    env['PROTOCCOM'] = '$PROTOC -I${SOURCE.dir} ${["-I%s"%x for x in PROTOCPROTOPATH]} $PROTOCFLAGS --cpp_out=$PROTOCCPPOUTFLAGS$PROTOCOUTDIR ${PROTOCPYTHONOUTDIR and ("--python_out="+PROTOCPYTHONOUTDIR) or ""} ${PROTOCFDSOUT and ("-o"+PROTOCFDSOUT) or ""} ${SOURCES}'
    env['PROTOCOUTDIR'] = '${SOURCE.dir}'
    env['PROTOCPYTHONOUTDIR'] = None #"python"
    env['PROTOCSRCSUFFIX']  = '.proto'

def exists(env):
    return env.Detect(protocs)
