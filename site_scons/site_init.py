# TODO: env.Proto('a.proto')

def TOOL_PROTO(env):
    """A Tool to add a header from $HEADER to the source file"""
        
    add_header = Builder(action=['echo "$HEADER" > $TARGET', 'cat $SOURCE >> $TARGET'])
    env.Append(BUILDERS = {'Proto' : add_header})
    # env['HEADER'] = '' # set default value

