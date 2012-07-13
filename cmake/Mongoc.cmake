FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/third-party/mongo-c-driver/*" )
ADD_LIBRARY( "Mongoc" ${BUILD_SRC_CPP_SOURCE})
