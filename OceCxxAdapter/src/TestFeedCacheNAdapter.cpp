#include <IceLogger.h>
#include "TestFeedCacheNAdapter.h"


using namespace xce::feed;


/////////////////////////////////////////////////////////////////
FeedCacheNPrx TestFeedCacheNAdapter::getManager(int id, int timeout) {
	return locate<FeedCacheNPrx>(_managers, "M", id, TWO_WAY, timeout);
}

FeedCacheNPrx TestFeedCacheNAdapter::getManagerOneway(int id) {
	return locate<FeedCacheNPrx>(_managersOneway, "M", id, ONE_WAY);
}

void TestFeedCacheNAdapter::put(const FeedItem& feed, const Int2IntMap& userWeight){
	map<int, map<int, int> > splits;
	MyUtil::Int2IntMap::const_iterator it = userWeight.begin();

	for(; it!=userWeight.end(); ++it){
    if((it->first % 10) == 3) { //////////////////////////////////////////////////////
      splits[it->first%cluster()][it->first] = it->second;
    }
//    //debug------------------
//    if((it->first%cluster()) != 0) {
//      continue;
//    }

////////////
	}
	
	map<int, map<int, int> >::iterator sIt = splits.begin();
	for(; sIt!=splits.end(); ++sIt){
		try{
			getManagerOneway(sIt->first)->put(feed, sIt->second);
		}catch(Ice::Exception& e){
			MCE_WARN("TestFeedCacheNAdapter::put --> invoke FeedCache err " << sIt->first << ", " << e);
		}
	}

}

void TestFeedCacheNAdapter::modify(int userId, const FeedItem & item) {
	return getManagerOneway(userId)->modify(userId, item);
}

FeedItemSeq TestFeedCacheNAdapter::get(int userId) {
	return getManager(userId)->get(userId);
}

FeedItemSeq TestFeedCacheNAdapter::getMore(int userId, Ice::Long maxFeed){
	return getManager(userId)->getMore(userId, maxFeed);
}

void TestFeedCacheNAdapter::remove(int userId, Ice::Long feedId){
	getManagerOneway(userId)->remove(userId, feedId);
}

void  TestFeedCacheNAdapter::removeAll(int userId){
	getManagerOneway(userId)->removeAll(userId);
}

MyUtil::LongSeq TestFeedCacheNAdapter::removeByMerge(int userId, Ice::Long merge, int stype) {
  return getManager(userId)->removeByMerge(userId, merge, stype);
}

MyUtil::LongSeq TestFeedCacheNAdapter::removeMergeByFeedId(int userId, Ice::Long feedId) {
  return getManager(userId)->removeMergeByFeedId(userId, feedId);
}
/*
void    TestFeedCacheNAdapter::removeUserFeeds(int userId) {
  getManagerOneway(userId)->removeUserFeeds(userId);
} */


void TestFeedCacheNAdapter::test(const FeedItem& feed, const Int2IntMap& userWeight){
   
  
}

