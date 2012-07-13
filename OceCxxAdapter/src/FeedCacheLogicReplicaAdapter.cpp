#include <cmath>
#include "FeedCacheLogicReplicaAdapter.h"

using namespace MyUtil;
using namespace xce::feed;

FeedCacheLogicPrx FeedCacheLogicReplicaAdapter::getManager(long id) {
  return getProxy(id);
}

FeedCacheLogicPrx FeedCacheLogicReplicaAdapter::getManagerOneway(int id) {
  return getProxyOneway(id);
}

MyUtil::LongSeq FeedCacheLogicReplicaAdapter::resortFeedItems(int userId, const ::xce::feed::FeedItemSeq & feedItemSeq) {

  return getManager(userId)->resortFeedItems(userId, feedItemSeq);
}


FeedItemSeq   FeedCacheLogicReplicaAdapter::get(int userId, int begin, int limit) {
  return getManager(userId)->get(userId, begin, limit);
}

void    FeedCacheLogicReplicaAdapter::put(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, bool syncFeedDB) {
  getManagerOneway(feed.actor)->put(feed, usersWeight, syncFeedDB);
}

void    FeedCacheLogicReplicaAdapter::remove(int userId, Ice::Long feedId, bool syncFeedDB) {
  getManagerOneway(userId)->remove(userId, feedId, syncFeedDB);
//  getManagerOneway(userId)->removeMergeByFeedId(userId, feedId, syncFeedDB);
}

void    FeedCacheLogicReplicaAdapter::removeAll(int userId, bool syncFeedDB) {
  getManagerOneway(userId)->removeAll(userId, syncFeedDB);
}

void    FeedCacheLogicReplicaAdapter::removeByMerge(int userId, Ice::Long mergeId, int stype, bool syncFeedDB) {
  getManagerOneway(userId)->removeByMerge(userId, mergeId, stype, syncFeedDB);
}

void    FeedCacheLogicReplicaAdapter::removeMergeByFeedId(int userId, Ice::Long feedId, bool syncFeedDB) {
  getManagerOneway(userId)->removeMergeByFeedId(userId, feedId, syncFeedDB);
}


void    FeedCacheLogicReplicaAdapter::test(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, bool syncDB) {
  //getManager(userId)->test(feed, usersWeight);
}



