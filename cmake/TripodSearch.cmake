
FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/search/tripodsearchcache/*.cpp" )

ADD_LIBRARY( "TripodSearch" ${BUILD_SRC_CPP_SOURCE})
