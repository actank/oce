#include <IceLogger.h>
#include "FeedCacheAdapter.h"


using namespace xce::feed;


/////////////////////////////////////////////////////////////////
FeedCachePrx FeedCacheAdapter::getManager(int id, int timeout) {
	return locate<FeedCachePrx>(_managers, "M", id, TWO_WAY, timeout);
}

FeedCachePrx FeedCacheAdapter::getManagerOneway(int id) {
	return locate<FeedCachePrx>(_managersOneway, "M", id, ONE_WAY);
}

void FeedCacheAdapter::put(const FeedItem& feed, const Int2IntMap& userWeight){
	map<int, map<int, int> > splits;
	MyUtil::Int2IntMap::const_iterator it = userWeight.begin();
	for(; it!=userWeight.end(); ++it){
		splits[it->first%cluster()][it->first] = it->second;
	}
	
	map<int, map<int, int> >::iterator sIt = splits.begin();
	for(; sIt!=splits.end(); ++sIt){
		try{
			getManagerOneway(sIt->first)->put(feed, sIt->second);
		}catch(Ice::Exception& e){
			MCE_WARN("FeedCacheAdapter::put --> invoke FeedCache err " << sIt->first << ", " << e);
		}
	}

}

FeedItemSeq FeedCacheAdapter::get(int userId){
	return getManager(userId)->get(userId);
}

void FeedCacheAdapter::remove(int userId, Ice::Long feedId){
	getManagerOneway(userId)->remove(userId, feedId);
}

void  FeedCacheAdapter::removeAll(int userId){
	getManagerOneway(userId)->removeAll(userId);
}

