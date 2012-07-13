
FILE( GLOB CPP_SOURCE "${XNS_ROOT_DIR}/feed/FeedMemcProxy/client/*.cc" )
FILE( GLOB CC_SOURCE "${XNS_ROOT_DIR}/feed/FeedMemcProxy/client/*.cpp" )
LIST(APPEND FeedMemcClient_SOURCE ${CC_SOURCE} ${CPP_SOURCE})

MESSAGE(STATUS "files ${FeedMemcClient_SOURCE}")

ADD_LIBRARY( FeedMemcClient STATIC ${FeedMemcClient_SOURCE})

TARGET_LINK_LIBRARIES(FeedMemcClient OceSliceRFeed)
TARGET_LINK_LIBRARIES(FeedMemcClient OceSliceFeedMemcProxy)
TARGET_LINK_LIBRARIES(FeedMemcClient OceCxxAdapterFeedMemcProxyAdapter)
TARGET_LINK_LIBRARIES(FeedMemcClient OceCxxAdapterTalkCacheLoaderAdapter)
TARGET_LINK_LIBRARIES(FeedMemcClient OceProto)
TARGET_LINK_LIBRARIES(FeedMemcClient protobuf)
TARGET_LINK_LIBRARIES(FeedMemcClient /opt/libmemcached/lib/libmemcached.a)
#TARGET_LINK_LIBRARIES(FeedMemcClient memcached)

#ADD_SERVICE("FeedMemcProxy/loader" "FeedMemcLoader" FeedMemcClient OceSliceRFeed OceSliceFeedMemcProxy ReplicaCluster IceExt OceSliceReplicaCluster OceSliceSubjectObserver OceSliceXceStorm DbCxxPool IceExt UtilCxx UtilSlice protobuf memcached)
#ADD_SERVICE("FeedMemcProxy/content" "FeedMemcContent" FeedMemcClientNew OceSliceRFeed OceSliceFeedMemcProxy ReplicaCluster IceExt OceSliceReplicaCluster OceSliceSubjectObserver OceSliceXceStorm DbCxxPool IceExt UtilCxx UtilSlice protobuf memcached)
ADD_SERVICE("feed/FeedMemcProxy/proxy" "FeedMemcProxy" OceSliceRFeed OceSliceFeedMemcProxy ReplicaCluster IceExt OceSliceReplicaCluster OceSliceSubjectObserver OceSliceXceStorm DbCxxPool IceExt UtilCxx UtilSlice boost_system boost_regex boost_thread)

#MESSAGE(STATUS "google breakpad : ${BREAKPAD_INCLUDE_DIR} ${BREAKPAD_LIBRARY}")
#INCLUDE_DIRECTORIES( ${BREAKPAD_INCLUDE_DIR} )

IF (BREAKPAD_FOUND)
  MESSAGE(STATUS "google breakpad : ${BREAKPAD_INCLUDE_DIR} ${BREAKPAD_LIBRARY}")
  INCLUDE_DIRECTORIES( ${BREAKPAD_INCLUDE_DIR} )
  TARGET_LINK_LIBRARIES(FeedMemcProxy ${BREAKPAD_LIBRARY})
ENDIF (BREAKPAD_FOUND)

