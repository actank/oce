cmake_minimum_required(VERSION 2.6)

PROJECT(FCGI_FRIENDINPUT)

SET(OCE_BASE ${CMAKE_CURRENT_SOURCE_DIR}/../../../)
SET(ICE_HOME /opt/Ice-3.3/)
SET(FCGI_BASE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../fcgi/)

SET(SRC_SHOW main/main_show.cpp)
SET(SRC_CLICK main/main_click.cpp)
SET(SRC_CLOSE main/main_close.cpp)
#SET(SRC_PRESS main/main_press.cpp)

SET(INCLUDE_PATH /usr/local/xce/include /usr/local/xce-dev/include/ /usr/local/xce/include/boost-1_38/ include generated ${OCE_BASE}
${OCE_BASE}/UtilCxx/src ${OCE_BASE}/IceExt/src ${OCE_BASE}/DbCxxPool/src
${OCE_BASE}/release-newarch/tmp/UtilSlice/cpp
${OCE_BASE}/release-newarch/tmp/OceSlice/cpp  /usr/include/mysql
${OCE_BASE}/BatchUpdater/src ${OCE_BASE}/XceStorm/src
${OCE_BASE}/OceCxxAdapter/src usr/local/include ${FCGI_BASE}/include
${ICE_HOME}/include main/)

SET(LIB_PATH /usr/local/xce/lib64 ${OCE_BASE}/release-newarch ${FCGI_BASE}/lib ${ICE_HOME}/lib)

SET(CMAKE_CXX_FLAGS "-g")
#SET(CMAKE_CXX_FLAGS "-O2 -fPIC")

#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

INCLUDE_DIRECTORIES(${INCLUDE_PATH})

LINK_DIRECTORIES(${LIB_PATH})

LINK_LIBRARIES(Ice IceUtil IceXML IceBox IceStorm IceGrid pthread mysqlpp
crypto ssl rt log4cplus)
LINK_LIBRARIES(IceExt DbCxxPool OceSliceXceStorm UtilCxx UtilSlice )
LINK_LIBRARIES(libReplicaCluster.a)
#LINK_LIBRARIES(OceCxxAdapter)
LINK_LIBRARIES(libOceSliceReplicaCluster.a libOceSliceSubjectObserver.a )
LINK_LIBRARIES(libOceCxxAdapterBulletinAdapter.a)
LINK_LIBRARIES(libOceSliceBulletin.a)
LINK_LIBRARIES(libReplicaCluster.a libfcgi.a)
LINK_LIBRARIES(IceExt DbCxxPool OceSliceXceStorm UtilCxx UtilSlice OceSlice)

ADD_EXECUTABLE(show ${SRC_SHOW})
ADD_EXECUTABLE(click ${SRC_CLICK})
ADD_EXECUTABLE(close ${SRC_CLOSE})
#ADD_EXECUTABLE(press ${SRC_PRESS})
