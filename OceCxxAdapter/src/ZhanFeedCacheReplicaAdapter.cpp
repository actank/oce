#include <IceLogger.h>
#include "ZhanFeedCacheReplicaAdapter.h"


using namespace xce::feed;


/////////////////////////////////////////////////////////////////
FeedCachePrx ZhanFeedCacheAdapter::getManager(int id, int timeout) {
    return getProxy(id);
//	return locate<FeedCachePrx>(_managers, "M", id, TWO_WAY, timeout);
}

FeedCachePrx ZhanFeedCacheAdapter::getManagerOneway(int id) {
    return getProxy(id);
	//return locate<FeedCachePrx>(_managersOneway, "M", id, ONE_WAY);
}

void ZhanFeedCacheAdapter::put(const FeedItem& feed, const Int2IntMap& userWeight){
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
			MCE_WARN("ZhanFeedCacheAdapter::put --> invoke FeedCache err " << sIt->first << ", " << e);
		}
	}

}

FeedItemSeq ZhanFeedCacheAdapter::get(int userId){
	return getManager(userId)->get(userId);
}

void ZhanFeedCacheAdapter::remove(int userId, Ice::Long feedId){
	getManagerOneway(userId)->remove(userId, feedId);
}

void  ZhanFeedCacheAdapter::removeAll(int userId){
	getManagerOneway(userId)->removeAll(userId);
}

