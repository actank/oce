
FILE( GLOB CPP_SOURCE "${XNS_ROOT_DIR}/feed/FeedMemcProxy/client/*.cc" )
FILE( GLOB CC_SOURCE "${XNS_ROOT_DIR}/feed/FeedMemcProxy/client/*.cpp" )
LIST(APPEND FeedMemcClient_SOURCE ${CC_SOURCE} ${CPP_SOURCE})

FILE( GLOB CPP1_SOURCE "${XNS_ROOT_DIR}/feed/FeedMemcProxy/util/*.cc" )
FILE( GLOB CC1_SOURCE "${XNS_ROOT_DIR}/feed/FeedMemcProxy/util/*.cpp" )
LIST(APPEND MemcProtoParser_SOURCE ${CC1_SOURCE} ${CPP1_SOURCE})

MESSAGE(STATUS "files ${FeedMemcClient_SOURCE}")

ADD_LIBRARY(MemcProtoParser STATIC ${MemcProtoParser_SOURCE})

ADD_LIBRARY(FeedMemcClient STATIC ${FeedMemcClient_SOURCE})

ADD_SERVICE_EXEC("feed/FeedMemcProxy/client" "client" MemcProtoParser Ice OceSliceRFeed OceSliceFeedMemcProxy OceProto protobuf memcached IceExt)
#TARGET_LINK_LIBRARIES(client OceSliceRFeed)
#TARGET_LINK_LIBRARIES(client OceSliceFeedMemcProxy)
#TARGET_LINK_LIBRARIES(client OceCxxAdapterFeedMemcProxyAdapter)
#TARGET_LINK_LIBRARIES(client OceCxxAdapterTalkCacheLoaderAdapter)
#TARGET_LINK_LIBRARIES(client OceProto)
#TARGET_LINK_LIBRARIES(client protobuf)
#TARGET_LINK_LIBRARIES(client memcached)


TARGET_LINK_LIBRARIES(FeedMemcClient MemcProtoParser)
TARGET_LINK_LIBRARIES(FeedMemcClient OceSliceRFeed)
TARGET_LINK_LIBRARIES(FeedMemcClient OceSliceFeedMemcProxy)
TARGET_LINK_LIBRARIES(FeedMemcClient OceCxxAdapterFeedMemcProxyAdapter)
TARGET_LINK_LIBRARIES(FeedMemcClient OceCxxAdapterTalkCacheLoaderAdapter)
TARGET_LINK_LIBRARIES(FeedMemcClient OceProto)
TARGET_LINK_LIBRARIES(FeedMemcClient protobuf)
TARGET_LINK_LIBRARIES(FeedMemcClient memcached)

ADD_SERVICE("feed/FeedMemcProxy/loader" "FeedMemcLoader" FeedMemcClient OceSliceRFeed OceSliceFeedMemcProxy ReplicaCluster IceExt OceSliceReplicaCluster OceSliceSubjectObserver OceSliceXceStorm DbCxxPool IceExt UtilCxx UtilSlice)
#ADD_SERVICE("FeedMemcProxy/content" "FeedMemcContent" FeedMemcClientNew OceSliceRFeed OceSliceFeedMemcProxy ReplicaCluster IceExt OceSliceReplicaCluster OceSliceSubjectObserver OceSliceXceStorm DbCxxPool IceExt UtilCxx UtilSlice protobuf memcached)
ADD_SERVICE("feed/FeedMemcProxy/proxy" "FeedMemcProxy" MemcProtoParser OceSliceRFeed OceSliceFeedMemcProxy ReplicaCluster IceExt OceSliceReplicaCluster OceSliceSubjectObserver OceSliceXceStorm DbCxxPool IceExt UtilCxx UtilSlice boost_system-gcc41-mt boost_regex boost_thread)

#MESSAGE(STATUS "google breakpad : ${BREAKPAD_INCLUDE_DIR} ${BREAKPAD_LIBRARY}")
#INCLUDE_DIRECTORIES( ${BREAKPAD_INCLUDE_DIR} )

IF (BREAKPAD_FOUND)
  MESSAGE(STATUS "google breakpad : ${BREAKPAD_INCLUDE_DIR} ${BREAKPAD_LIBRARY}")
  INCLUDE_DIRECTORIES( ${BREAKPAD_INCLUDE_DIR} )
  TARGET_LINK_LIBRARIES(FeedMemcProxy ${BREAKPAD_LIBRARY})
ENDIF (BREAKPAD_FOUND)

