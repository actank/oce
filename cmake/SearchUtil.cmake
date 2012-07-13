
FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/search/SearchCache2/src/Util/*.cpp" )

ADD_LIBRARY( "SearchUtil" ${BUILD_SRC_CPP_SOURCE})
