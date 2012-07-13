
AUX_SOURCE_DIRECTORY("../buddy/BuddyEntry/src" BUILD_BuddyEntry_SRC_CPP_SOURCE )
AUX_SOURCE_DIRECTORY("../buddy/Buddy/src/cache" BUILD_Buddy_SRC_CPP_SOURCE )
ADD_LIBRARY( "Buddy" SHARED EXCLUDE_FROM_ALL ${BUILD_BuddyEntry_SRC_CPP_SOURCE} ${BUILD_Buddy_SRC_CPP_SOURCE})
#ADD_EXECUTABLE( "Buddy"  EXCLUDE_FROM_ALL ${BUILD_BuddyEntry_SRC_CPP_SOURCE} ${BUILD_Buddy_SRC_CPP_SOURCE})
TARGET_LINK_LIBRARIES( "Buddy" OceCxxAdapterBuddyAdapter ReplicaCluster IceExt OceSliceReplicaCluster OceSliceSubjectObserver OceSliceXceStorm DbCxxPool OceCxxAdapterBuddyLogicAdapter OceCxxAdapterActiveTrackAdapter OceCxxAdapterActiveTrackFacadeAdapter OceCxxAdapterXceLoggerAdapter OceCxxAdapterUserCountAdapter OceSliceActiveTrack OceSliceUserCount OceSliceBuddy OceSliceBuddyCommon UtilCxx UtilSlice)

