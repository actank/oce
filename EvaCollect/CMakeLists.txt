cmake_minimum_required(VERSION 2.6)

PROJECT(CollectThread)

SET(ICE_HOME /opt/Ice-3.3/)
SET(ICE_SRC /opt/Ice-3.3/)
SET(OCE_HOME ..)
SET(SRC_EXTEND)# ${OCE_HOME}/OceCxxAdapter/src/XceLoggerAdapter.cpp)
SET(SRC src/CollectThread.cpp ${SRC_EXTEND})

#SET(INCLUDE_PATH include /usr/local/xce/include src ${ICE_HOME}/include ${ICE_HOME}/src)
SET(INCLUDE_PATH src /usr/local/xce/include ${ICE_HOME}/include ${OCE_HOME}
	${OCE_HOME}/UtilCxx/src
	${OE_HOME}/OceCxxAdapter/src
	${OCE_HOME}/release-newarch/tmp/OceSlice/cpp
	${OCE_HOME}/release-newarch/tmp/UtilSlice/cpp) 
#SET(LIB_PATH /usr/local/xce/lib64 ${ICE_HOME}/lib)
SET(LIB_PATH ${ICE_HOME}/lib /usr/local/xce/lib64
	${OCE_HOME}/release-newarch /data/home/han.zhang/code/oce/main_project/trunk/release-newarch)

#SET(CMAKE_CXX_FLAGS "-g")
SET(CMAKE_CXX_FLAGS "-O2 -fPIC")

INCLUDE_DIRECTORIES(${INCLUDE_PATH})
LINK_DIRECTORIES(${LIB_PATH})

#LINK_LIBRARIES(Ice IceUtil IceXML IceBox IceStorm IceGrid pthread mysqlpp crypto ssl rt log4cplus)
LINK_LIBRARIES(Ice IceUtil IceXML IceBox IceStorm IceGrid pthread log4cplus
 libOceCxxAdapterXceLoggerAdapter.a
 libOceSliceXceLogger.a
 libOceSliceReplicaCluster.a
 libOceSliceSubjectObserver.a 
 libIceExt.a
 libOceSliceXceStorm.a libDbCxxPool.a libUtilCxx.a libUtilSlice.a)
ADD_LIBRARY(EvaCollect SHARED ${SRC})
#ADD_EXECUTABLE(EvaCollect ${SRC})
