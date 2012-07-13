#!/bin/bash
rm -rf tmp
mkdir tmp
cd tmp

topdir="../../../../"
ocedir=$topdir"OceSlice/slice/"
utildir=$topdir"UtilSlice/slice"
iceextdir=$topdir"IceExt/src"
utilcxxdir=$topdir"UtilCxx/src"
xcedir="/usr/local/xce/include"
xcelib="/usr/local/xce/lib64/"

$ICE_HOME/bin/slice2cpp ../../slice/ObjectCacheDemo.ice -I$ocedir -I$utildir -I$ICE_HOME/slice
$ICE_HOME/bin/slice2cpp $utildir/ObjectCache.ice -I$utildir -I$ICE_HOME/slice
$ICE_HOME/bin/slice2cpp $utildir/Util.ice -I$utildir -I$ICE_HOME/slice
c++ -c -I. -I$ICE_HOME/include -O2 -m64 -Wall -D_REENTRANT ./*.cpp
c++ -c -I. -I$topdir -I$xcedir -I$iceextdir -I$utilcxxdir -I../../src -I$ICE_HOME/include -O2 -m64 -Wall ../../src/ObjectCacheDemoI.cpp ../../src/ObjectCacheDemoServer.cpp $iceextdir/ObjectCacheI.cpp $utilcxxdir/TaskManager.cpp $utilcxxdir/ThreadPoolExecutor.cpp $utilcxxdir/ThreadPoolScheduler.cpp
c++ -c -I. -I$topdir -I$xcedir -I$iceextdir -I$utilcxxdir -I../../src -I$ICE_HOME/include -O2 -m64 -Wall ../../src/ObjectCacheDemoClient.cpp
c++ -Wl,-rpath,$ICE_HOME/lib64 -rdynamic -m64 -Wall -L$ICE_HOME/lib64 -L$xcelib -o client ObjectCacheDemoClient.o ObjectCacheDemo.o Util.o ObjectCache.o -lIce -lIceUtil -lIceGrid -lpthread -lrt
c++ -Wl,-rpath,$ICE_HOME/lib64 -Wl,-rpath,$xcelib -rdynamic -m64 -Wall -L$ICE_HOME/lib64 -L$xcelib -o server ObjectCacheDemoServer.o ObjectCacheDemoI.o ObjectCacheDemo.o Util.o ObjectCache.o ObjectCacheI.o TaskManager.o ThreadPoolExecutor.o ThreadPoolScheduler.o -lIce -lIceUtil -lIceGrid -lpthread -lrt -llog4cplus
mv ./client ./server ../
rm -rf ../tmp
