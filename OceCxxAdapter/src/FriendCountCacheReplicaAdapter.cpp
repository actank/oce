#include "FriendCountCacheReplicaAdapter.h"

using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= FriendCountCacheAdapter =================================

Ice::Int FriendCountCacheAdapter::getFriendCount(Ice::Int id){
	return getProxy(id)->getFriendCount(id);
}

MyUtil::Int2IntMap FriendCountCacheAdapter::getFriendCounts(const MyUtil::IntSeq& ids){
	if (ids.empty()){
		return MyUtil::Int2IntMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[FriendCountCacheAdapter::getFriendCounts] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return MyUtil::Int2IntMap();
	}
	if (numCluster == 0 || numCluster == 1) {
		try {
			return getProxy(ids.at(0))->getFriendCounts(ids);
		} catch (...) {
			return MyUtil::Int2IntMap();
		}
	} else {
		vector< vector<int> > seperatedIds;
		seperatedIds.resize(numCluster);
		for (MyUtil::IntSeq::const_iterator it = ids.begin(); it
				!= ids.end(); ++it) {
			seperatedIds.at(*it % numCluster).push_back(*it);
		}
		MyUtil::Int2IntMap result;
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			MyUtil::Int2IntMap someResult;
			if( seperatedIds.at(posCluster).size() > 0 ){
				try {
					someResult = getProxy(posCluster)->getFriendCounts(seperatedIds.at(posCluster));
				} catch (...) {
					MCE_WARN( "FriendCountCacheAdapter::getFriendCounts from Proxy(" << posCluster << ") handle unknown exception." );
				}
				result.insert( someResult.begin(), someResult.end() );
			}
		}
		return result;
	}
}

void FriendCountCacheAdapter::setFriendCount( Ice::Int id, Ice::Int count ){
        vector<FriendCountCacheManagerPrx> prxs = getAllProxySeq( id );
        for( vector<FriendCountCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
                        (*itPrx)->setFriendCount( id, count );
                } catch( Ice::Exception e ){
                        MCE_WARN("[FriendCountCacheAdapter::setFriendCount] id=" << id << " count=" << count << " found Ice::Exception : " << e.what() );
                        continue;
                } catch( ... ){
                        MCE_WARN("[FriendCountCacheAdapter::setFriendCount] id=" << id << " count=" << count << " found unkown exception" );
                        continue;
                }
        }
}

void FriendCountCacheAdapter::setFriendCounts( const MyUtil::Int2IntMap& counts ){
	if (counts.empty()){
		return;
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[FriendCountCacheAdapter::setFriendCounts] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return;
	}
	if (numCluster == 0 || numCluster == 1) {
		try {
			getProxy(0)->setFriendCounts(counts);
		} catch (...) {
			return;
		}
	} else {
		vector< MyUtil::Int2IntMap > seperatedObjs;
		seperatedObjs.resize(numCluster);
                for ( MyUtil::Int2IntMap::const_iterator it = counts.begin(); it!= counts.end(); ++it ){
			seperatedObjs.at(it->first % numCluster).insert( make_pair<Ice::Int,Ice::Int>(it->first,it->second) );
                }
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			if( seperatedObjs.at(posCluster).size() > 0 ){
				vector<FriendCountCacheManagerPrx> prxs = getAllProxySeq( posCluster );
				for( vector<FriendCountCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
					try {
						(*itPrx)->setFriendCounts( seperatedObjs.at(posCluster) );
					} catch( Ice::Exception e ){
						MCE_WARN("[FriendCountCacheAdapter::setFriendCounts] Proxy(" << posCluster << ") handle Ice::Exception : " << e.what() );
						continue;
					} catch( ... ){
						MCE_WARN("[FriendCountCacheAdapter::setFriendCounts] Proxy(" << posCluster << ") handle unkown exception : " );
						continue;
					}
				}
			}
		}
	}
}

void FriendCountCacheAdapter::dec( Ice::Int id ){
        vector<FriendCountCacheManagerPrx> prxs = getAllProxySeq( id );
        for( vector<FriendCountCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
                        (*itPrx)->dec( id );
                } catch( Ice::Exception e ){
                        MCE_WARN("[FriendCountCacheAdapter::dec] id=" << id << " found Ice::Exception : " << e.what() );
                        continue;
                } catch( ... ){
                        MCE_WARN("[FriendCountCacheAdapter::dec] id=" << id << " found unkown exception" );
                        continue;
                }
        }
}

void FriendCountCacheAdapter::reload( Ice::Int id ){
        vector<FriendCountCacheManagerPrx> prxs = getAllProxySeq( id );
        for( vector<FriendCountCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
                        (*itPrx)->reload( id );
                } catch( Ice::Exception e ){
                        MCE_WARN("[FriendCountCacheAdapter::reload] id=" << id << " found Ice::Exception : " << e.what() );
                        continue;
                } catch( ... ){
                        MCE_WARN("[FriendCountCacheAdapter::reload] id=" << id << " found unkown exception" );
                        continue;
                }
        }
}

