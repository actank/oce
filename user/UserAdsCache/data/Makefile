CC     = g++ # used C++-compiler
LD     = g++ # used linker
CFLAGS = -g -DNEWARCH -Wall # debugging <C++ compiler flags>
LFLAGS = -O0 # linker flags
INCS   = -I/usr/local/xce/include/boost-1_38/ -I/usr/local/xce/include -I../../OceCxxAdapter/src/ -I/opt/Ice-3.3/include -I../../ -I../../UtilCxx/src -I../../IceExt/src -I../../release-newarch/tmp/OceSlice/cpp/ -I../../release-newarch/tmp/UtilSlice/cpp
#INCS   = -I/usr/local/xce/include -I/opt/Ice-3.3/include -I./include 
#LIBS   = /opt/Ice-3.3/lib/ -lIce -lIceUtil#-L/usr/local/lib/ -ldatrie
 
SRCS   = $(wildcard ../../OceCxxAdapter/src/UserAdsCacheAdapter.cpp ../../UtilCxx/src/TaskManager.cpp ../../UtilCxx/src/ThreadPoolExecutor.cpp ../../UtilCxx/src/ThreadPoolScheduler.cpp ../../IceExt/src/AdapterI.cpp ../../IceExt/src/Channel.cpp ../../release-newarch/tmp/OceSlice/cpp/UserAdsCache.cpp ../../release-newarch/tmp/OceSlice/cpp/SubjectObserver.cpp ../../release-newarch/tmp/OceSlice/cpp/ReplicaCluster.cpp ../../release-newarch/tmp/UtilSlice/cpp/Util.cpp ../../release-newarch/tmp/UtilSlice/cpp/ObjectCache.cpp ../../ReplicaCluster/ClusterState/src/client/cxx/ClusterStateSubscriber.cpp ../../ReplicaCluster/SubjectObserver/src/client/cxx/SbjTopicI.cpp ../../release-newarch/tmp/OceSlice/cpp/BuddyCache.cpp ../../OceCxxAdapter/src/BuddyCacheAdapter.cpp ../../OceCxxAdapter/src/UserCacheReplicaAdapter.cpp ../../release-newarch/tmp/OceSlice/cpp/UserCache.cpp ../../OceCxxAdapter/src/UserCacheLoaderAdapter.cpp)
OBJS   = $(patsubst %cpp, %o, $(SRCS))
EXES   = libUserAdsCacheClient.a
 
.PHONY: all clean
 
all: $(EXES)
	@echo "----- success -----"
 
clean:
	rm -f $(OBJS) $(EXES)
 
rebuild: clean all
 
$(EXES): $(OBJS)
	ar -cr $@ $^
#  $(LD) $(LFLAGS) -o $@ $^ $(LIBS)
 
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@
 
%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@
