TOP_SRCDIR = ..

# 1 得到所有源代码文件
CPPS = ${wildcard *.cc}
CPPS += ${wildcard *.cpp}

# 3 all *.o files
OBJS := ${subst .cc,.o, $(CPPS)}
OBJS := ${subst .cpp,.o,$(OBJS)}

CXXFLAGS += -DPOSIX -I/opt/Ice-3.3.1/include -I$(TOP_SRCDIR)



all: libfoo.a slice test
	@echo CPPS = $(CPPS)
	@echo OBJS = $(OBJS)

.PHONY: clean test slice

libfoo.a: libfoo.a($(OBJS))
#	@$(RANLIB) cr libfoo $(OBJS)

slice:
	$(MAKE) -f MakeSlice

test:	
	$(MAKE) -C test -f ../MakeTest

clean:
	rm -f $(OBJS) libfoo.a
	$(MAKE) -C test -f ../MakeTest clean
	$(MAKE) -f MakeSlice clean


