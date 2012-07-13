
FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/search/SearchCache2/src/DistSearchCacheLogic/*.cpp" )

GENERATE_ARCHIVE_EACH( "DistSearchLogic" "${BUILD_SRC_CPP_SOURCE}")
#ADD_LIBRARY( "DistSearchLogic" ${BUILD_SRC_CPP_SOURCE})
