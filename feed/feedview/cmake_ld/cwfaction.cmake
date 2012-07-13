FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/feed/cwf_action/*.cc" )

ADD_LIBRARY( "cwf_action" ${BUILD_SRC_CPP_SOURCE})
