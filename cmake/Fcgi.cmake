FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/feed/cwf_feed/libfcgi/*.c" )

ADD_LIBRARY( "Fcgi" ${BUILD_SRC_CPP_SOURCE})
SET_TARGET_PROPERTIES(Fcgi PROPERTIES LINKER_LANGUAGE C)