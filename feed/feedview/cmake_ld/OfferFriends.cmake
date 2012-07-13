
FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/socialgraph/OfferFriends/share/*.cpp" )

ADD_LIBRARY( OfferCacheData ${BUILD_SRC_CPP_SOURCE})

