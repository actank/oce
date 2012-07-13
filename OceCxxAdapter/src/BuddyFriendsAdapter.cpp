#include "BuddyFriendsAdapter.h"

using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= BuddyFriendsCacheAdapter =================================

Ice::Int BuddyFriendsCacheAdapter::getFriendCount(Ice::Int userId){
	return getProxy(userId)->getFriendCount(userId);
}

MyUtil::Int2IntMap BuddyFriendsCacheAdapter::getFriendCounts(const MyUtil::IntSeq& ids){
	if (ids.empty()){
		return MyUtil::Int2IntMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyFriendsCacheAdapter::getFriendCounts] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
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
					MCE_WARN( "BuddyFriendsCacheAdapter::getFriendCounts from Proxy(" << posCluster << ") handle unknown exception." );
				}
				result.insert( someResult.begin(), someResult.end() );
			}
		}
		return result;
	}
}

MyUtil::IntSeq BuddyFriendsCacheAdapter::getFriendListAsc(Ice::Int userId,Ice::Int begin, Ice::Int limit){
	return getProxy(userId)->getFriendList(userId,begin,limit);
}

bool BuddyFriendsCacheAdapter::isFriendsFriend(int hostId, int guestId) {
	try{ 
		return getProxy(hostId)->isFriendsFriend(hostId, guestId);
	}catch (...) {
		MCE_WARN( "BuddyFriendsCacheAdapter::isFriendsFriend handle unknown exception." );
	}
	return false;
}

MyUtil::Int2IntMap BuddyFriendsCacheAdapter::isFriendsFriendBatch(int hostId, const MyUtil::IntSeq& ids) {
	try{ 
		return getProxy(hostId)->isFriendsFriendBatch(hostId, ids);
	}catch (...) {
		MCE_WARN( "BuddyFriendsCacheAdapter::isFriendsFriendBatch handle unknown exception." );
	}
	return MyUtil::Int2IntMap();
}

MyUtil::IntSeq BuddyFriendsCacheAdapter::getCommonFriends(int hostId, int guestId) {
	try{
		return getProxy(hostId)->getCommonFriends(hostId, guestId);
	}catch (...) {
		MCE_WARN( "BuddyFriendsCacheAdapter::getCommonFriends handle unknown exception." );
	}
	return MyUtil::IntSeq();
}


MyUtil::Str2IntSeqMap BuddyFriendsCacheAdapter::getCommonFriendsBatch( MyUtil::Int2IntMap& idpairs ){
	try{
		return getProxy(0)->getCommonFriendsBatch(idpairs);
	}catch (...) {
		MCE_WARN( "BuddyFriendsCacheAdapter::getCommonFriendsBatch handle unknown exception." );
	}
	return MyUtil::Str2IntSeqMap();

}

MyUtil::Int2IntSeqMap BuddyFriendsCacheAdapter::getFriendLists(const MyUtil::IntSeq& ids){
	if (ids.empty()){
		return MyUtil::Int2IntSeqMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyFriendsCacheAdapter::getFriendLists] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
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
					MCE_WARN( "BuddyFriendsCacheAdapter::getFriendLists from Proxy(" << posCluster << ") handle unknown exception." );
				}
				result.insert( someResult.begin(), someResult.end() );
			}
		}
		return result;
	}
}

void BuddyFriendsCacheAdapter::load( Ice::Int id, const Ice::ObjectPtr& o ){
	vector<BuddyFriendsCacheManagerPrx> prxs = getAllProxySeq( id );
	for( vector<BuddyFriendsCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		try {
			(*itPrx)->load( id, o );
		} catch(Ice::TimeoutException e){ 
			MCE_WARN("[BuddyFriendsCacheAdapter::load] id=" << id << " found Ice::TimeoutException : " << e.what() );
			continue;
		} catch( Ice::Exception e ){
			MCE_WARN("[BuddyFriendsCacheAdapter::load] id=" << id << " found Ice::Exception : " << e.what() );
			continue;
		} catch( ... ){
			MCE_WARN("[BuddyFriendsCacheAdapter::load] id=" << id << " found unkown exception : " );
			continue;
		}
	}
}

