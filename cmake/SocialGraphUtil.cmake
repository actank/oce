FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/socialgraph/socialgraphutil/*" )
FILE( GLOB CAL_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/socialgraph/socialgraphutil/calculator/*" )
FILE( GLOB COM_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/socialgraph/socialgraphutil/calculator/component/*" )

ADD_LIBRARY( "SocialGraphUtil" ${BUILD_SRC_CPP_SOURCE} ${CAL_SRC_CPP_SOURCE} ${COM_SRC_CPP_SOURCE} )
TARGET_LINK_LIBRARIES("SocialGraphUtil" ReplicaCluster ReplicaCluster OceSliceReplicaCluster OceSliceSubjectObserver IceExt OceSliceXceStorm DbCxxPool UtilCxx UtilSlice rdc-client.a)
