EXECUTABLE := OpiMMseg

LIBS := #pthread nsl m z rt Strategy DocUnique TCWordSeg #DocUnique #rt  #WenWenRelIdx
COMMON_LIB_PATH=#../baselib/src
COMMON_LIB_MODULES=#arith conf def encode io log mem network os util
INCS := $(COMMON_LIB_PATH) #./ ../baselib/src/log/ ../

LIBPATHS := # ../lib/

#USER_MARCOS := __PTHREADS_MUTEXES__ __linux__ __PTHREADS__ _MT  DEBUG __CMPR_POS__
USER_MARCOS := OUTPUT_RELA LOCAL_PC DOCUNIQUE TEST_SPEED ZHISHI REL LOAD_CAL SEARCH_REAULT_CAL TC_SEGMENT __PTHREADS_MUTEXES__ __linux__ __PTHREADS__ DEBUG _MT #__RELA_CALC__ INC_ONE_FILE
CFLAGS = -g -Wall  -Wl,-rpath=../../lib -D_REENTRANT -D_PTHREADS=6 -finline-functions -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
CC = g++

COMMON_LIB_DIRS := $(addprefix $(COMMON_LIB_PATH)/, $(COMMON_LIB_MODULES))
COMON_LIB_SRC := $(foreach COMMON_LIB_DIRS, $(COMMON_LIB_DIRS), $(wildcard $(COMMON_LIB_DIRS)/*.cpp))
SOURCE := $(wildcard *.cpp interface/*.cpp cache/*.cpp broker/*.cpp main/*.cpp common/*.cpp net/*.cpp conf/*.cpp data_mgr/*.cpp ) $(COMON_LIB_SRC)
OBJS := $(patsubst %.cpp,%.o,$(SOURCE))

#编译成目标文件.o
%.o:%.cpp
	$(CC) -fPIC $(CFLAGS) $(addprefix -D,$(USER_MARCOS)) $(addprefix -I, $(INCS)) -c $< -o $@

#编译成可执行文件
$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(addprefix -L,$(LIBPATHS)) -o  $(EXECUTABLE) $(OBJS) $(addprefix -l,$(LIBS))

link:
	$(CC) $(CFLAGS) $(addprefix -L,$(LIBPATHS)) -o  $(EXECUTABLE) $(OBJS) $(addprefix -l,$(LIBS))
