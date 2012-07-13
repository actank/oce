__author__ = "Scott Stafford"

import SCons.Action
import SCons.Builder
import SCons.Defaults
import SCons.Node.FS
import SCons.Util

from SCons.Script import File, Dir

import os.path

protocs = 'slice2cpp'

SlicecAction = SCons.Action.Action('$SLICECCOM', '$SLICECCOMSTR')
def SlicecEmitter(target, source, env):
    #~print 'enter emitter, source: ', [str(s) for s in source], 'target: ', [str(s) for s in target]
    #~print 'module: ', Dir('.').srcnode().path
    
    # source: configserver.ice
    # target: configserver
    
    # source: project/trunk/UtilSlice/slice/ReplicaCluster.ice
    # target: project/trunk/UtilSlice/slice/ReplicaCluster
    
    # env.PrependUnique(SLICECSLICEPATH = Dir('.').srcnode().path)
    for src in source:
      modulename = os.path.split(str(src))[0]
      if modulename:
        env.PrependUnique(SLICECSLICEPATH = modulename)
    
    
    target_dir = str(target[0])
    target.pop()
    
    for src in source:
        modulename = os.path.split(str(src))[0]
        file = os.path.basename(str(src))
        basename = os.path.splitext(file)[0]

        base = os.path.join(modulename, basename)
        target.extend( [ base + '.cpp'] ) #, base + '.h' ] )

    #~print "SLICEC SOURCE:", [str(s) for s in source]
    #~print "SLICEC TARGET:", [str(s) for s in target]

    return target, source

SlicecBuilder = SCons.Builder.Builder(action = SlicecAction,
                                   emitter = SlicecEmitter,
                                   srcsuffix = '$SLICECSRCSUFFIX')

def generate(env):
    """Add Builders and construction variables for slicec to an Environment."""
    try:
        bld = env['BUILDERS']['Slicec']
    except KeyError:
        bld = SlicecBuilder
        env['BUILDERS']['Slicec'] = bld
        
    env['SLICEC'] = env.Detect(protocs) or '/opt/Ice-3.3/bin/slice2cpp'
    env['SLICECFLAGS'] = SCons.Util.CLVar('')
    env['SLICECSLICEPATH'] = SCons.Util.CLVar('')
    env['SLICECCOM'] = '$SLICEC ${["-I%s"%x for x in SLICECSLICEPATH]} $SLICECFLAGS --output-dir ${SOURCE.dir} -DA ${SOURCE}'
    env['SLICECOUTDIR'] = '${SOURCE.dir}'
    env['SLICECPYTHONOUTDIR'] = None #"python"
    env['SLICECSRCSUFFIX']  = '.ice'

def exists(env):
    return env.Detect(protocs)
