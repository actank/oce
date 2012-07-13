
SET( XNS_PROTO_LIB_NAME "SocialGraphProto")
SET( SOCIALGRAPHPROTO_SRC_DIR "${XNS_ROOT_DIR}/socialgraph/socialgraphproto")
SET( SOCIALGRAPHPROTO_CC_DIR "${XNS_BUILD_DIR}/tmp/${XNS_PROTO_LIB_NAME}/cc")
SET( SOCIALGRAPHPROTO_JAVA_DIR "${XNS_BUILD_DIR}/tmp/${XNS_PROTO_LIB_NAME}/java")
FILE( MAKE_DIRECTORY ${SOCIALGRAPHPROTO_CC_DIR} ${SOCIALGRAPHPROTO_JAVA_DIR} )
#
# Proto
#
FILE( GLOB oce_proto_source RELATIVE ${SOCIALGRAPHPROTO_SRC_DIR} "${SOCIALGRAPHPROTO_SRC_DIR}/*.proto" )
SET ( XNS_SOCIALGRAPHPROTO_SOURCE_FILES ${oce_proto_source} CACHE INTERNAL "SocialGraphProto proto files" FORCE)


#
# Cc
#
MESSAGE( STATUS "Planning to build cc version of Proto interfaces ${XNS_PROTO_LIB_NAME}." )

INCLUDE( ${XNS_CMAKE_DIR}/protobuf_rules.cmake )

GENERATE_PROTOC_RULES( ${SOCIALGRAPHPROTO_SRC_DIR} ${SOCIALGRAPHPROTO_CC_DIR} oce_proto_generated_sources
  oce_proto_generated_headers ${XNS_SOCIALGRAPHPROTO_SOURCE_FILES} )

#ADD_LIBRARY(SocialGraphProto ${oce_proto_generated_sources})
#SET_TARGET_PROPERTIES( SocialGraphProto PROPERTIES LINKER_LANGUAGE C )
#TARGET_LINK_LIBRARIES(SocialGraphProto ${PROTOBUF_LIBRARY})

GENERATE_ARCHIVE_EACH( SocialGraphProto "${oce_proto_generated_sources}" )

