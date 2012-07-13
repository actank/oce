CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )

PROJECT (DjLoggingServer)

SET (CMAKE_VERBOSE_MAKEFILE "1")
SET (CMAKE_CXX_COMPILER "g++")
SET (CMAKE_CXX_FLAGS "-Wall -O3")

MESSAGE ( STATUS "**************** ENV ****************" )

EXEC_PROGRAM ( "date +%F_%k_%M" OUTPUT_VARIABLE SYS_TIME )

MESSAGE( STATUS "SYS_TIME: ${SYS_TIME}" )

MESSAGE ( STATUS "**************** DIR ****************" )

GET_FILENAME_COMPONENT( DJSERVER_DIR "${${PROJECT_NAME}_SOURCE_DIR}/../" ABSOLUTE )
GET_FILENAME_COMPONENT( TRUNK_DIR "${DJSERVER_DIR}/../../../" ABSOLUTE )
GET_FILENAME_COMPONENT( LOGGINGSERVER_DIR "${DJSERVER_DIR}/../" ABSOLUTE )
GET_FILENAME_COMPONENT( THIRDLIB_DIR "/data/home/xiang.ye/programs/" ABSOLUTE )

MESSAGE( STATUS "DEBUG: ${${PROJECT_NAME}_SOURCE_DIR}" )
MESSAGE( STATUS "DEBUG: ${DJSERVER_DIR}" )
MESSAGE( STATUS "DEBUG: ${TRUNK_DIR}" )
MESSAGE( STATUS "DEBUG: ${LOGGINGSERVER_DIR}" )
MESSAGE( STATUS "DEBUG: ${THIRDLIB_DIR}" )

INCLUDE_DIRECTORIES( 
        ${THIRDLIB_DIR}/jsoncpp-src-0.5.0/include
        ${THIRDLIB_DIR}/boost/include
        ${THIRDLIB_DIR}/log4cplus/include
        ${LOGGINGSERVER_DIR}/include
        ${TRUNK_DIR}
)

LINK_DIRECTORIES(
        ${THIRDLIB_DIR}/jsoncpp-src-0.5.0/libs/linux-gcc-4.1.2
        ${THIRDLIB_DIR}/boost/lib
        ${THIRDLIB_DIR}/log4cplus/lib
)

LINK_LIBRARIES(
	log4cplus boost_thread json_linux-gcc-4.1.2_libmt
)

MESSAGE ( STATUS "**************** CMAKE ****************" )

SET( SRC_LIST ${DJSERVER_DIR}/src/main.cc ${TRUNK_DIR}/base/url.cc ${TRUNK_DIR}/base/port.cc )

ADD_EXECUTABLE(djloggingserver.${SYS_TIME} ${SRC_LIST})

