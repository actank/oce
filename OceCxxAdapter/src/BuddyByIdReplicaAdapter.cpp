#include "BuddyByIdReplicaAdapter.h"
#include "BuddyCheckHelper.h"

using namespace xce::buddy::common;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= BuddyByIdCacheAdapter =================================

Ice::Int BuddyByIdCacheAdapter::getFriendCount(Ice::Int userId){
	return getProxy(userId)->getFriendCount(userId);
}

MyUtil::Int2IntMap BuddyByIdCacheAdapter::getFriendCounts(const MyUtil::IntSeq& ids){
	if (ids.empty()){
		return MyUtil::Int2IntMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByIdCacheAdapter::getFriendCounts] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
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
					MCE_WARN( "BuddyByIdCacheAdapter::getFriendCounts from Proxy(" << posCluster << ") handle unknown exception." );
				}
				result.insert( someResult.begin(), someResult.end() );
			}
		}
		return result;
	}
}

MyUtil::IntSeq BuddyByIdCacheAdapter::getFriendListAsc(Ice::Int userId,Ice::Int limit){
	return getProxy(userId)->getFriendList(userId,limit);
}

MyUtil::IntSeq BuddyByIdCacheAdapter::getFriendListAsc(Ice::Int userId,Ice::Int begin, Ice::Int limit){
        return getProxy(userId)->getFriendListN(userId,begin,limit);
}

MyUtil::Int2IntSeqMap BuddyByIdCacheAdapter::getFriendLists(const MyUtil::IntSeq& ids){
	if (ids.empty()){
		return MyUtil::Int2IntSeqMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByIdCacheAdapter::getFriendLists] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return MyUtil::Int2IntSeqMap();
	}
	if (numCluster == 0 || numCluster == 1) {
		try {
			return getProxy(ids.at(0))->getFriendLists(ids);
		} catch (...) {
			return MyUtil::Int2IntSeqMap();
		}
	} else {
		vector< vector<int> > seperatedIds;
		seperatedIds.resize(numCluster);
		for (MyUtil::IntSeq::const_iterator it = ids.begin(); it
				!= ids.end(); ++it) {
			seperatedIds.at(*it % numCluster).push_back(*it);
		}
		MyUtil::Int2IntSeqMap result;
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			MyUtil::Int2IntSeqMap someResult;
			if( seperatedIds.at(posCluster).size() > 0 ){
				try {
					someResult = getProxy(posCluster)->getFriendLists(seperatedIds.at(posCluster));
				} catch (...) {
					MCE_WARN( "BuddyByIdCacheAdapter::getFriendLists from Proxy(" << posCluster << ") handle unknown exception." );
				}
				result.insert( someResult.begin(), someResult.end() );
			}
		}
		return result;
	}
}

void BuddyByIdCacheAdapter::load( Ice::Int id, const Ice::ObjectPtr& o ){
        vector<BuddyByIdCacheManagerPrx> prxs = getAllProxySeq( id );
        for( vector<BuddyByIdCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
                        (*itPrx)->load( id, o );
                } catch(Ice::TimeoutException e){ 
			MCE_WARN("[BuddyByIdCacheAdapter::load] id=" << id << " found Ice::TimeoutException : " << e.what() );
                        continue;
		} catch( Ice::Exception e ){
                        MCE_WARN("[BuddyByIdCacheAdapter::load] id=" << id << " found Ice::Exception : " << e.what() );
                        continue;
                } catch( ... ){
                        MCE_WARN("[BuddyByIdCacheAdapter::load] id=" << id << " found unkown exception : " );
                        continue;
                }
        }
}

void BuddyByIdCacheAdapter::remove( Ice::Int hostId, Ice::Int guestId ){
        vector<BuddyByIdCacheManagerPrx> prxs = getAllProxySeq( hostId );
        for( vector<BuddyByIdCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
                        (*itPrx)->remove( hostId, guestId );
                } catch(Ice::TimeoutException e){
                        MCE_WARN("[BuddyByIdCacheAdapter::remove] hostId=" << hostId << " guestId=" << guestId << " found Ice::TimeoutException : " << e.what() );
                        continue;
                } catch( Ice::Exception e ){
                        MCE_WARN("[BuddyByIdCacheAdapter::remove] hostId=" << hostId << " guestId=" << guestId << " found Ice::Exception : " << e.what() );
                        break;
                } catch( ... ){
                        MCE_WARN("[BuddyByIdCacheAdapter::remove] hostId=" << hostId << " guestId=" << guestId << " found unkown exception : " );
                        break;
                }
        }
}

