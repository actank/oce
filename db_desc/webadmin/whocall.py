#!/usr/bin/python
#encode: utf-8

r'''
1,
@who_called_me()
def index(request):
    return render_to_response('phc/index.html', None, context_instance=RequestContext(request))

2,
@who_called_me(out=open('/tmp/who_called_me', 'a'))
def index(request):
    return render_to_response('phc/index.html', None, context_instance=RequestContext(request))
'''

def funme():
    pass

def who_called_me(show_filename=True, out=None, indent=' '):
    def _wrapper(fn):
        def _inner_wrapper(*args, **kwargs):
            import sys
            import inspect
            output = out or sys.stdout
            print "NB:CALLBACK"
            assert hasattr(output, 'write'), \
                'argument \'out\' of function \'who_called_me\' must have a write method'
            index = 0
            stack = inspect.stack()
            stack.reverse()
            # remove ourself from the stack list
            stack.pop()
            for record in stack:
                frame = record[0]
                line = frame.f_lineno
                func_name = frame.f_code.co_name
                if show_filename:
                    descr = frame.f_code.co_filename
                else:
                    descr = frame.f_globals["__name__"]
                print >>output, '%s%s@%s:%d' % (indent*index, descr, func_name, line)
                # to be safe explicitly delete the stack frame reference
                # @see http://docs.python.org/lib/inspect-stack.html
                del frame
                index += 1
            del stack
            if hasattr(output, 'flush'):
                output.flush()
            return fn(*args, **kwargs)
        return _inner_wrapper
    return _wrapper

if __name__ == "__main__":

   @who_called_me()
   def fun3():
     print "fun3"

   def fun2():
     fun3()

   def fun1():
     fun2()
   
   fun1()


