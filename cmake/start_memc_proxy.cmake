
FILE(GLOB CPP_SOURCE "${XNS_ROOT_DIR}/memc_proxy/client/*.cc")
FILE(GLOB CC_SOURCE "${XNS_ROOT_DIR}/memc_proxy/client/*.cpp")
LIST(APPEND MemcClient_SOURCE ${CC_SOURCE} ${CPP_SOURCE})

ADD_LIBRARY(MemcClient STATIC ${MemcClient_SOURCE})
TARGET_LINK_LIBRARIES(MemcClient OceSliceRFeed)
TARGET_LINK_LIBRARIES(MemcClient OceSliceMemcProxy)
TARGET_LINK_LIBRARIES(MemcClient OceCxxAdapterMemcProxyAdapter)
TARGET_LINK_LIBRARIES(MemcClient OceProtoFeedContent)
TARGET_LINK_LIBRARIES(MemcClient OceProtoAdUserProfile)
TARGET_LINK_LIBRARIES(MemcClient protobuf)
TARGET_LINK_LIBRARIES(MemcClient memcached)

ADD_SERVICE("memc_proxy/proxy" "MemcProxy" OceSliceMemcProxy OceSliceRFeed ReplicaCluster IceExt OceSliceReplicaCluster OceSliceSubjectObserver OceSliceXceStorm DbCxxPool IceExt UtilCxx UtilSlice)

ADD_SERVICE("memc_proxy/feed_loader" "MemcFeedLoader" MemcClient ReplicaCluster IceExt OceSliceReplicaCluster OceSliceSubjectObserver OceSliceXceStorm DbCxxPool IceExt UtilCxx UtilSlice)
ADD_SERVICE("memc_proxy/feed_test" "MemcFeedTest" MemcClient ReplicaCluster IceExt OceSliceReplicaCluster OceSliceSubjectObserver OceSliceXceStorm DbCxxPool IceExt UtilCxx UtilSlice)

FIND_LIBRARY(BOOST_SYSTEM_LIBRARY NAMES boost_system PATHS ~yuwei.mu/xce/lib) 
IF (BOOST_SYSTEM_LIBRARY)
  TARGET_LINK_LIBRARIES(MemcProxy ${BOOST_SYSTEM_LIBRARY})
ENDIF (BOOST_SYSTEM_LIBRARY)

IF (BREAKPAD_FOUND)
  MESSAGE(STATUS "google breakpad : ${BREAKPAD_INCLUDE_DIR} ${BREAKPAD_LIBRARY}")
  INCLUDE_DIRECTORIES( ${BREAKPAD_INCLUDE_DIR} )
  TARGET_LINK_LIBRARIES(MemcProxy ${BREAKPAD_LIBRARY})
ENDIF (BREAKPAD_FOUND)

