cmake_minimum_required(VERSION 2.6)

PROJECT(XceLogger)

SET(ICE_HOME /opt/Ice-3.3)
SET(ICE_SRC /opt/Ice-3.3)
SET(SLICE slice/XceLogger.ice)
AUX_SOURCE_DIRECTORY(src SRC_LIST)

ADD_CUSTOM_COMMAND(
	OUTPUT XceLogger.h XceLogger.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory build00
	COMMAND ${ICE_HOME}/bin/slice2cpp ${SLICE}
)
#AUX_SOURCE_DIRECTORY(build/generated GENERATED_LIST)

SET(INCLUDE_PATH src build/generated /usr/local/xce/include ${ICE_HOME}/include	/usr/include/mysql)
SET(LIB_PATH ${ICE_HOME}/lib /usr/local/xce/lib64)

SET(CMAKE_CXX_FLAGS "-O2 -fPIC")

INCLUDE_DIRECTORIES(${INCLUDE_PATH})
LINK_DIRECTORIES(${LIB_PATH})

LINK_LIBRARIES(Ice IceUtil IceXML IceBox IceStorm IceGrid pthread log4cplus)
ADD_LIBRARY(EvaLogger SHARED ${SRC_LIST})
#ADD_EXECUTABLE(EvaLogger ${SRC_LIST})
