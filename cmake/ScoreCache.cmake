
FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/score/ScoreCache/share/*.cpp" )

ADD_LIBRARY( ScoreCacheData ${BUILD_SRC_CPP_SOURCE})

