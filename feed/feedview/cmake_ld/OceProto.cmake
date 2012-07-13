
SET( XNS_PROTO_LIB_NAME "OceProto")
SET( OCEPROTO_SRC_DIR "${XNS_ROOT_DIR}/${XNS_PROTO_LIB_NAME}/proto")
SET( OCEPROTO_CC_DIR "${XNS_BUILD_DIR}/tmp/${XNS_PROTO_LIB_NAME}/cc")
SET( OCEPROTO_JAVA_DIR "${XNS_BUILD_DIR}/tmp/${XNS_PROTO_LIB_NAME}/java")
FILE( MAKE_DIRECTORY ${OCEPROTO_CC_DIR} ${OCEPROTO_JAVA_DIR} )
#
# Proto
#
FILE( GLOB oce_proto_source RELATIVE ${OCEPROTO_SRC_DIR} "${OCEPROTO_SRC_DIR}/*.proto" )
SET ( XNS_OCEPROTO_SOURCE_FILES ${oce_proto_source} CACHE INTERNAL "OceProto proto files" FORCE)


#
# Cc
#
MESSAGE( STATUS "Planning to build cc version of Proto interfaces ${XNS_PROTO_LIB_NAME}." )

INCLUDE( ${XNS_CMAKE_DIR}/protobuf_rules.cmake )

GENERATE_PROTOC_RULES( ${OCEPROTO_SRC_DIR} ${OCEPROTO_CC_DIR} oce_proto_generated_sources
  oce_proto_generated_headers ${XNS_OCEPROTO_SOURCE_FILES} )

#ADD_LIBRARY(OceProto ${oce_proto_generated_sources})
#SET_TARGET_PROPERTIES( OceProto PROPERTIES LINKER_LANGUAGE C )
#TARGET_LINK_LIBRARIES(OceProto ${PROTOBUF_LIBRARY})

GENERATE_ARCHIVE_EACH( OceProto "${oce_proto_generated_sources}" )

