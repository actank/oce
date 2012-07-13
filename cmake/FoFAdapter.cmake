
FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/search/FriendOfFriendSearch2/adapter/src/*.cpp" )

ADD_LIBRARY( "FoFAdapter" ${BUILD_SRC_CPP_SOURCE})