void BuddyFriendsCacheAdapter::add( Ice::Int hostId, Ice::Int guestId ){
	vector<BuddyFriendsCacheManagerPrx> prxs = getAllProxySeq( hostId );
	for( vector<BuddyFriendsCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		try {
			(*itPrx)->add( hostId, guestId );
		} catch(Ice::TimeoutException e){
			MCE_WARN("[BuddyFriendsCacheAdapter::add] hostId=" << hostId << " guestId=" << guestId << " found Ice::TimeoutException : " << e.what() );
			continue;
		} catch( Ice::Exception e ){
			MCE_WARN("[BuddyFriendsCacheAdapter::add] hostId=" << hostId << " guestId=" << guestId << " found Ice::Exception : " << e.what() );
			break;
		} catch( ... ){
			MCE_WARN("[BuddyFriendsCacheAdapter::add] hostId=" << hostId << " guestId=" << guestId << " found unkown exception : " );
			break;
		}
	}
}


void BuddyFriendsCacheAdapter::remove( Ice::Int hostId, Ice::Int guestId ){
	vector<BuddyFriendsCacheManagerPrx> prxs = getAllProxySeq( hostId );
	for( vector<BuddyFriendsCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		try {
			(*itPrx)->remove( hostId, guestId );
		} catch(Ice::TimeoutException e){
			MCE_WARN("[BuddyFriendsCacheAdapter::remove] hostId=" << hostId << " guestId=" << guestId << " found Ice::TimeoutException : " << e.what() );
			continue;
		} catch( Ice::Exception e ){
			MCE_WARN("[BuddyFriendsCacheAdapter::remove] hostId=" << hostId << " guestId=" << guestId << " found Ice::Exception : " << e.what() );
			break;
		} catch( ... ){
			MCE_WARN("[BuddyFriendsCacheAdapter::remove] hostId=" << hostId << " guestId=" << guestId << " found unkown exception : " );
			break;
		}
	}
}

void BuddyFriendsCacheAdapter::setData( const MyUtil::ObjectResultPtr& datas ){
	if (datas->data.empty()){
		return;
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyFriendsCacheAdapter::setData] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
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
				vector<BuddyFriendsCacheManagerPrx> prxs = getAllProxySeq( posCluster );
				for( vector<BuddyFriendsCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
					try {
						(*itPrx)->setData( seperatedObjs.at(posCluster) );
					} catch(Ice::TimeoutException e){
						MCE_WARN("[BuddyFriendsCacheAdapter::setData] Proxy(" << posCluster << ") handle Ice::TimeoutException : " << e.what() );
						continue;
					} catch( Ice::Exception e ){
						MCE_WARN("[BuddyFriendsCacheAdapter::setData] Proxy(" << posCluster << ") handle Ice::Exception : " << e.what() );
						continue;
					} catch( ... ){
						MCE_WARN("[BuddyFriendsCacheAdapter::setData] Proxy(" << posCluster << ") handle unkown exception : " );
						continue;
					}
				}
			}
		}
	}
}

// ========= BuddyFriendsLoaderAdapter =================================

void BuddyFriendsLoaderAdapter::load( Ice::Int hostId, const MyUtil::IntSeq& buddies ){
	MCE_DEBUG( "[BuddyFriendsLoaderAdapter::load] hostId=" << hostId << " buddies.size()=" << buddies.size() );
	getProxy( hostId )->load( hostId, buddies );
}

void BuddyFriendsLoaderAdapter::reload( Ice::Int id ){
	MCE_DEBUG( "[BuddyFriendsLoaderAdapter::reload] id=" << id );
	getProxy( id )->reload( id );
}

void BuddyFriendsLoaderAdapter::add( Ice::Int hostId, Ice::Int guestId ){
	MCE_DEBUG( "[BuddyFriendsLoaderAdapter::add] hostId=" << hostId << ", guestId=" << guestId );
	getProxy( hostId )->add( hostId, guestId );
}
