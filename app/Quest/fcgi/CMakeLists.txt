cmake_minimum_required(VERSION 2.6)

PROJECT(FCGI_ILIKE)

SET(OCE_BASE ${CMAKE_CURRENT_SOURCE_DIR}/../../code/oce/main_project/trunk/)
SET(ICE_HOME /opt/Ice-3.3/)
SET(FCGI_BASE ${CMAKE_CURRENT_SOURCE_DIR}/../../fcgi/)

SET(SRC_NEW main/newFlow.cpp)
SET(SRC_SHOW main/showTip.cpp)
SET(SRC_CLICK main/clickTip.cpp)

SET(INCLUDE_PATH /usr/local/xce/include include generated ${OCE_BASE}
${OCE_BASE}/UtilCxx/src ${OCE_BASE}/IceExt/src ${OCE_BASE}/DbCxxPool/src
${OCE_BASE}/release-newarch/tmp/UtilSlice/cpp
${OCE_BASE}/release-newarch/tmp/OceSlice/cpp  /usr/include/mysql
${OCE_BASE}/BatchUpdater/src ${OCE_BASE}/XceStorm/src
${OCE_BASE}/OceCxxAdapter/src usr/local/include ${FCGI_BASE}/include
${ICE_HOME}/include main/ /usr/local/xce/include/boost-1_38/boost)

SET(LIB_PATH /usr/local/xce/lib64 ${OCE_BASE}/release-newarch ${FCGI_BASE}/lib ${ICE_HOME}/lib)

SET(CMAKE_CXX_FLAGS "-g -O2")
#SET(CMAKE_CXX_FLAGS "-O2 -fPIC")

#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

INCLUDE_DIRECTORIES(${INCLUDE_PATH})

LINK_DIRECTORIES(${LIB_PATH})

LINK_LIBRARIES(Ice IceUtil IceXML IceBox IceStorm IceGrid pthread mysqlpp
crypto ssl rt log4cplus)
#LINK_LIBRARIES(IceExt DbCxxPool OceSliceXceStorm UtilCxx UtilSlice )
#LINK_LIBRARIES(libReplicaCluster.a)
#LINK_LIBRARIES(OceCxxAdapter)
#LINK_LIBRARIES(libOceSliceReplicaCluster.a libOceSliceSubjectObserver.a libOceSliceILikeIt.a)
LINK_LIBRARIES(libOceSliceQuest.a)
LINK_LIBRARIES(libOceCxxAdapterQuestCacheAdapter.a)
#LINK_LIBRARIES(libOceCxxAdapterLikeEvaAdapter.a)
LINK_LIBRARIES(libReplicaCluster.a libfcgi.a libboost_regex.a)
LINK_LIBRARIES(IceExt DbCxxPool OceSliceXceStorm UtilCxx UtilSlice OceSlice)

ADD_EXECUTABLE(questnew ${SRC_NEW})
ADD_EXECUTABLE(questshow ${SRC_SHOW})
ADD_EXECUTABLE(questclick ${SRC_CLICK})
