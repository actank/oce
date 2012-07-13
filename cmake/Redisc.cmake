FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/third-party/redis-c-driver/*" )
ADD_LIBRARY( "Redisc" ${BUILD_SRC_CPP_SOURCE})
