#include <IceLogger.h>
#include "FeedCacheLogicAdapter.h"


using namespace xce::feed;


/////////////////////////////////////////////////////////////////
FeedCacheLogicPrx FeedCacheLogicAdapter::getManager(int id, int timeout) {
  return locate<FeedCacheLogicPrx>(_managers, "M", id, TWO_WAY, timeout);
}

FeedCacheLogicPrx FeedCacheLogicAdapter::getManagerOneway(int id) {
  return locate<FeedCacheLogicPrx>(_managersOneway, "M", id, ONE_WAY);
}

MyUtil::LongSeq FeedCacheLogicAdapter::resortFeedItems(int userId, const ::xce::feed::FeedItemSeq & feedItemSeq) {

  return getManager(userId)->resortFeedItems(userId, feedItemSeq);
}

//MyUtil::LongSeq FeedCacheLogicAdapter::resortFeedItems2(int userId, const ::xce::feed::FeedItemSeq & feedItemSeq) {
//
//  return getManager(userId)->resortFeedItems2(userId, feedItemSeq);
//} 

FeedItemSeq   FeedCacheLogicAdapter::get(int userId, int begin, int limit) {
  return getManager(userId)->get(userId, begin, limit);
}

void    FeedCacheLogicAdapter::put(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, bool syncFeedDB) {
  getManagerOneway(feed.actor)->put(feed, usersWeight, syncFeedDB);
}

void    FeedCacheLogicAdapter::remove(int userId, Ice::Long feedId, bool syncFeedDB) {
  getManagerOneway(userId)->remove(userId, feedId, syncFeedDB);
//  getManagerOneway(userId)->removeMergeByFeedId(userId, feedId, syncFeedDB);
}

void    FeedCacheLogicAdapter::removeAll(int userId, bool syncFeedDB) {
  getManagerOneway(userId)->removeAll(userId, syncFeedDB);
}

void    FeedCacheLogicAdapter::removeByMerge(int userId, Ice::Long mergeId, int stype, bool syncFeedDB) {
  getManagerOneway(userId)->removeByMerge(userId, mergeId, stype, syncFeedDB);
}

void    FeedCacheLogicAdapter::removeMergeByFeedId(int userId, Ice::Long feedId, bool syncFeedDB) {
  getManagerOneway(userId)->removeMergeByFeedId(userId, feedId, syncFeedDB);
}


void    FeedCacheLogicAdapter::test(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, bool syncDB) {
  //getManager(userId)->test(feed, usersWeight);
}



