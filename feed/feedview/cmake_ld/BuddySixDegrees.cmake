AUX_SOURCE_DIRECTORY("../buddy/BuddySixDegrees/friendindex" BUILD_FRIENDINDEX_SRC_CPP_SOURCE )
AUX_SOURCE_DIRECTORY("../buddy/BuddySixDegrees/src" BUILD_BuddySixDegree_SRC_CPP_SOURCE )
ADD_LIBRARY( "BuddySixDegrees" SHARED EXCLUDE_FROM_ALL ${BUILD_FRIENDINDEX_SRC_CPP_SOURCE} ${BUILD_BuddySixDegree_SRC_CPP_SOURCE})
TARGET_LINK_LIBRARIES( "BuddySixDegrees" IceExt ReplicaCluster OceSliceBuddySixDegrees OceSliceReplicaCluster OceSliceSubjectObserver OceCommon OceSliceXceStorm DbCxxPool UtilCxx UtilSlice tokyotyrant tokyocabinet)
