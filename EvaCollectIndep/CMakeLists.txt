cmake_minimum_required(VERSION 2.6)

PROJECT(EvaCollect)

SET(ICE_HOME /opt/Ice-3.3)
SET(SLICE_SRC slice/XceLogger.ice)
SET(SLICE_DIR slice)
SET(GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/generated)
AUX_SOURCE_DIRECTORY(src SRC_LIST)
AUX_SOURCE_DIRECTORY(build/generated GENERATED_LIST)

SET(INCLUDE_PATH src generated /usr/local/xce/include ${ICE_HOME}/include /usr/include/mysql)
SET(LIB_PATH ${ICE_HOME}/lib /usr/local/xce/lib64)

#SET(CMAKE_CXX_FLAGS "-O2 -fPIC")
SET(CMAKE_CXX_FLAGS "-DEBUG -fPIC")

#generate slice
INCLUDE(slice2cpp_rules.cmake)
GENERATE_SLICE2CPP_RULES(${SLICE_DIR} ${GENERATED_DIR}
	slice_generated_sources	slice_generated_headers ${SLICE_SRC})


INCLUDE_DIRECTORIES(${INCLUDE_PATH})
LINK_DIRECTORIES(${LIB_PATH})

LINK_LIBRARIES(Ice IceUtil IceXML IceBox IceStorm IceGrid pthread)
ADD_LIBRARY(EvaCollect SHARED ${SRC_LIST} ${GENERATED_LIST})
#ADD_EXECUTABLE(EvaCollect ${SRC_LIST} ${GENERATED_LIST})
