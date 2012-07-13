#include <IceLogger.h>
#include "FeedCacheNReplicaAdapter.h"


using namespace xce::feed;


/////////////////////////////////////////////////////////////////
FeedCacheNPrx FeedCacheNReplicaAdapter::getManager(int id, int timeout) {
    return getProxy(id);
}

FeedCacheNPrx FeedCacheNReplicaAdapter::getManagerOneway(int id) {
    return getProxyOneway(id);
}

void FeedCacheNReplicaAdapter::put(const FeedItem& feed, const Int2IntMap& userWeight){
	map<int, map<int, int> > splits;
	MyUtil::Int2IntMap::const_iterator it = userWeight.begin();
	for(; it!=userWeight.end(); ++it){
    if(it->first > 0) {
      splits[it->first%getCluster()][it->first] = it->second; // 这里不模也可以
    }
	}
	
	map<int, map<int, int> >::iterator sIt = splits.begin();
	for(; sIt!=splits.end(); ++sIt){

    vector<FeedCacheNPrx> prxs = getAllProxySeqOneway(sIt->first);
    for( vector<FeedCacheNPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
      try {
        MCE_DEBUG("FeedCacheNReplicaAdapter::put --> prx: " << (*itPrx)->ice_toString() << " prxs size:" << prxs.size() << " fid:" << feed.feed << " uw size:" << sIt->second.size());
        (*itPrx)->put(feed, sIt->second);
      } catch( Ice::Exception e ){
        MCE_WARN("FeedCacheNReplicaAdapter::put --> invoke FeedCache err " << sIt->first << ", " << e);
      } catch (std::exception e) {
        MCE_WARN("FeedCacheNReplicaAdapter::put --> invoke FeedCache err " << sIt->first << ", " << e.what());
      } catch( ... ){
        MCE_WARN("FeedCacheNReplicaAdapter::put --> invoke FeedCache err " << sIt->first << " unknown exception! ");
      }
    }

	}

}


FeedItemSeq FeedCacheNReplicaAdapter::get(int userId){
	return getManager(userId)->get(userId);
}

FeedItemSeq FeedCacheNReplicaAdapter::getMore(int userId, Ice::Long maxFeed){
	return getManager(userId)->getMore(userId, maxFeed);
}

void FeedCacheNReplicaAdapter::modify(int userId, const FeedItem & item) {
    vector<FeedCacheNPrx> prxs = getAllProxySeqOneway(userId);
    for( vector<FeedCacheNPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
      try {
        (*itPrx)->modify(userId, item);;
      } catch( Ice::Exception e ){
        MCE_WARN("FeedCacheNReplicaAdapter::modify --> invoke FeedCache err " << userId << ", " << e);
      } catch (std::exception e) {
        MCE_WARN("FeedCacheNReplicaAdapter::modify --> invoke FeedCache err " << userId << ", " << e.what());
      } catch( ... ){
        MCE_WARN("FeedCacheNReplicaAdapter::modify --> invoke FeedCache err " << userId << " unknown exception! ");
      }
    }
}

void FeedCacheNReplicaAdapter::remove(int userId, Ice::Long feedId){
    vector<FeedCacheNPrx> prxs = getAllProxySeqOneway(userId);
    for( vector<FeedCacheNPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
      try {
        (*itPrx)->remove(userId, feedId);
      } catch( Ice::Exception e ){
        MCE_WARN("FeedCacheNReplicaAdapter::remove --> invoke FeedCache err " << userId << " feedId:" << feedId << ", " << e);
      } catch (std::exception e) {
        MCE_WARN("FeedCacheNReplicaAdapter::remove--> invoke FeedCache err " << userId  << " feedId:" << feedId << ", " << e.what());
      } catch( ... ){
        MCE_WARN("FeedCacheNReplicaAdapter::remove--> invoke FeedCache err " << userId  << " feedId:" << feedId << " unknown exception! ");
      }
    }
}

void  FeedCacheNReplicaAdapter::removeAll(int userId){
    vector<FeedCacheNPrx> prxs = getAllProxySeqOneway(userId);
    for( vector<FeedCacheNPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
      try {
        (*itPrx)->removeAll(userId);
      } catch( Ice::Exception e ){
        MCE_WARN("FeedCacheNReplicaAdapter::removeAll --> invoke FeedCache err " << userId << ", " << e);
      } catch (std::exception e) {
        MCE_WARN("FeedCacheNReplicaAdapter::removeAll --> invoke FeedCache err " << userId << ", " << e.what());
      } catch( ... ){
        MCE_WARN("FeedCacheNReplicaAdapter::removeAll --> invoke FeedCache err " << userId << " unknown exception! ");
      }
    }
}

MyUtil::LongSeq FeedCacheNReplicaAdapter::removeByMerge(int userId, Ice::Long merge, int stype) {
    
  MyUtil::LongSeq deletedIds;
  vector<FeedCacheNPrx> prxs = getAllProxySeq(userId);

  for( vector<FeedCacheNPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
    try {
      MyUtil::LongSeq tmp_seq = (*itPrx)->removeByMerge(userId, merge, stype);
      if(tmp_seq.size() > deletedIds.size()) {
        deletedIds = tmp_seq;
      }
    } catch( Ice::Exception e ){
      MCE_WARN("FeedCacheNReplicaAdapter::removeByMerge --> invoke FeedCache err " << userId  << " merge:" << merge << " stype:" << stype << ", " << e);
    } catch (std::exception e) {
      MCE_WARN("FeedCacheNReplicaAdapter::removeByMerge --> invoke FeedCache err " << userId << " merge:" << merge << " stype:" << stype << ", " << e.what());
    } catch( ... ){
      MCE_WARN("FeedCacheNReplicaAdapter::removeByMerge --> invoke FeedCache err " << userId << " merge:" << merge << " stype:" << stype << " unknown exception! ");
    }
  }
  return deletedIds;
}

MyUtil::LongSeq FeedCacheNReplicaAdapter::removeMergeByFeedId(int userId, Ice::Long feedId) {
  MyUtil::LongSeq deletedIds;
  vector<FeedCacheNPrx> prxs = getAllProxySeq(userId);

  for( vector<FeedCacheNPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
    try {
      MCE_DEBUG("FeedCacheNReplicaAdapter::removeMergeByFeedId --> prx: " << (*itPrx)->ice_toString() << " prxs size:" << prxs.size() << " uid:" << userId << " fid:" << feedId);
      MyUtil::LongSeq tmp_seq = (*itPrx)->removeMergeByFeedId(userId, feedId);
      if(tmp_seq.size() > deletedIds.size()) {
        deletedIds = tmp_seq;
      }
    } catch( Ice::Exception e ){
      MCE_WARN("FeedCacheNReplicaAdapter::removeMergeByFeedId --> invoke FeedCache err " << userId  << " feedId:" << feedId << ", " << e);
    } catch (std::exception e) {
      MCE_WARN("FeedCacheNReplicaAdapter::removeMergeByFeedId --> invoke FeedCache err " << userId  << " feedId:" << feedId << ", " << e.what());
    } catch( ... ){
      MCE_WARN("FeedCacheNReplicaAdapter::removeMergeByFeedId --> invoke FeedCache err " << userId  << " feedId:" << feedId << " unknown exception! ");
    }
  }
  return deletedIds;
}

void FeedCacheNReplicaAdapter::test(const FeedItem& feed, const Int2IntMap& userWeight){
  
}