void BuddyByIdCacheAdapter::setData( const MyUtil::ObjectResultPtr& datas ){
	if (datas->data.empty()){
		return;
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByIdCacheAdapter::setData] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return;
	}
	if (numCluster == 0 || numCluster == 1) {
		try {
			getProxy(0)->setData(datas);
		} catch (...) {
			return;
		}
	} else {
		vector< MyUtil::ObjectResultPtr > seperatedObjs;
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			seperatedObjs.push_back( new MyUtil::ObjectResult() );
		}
                for ( std::map<Ice::Long, Ice::ObjectPtr>::const_iterator it = datas->data.begin();
                        it!= datas->data.end(); ++it ){
			seperatedObjs.at(it->first % numCluster)->data.insert( make_pair<Ice::Long, Ice::ObjectPtr>(it->first,it->second) );
                }
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			if( seperatedObjs.at(posCluster)->data.size() > 0 ){
				vector<BuddyByIdCacheManagerPrx> prxs = getAllProxySeq( posCluster );
				for( vector<BuddyByIdCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
					try {
						(*itPrx)->setData( seperatedObjs.at(posCluster) );
					} catch(Ice::TimeoutException e){
                        			MCE_WARN("[BuddyByIdCacheAdapter::setData] Proxy(" << posCluster << ") handle Ice::TimeoutException : " << e.what() );
                        			continue;
                			} catch( Ice::Exception e ){
						MCE_WARN("[BuddyByIdCacheAdapter::setData] Proxy(" << posCluster << ") handle Ice::Exception : " << e.what() );
						continue;
					} catch( ... ){
						MCE_WARN("[BuddyByIdCacheAdapter::setData] Proxy(" << posCluster << ") handle unkown exception : " );
						continue;
					}
				}
			}
		}
	}
}

MyUtil::IntSeq BuddyByIdCacheAdapter::checkFriendCounts(const MyUtil::IntSeq& ids) {
	size_t numCluster_sizeT = getCluster();
	std::vector<std::vector<BuddyByIdCacheManagerPrx> > prxs;
	for(size_t i = 0; i < numCluster_sizeT; ++i) {
		prxs.push_back(getAllProxySeq(i));
	}
	BuddyCheckHelper<BuddyByIdCacheManagerPrx> buddyCheck(numCluster_sizeT, "BuddyByIdCacheAdapter", prxs);
	return buddyCheck.checkFriendCounts(ids);
}

// ========= BuddyByIdLoaderAdapter =================================

void BuddyByIdLoaderAdapter::load( Ice::Int hostId, const MyUtil::IntSeq& buddies ){
        MCE_DEBUG( "[BuddyByIdLoaderAdapter::load] hostId=" << hostId << " buddies.size()=" << buddies.size() );
        getProxy( hostId )->load( hostId, buddies );
}

void BuddyByIdLoaderAdapter::reload( Ice::Int id ){
        MCE_DEBUG( "[BuddyByIdLoaderAdapter::reload] id=" << id );
        getProxy( id )->reload( id );
}

void BuddyByIdLoaderAdapter::add( Ice::Int hostId, Ice::Int guestId ){
        MCE_DEBUG( "[BuddyByIdLoaderAdapter::add] hostId=" << hostId << ", guestId=" << guestId );
        getProxy( hostId )->add( hostId, guestId );
}
void BuddyByIdLoaderAdapter::statusUpdate( Ice::Int id, Ice::Int status ){
        MCE_DEBUG( "[BuddyByIdLoaderAdapter::statusUpdate] id=" << id << ", status=" << status );
        getProxy( id )->statusUpdate( id, status );
}
int BuddyByIdLoaderAdapter::filter( Ice::Int hostId, const MyUtil::IntSeq& buddies ){
        MCE_DEBUG( "[BuddyByIdLoaderAdapter::filter] hostId=" << hostId << " buddies.size()=" << buddies.size() );
        try{
		return getProxy( hostId )->filter( hostId, buddies );
	}catch(Ice::TimeoutException e){
		MCE_WARN("[BuddyByIdLoaderAdapter::filter]  handle Ice::TimeoutException : " << e.what() );
	} catch( Ice::Exception e ){
		MCE_WARN("[BuddyByIdLoaderAdapter::filter]  handle Ice::Exception : " << e.what() );
	} catch( ... ){
		MCE_WARN("[BuddyByIdLoaderAdapter::filter]  handle unkown exception : " );
	}
	return 0;
}

void BuddyByIdLoaderAdapter::getAllStatus(std::bitset<STATUS_CACHE_SIZE>& status, bool& success) {
	MCE_INFO("[BuddyByIdLoaderAdapter::getAllStatus]");
	success = true;
	MyUtil::ByteSeq result;
	try {
		for (int i=0;i<210;i++) {
			MyUtil::ByteSeq temp = getProxy(0)->getStatusMinMax(i*10000000, (i+1)*10000000);
			if(temp.size() == 0) {
				success = false;
				return;
			}
			result.insert(result.end(), temp.begin(), temp.end());
		}
		MyUtil::ByteSeq temp=getProxy(0)->getStatusMinMax(2100000000, STATUS_CACHE_SIZE);
		if(temp.size() == 0) {
			success = false;
			return;
		}
		result.insert(result.end(), temp.begin(), temp.end());
	}catch(Ice::TimeoutException e){
		MCE_WARN("[BuddyByIdLoaderAdapter::getAllStatus]  handle Ice::TimeoutException : " << e.what() );
		success = false;
		return;
	} catch( Ice::Exception e ){
		MCE_WARN("[BuddyByIdLoaderAdapter::getAllStatus]  handle Ice::Exception : " << e.what() );
		success = false;
		return;
	} catch( ... ){
		MCE_WARN("[BuddyByIdLoaderAdapter::getAllStatus]  handle unkown exception : " );
		success = false;
		return;
	}
	
	for(int i=0;i<STATUS_CACHE_SIZE;++i) {
		if (result[i/8]&(1<<(i%8))) {
			status.set(i);
		}
	}
}
