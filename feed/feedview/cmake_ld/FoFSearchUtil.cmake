
FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/search/FriendOfFriendSearch/library/src/*.cpp" )

ADD_LIBRARY( "FoFSearchUtil" ${BUILD_SRC_CPP_SOURCE})
