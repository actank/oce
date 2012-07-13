#include <IceLogger.h>
#include "FeedCacheNAdapter.h"


using namespace xce::feed;


/////////////////////////////////////////////////////////////////
FeedCacheNPrx FeedCacheNAdapter::getManager(int id, int timeout) {
	return locate<FeedCacheNPrx>(_managers, "M", id, TWO_WAY, timeout);
}

FeedCacheNPrx FeedCacheNAdapter::getManagerOneway(int id) {
	return locate<FeedCacheNPrx>(_managersOneway, "M", id, ONE_WAY);
}

void FeedCacheNAdapter::put(const FeedItem& feed, const Int2IntMap& userWeight){
	map<int, map<int, int> > splits;
	MyUtil::Int2IntMap::const_iterator it = userWeight.begin();

	for(; it!=userWeight.end(); ++it){
    if(it->first > 0) {
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
			MCE_WARN("FeedCacheNAdapter::put --> invoke FeedCache err " << sIt->first << ", " << e);
		}
	}

}

void FeedCacheNAdapter::modify(int userId, const FeedItem & item) {
	return getManagerOneway(userId)->modify(userId, item);
}

FeedItemSeq FeedCacheNAdapter::get(int userId) {
	return getManager(userId)->get(userId);
}

FeedItemSeq FeedCacheNAdapter::getMore(int userId, Ice::Long maxFeed){
	return getManager(userId)->getMore(userId, maxFeed);
}

void FeedCacheNAdapter::remove(int userId, Ice::Long feedId){
	getManagerOneway(userId)->remove(userId, feedId);
}

void  FeedCacheNAdapter::removeAll(int userId){
	getManagerOneway(userId)->removeAll(userId);
}

MyUtil::LongSeq FeedCacheNAdapter::removeByMerge(int userId, Ice::Long merge, int stype) {
  return getManager(userId)->removeByMerge(userId, merge, stype);
}

MyUtil::LongSeq FeedCacheNAdapter::removeMergeByFeedId(int userId, Ice::Long feedId) {
  return getManager(userId)->removeMergeByFeedId(userId, feedId);
}
/*
void    FeedCacheNAdapter::removeUserFeeds(int userId) {
  getManagerOneway(userId)->removeUserFeeds(userId);
} */


void FeedCacheNAdapter::test(const FeedItem& feed, const Int2IntMap& userWeight){
	map<int, map<int, int> > splits;
	MyUtil::Int2IntMap::const_iterator it = userWeight.begin();
	for(; it!=userWeight.end(); ++it){
    if(it->first > 0) {
      splits[it->first%cluster()][it->first] = it->second;
    }
	}
	
	map<int, map<int, int> >::iterator sIt = splits.begin();
	for(; sIt!=splits.end(); ++sIt){
    if((it->first%cluster()) != 0){
      continue;
    }
		try{
      FeedCacheNPrx::uncheckedCast(getManagerOneway(sIt->first)->ice_datagram())->test(feed, sIt->second);
		}catch(Ice::Exception& e){
			MCE_WARN("FeedCacheAdapter::test --> invoke FeedCache err " << sIt->first << ", " << e);
		}
	}
   
  
}

