#include "BuddyByAddTimeReplicaAdapter.h"
#include "BuddyCheckHelper.h"

using namespace xce::buddy;
using namespace xce::buddy::common;
using namespace xce::buddy::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= BuddyByAddTimeCacheAdapter =================================

Ice::Int BuddyByAddTimeCacheAdapter::getFriendCount(Ice::Int userId){
	return getProxy(userId)->getFriendCount(userId);
}

MyUtil::Int2IntMap BuddyByAddTimeCacheAdapter::getFriendCounts(const MyUtil::IntSeq& ids){
	if (ids.empty()){
		return MyUtil::Int2IntMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByAddTimeCacheAdapter::getFriendCounts] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
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
					MCE_WARN( "BuddyByAddTimeCacheAdapter::getFriendCounts from Proxy(" << posCluster << ") handle unknown exception." );
				}
				result.insert( someResult.begin(), someResult.end() );
			}
		}
		return result;
	}
}

MyUtil::IntSeq BuddyByAddTimeCacheAdapter::getFriendList(Ice::Int userId,Ice::Int limit){
	return getProxy(userId)->getFriendList(userId,limit);
}

MyUtil::IntSeq BuddyByAddTimeCacheAdapter::getFriendList(Ice::Int userId,Ice::Int begin,Ice::Int limit){
        return getProxy(userId)->getFriendListN(userId,begin,limit);
}

MyUtil::Int2IntSeqMap BuddyByAddTimeCacheAdapter::getFriendLists(const MyUtil::IntSeq& ids){
	if (ids.empty()){
		return MyUtil::Int2IntSeqMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByAddTimeCacheAdapter::getFriendLists] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
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
                seperatedIds.resize( numCluster );
                for( MyUtil::IntSeq::const_iterator it = ids.begin(); it
                                != ids.end(); ++it ){
                        seperatedIds.at( *it % numCluster ).push_back( *it );
                }
                MyUtil::Int2IntSeqMap result;
                for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
                        MyUtil::Int2IntSeqMap someResult;
                        if( seperatedIds.at(posCluster).size() > 0 ){
                                try {
                                        someResult = getProxy(posCluster)->getFriendLists(seperatedIds.at(posCluster));
                                } catch (...) {
                                        MCE_WARN( "BuddyByAddTimeCacheAdapter::getFriendLists from Proxy(" << posCluster << ") handle unknown exception." );
                                }
                                result.insert( someResult.begin(), someResult.end() );
                        }
                }
                return result;
	}
}

/*
void BuddyByAddTimeCacheAdapter::load( Ice::Int id, const Ice::ObjectPtr& o, const std::string& times ){
        vector<BuddyByAddTimeCacheManagerPrx> prxs = getAllProxySeq( id );
        for( vector<BuddyByAddTimeCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
			if( times != "" ){
				Ice::Context ctx;
				ctx["TIMES"] = times;
				(*itPrx)->load( id, o, ctx );
			} else{
				(*itPrx)->load( id, o );
			}
                } catch( Ice::Exception e ){
                        MCE_WARN("[BuddyByAddTimeCacheAdapter::load] id=" << id << " found Ice::Exception : " << e.what() );
                        continue;
                } catch( ... ){
                        MCE_WARN("[BuddyByAddTimeCacheAdapter::load] id=" << id << " found unkown exception : " );
                        continue;
                }
        }
}
*/

void BuddyByAddTimeCacheAdapter::setData( const MyUtil::ObjectResultPtr& datas ){
	MCE_INFO("[BuddyByAddTimeAdapter::setData] total data size = " << datas->data.size());
        if (datas->data.empty()){
                return;
        }
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByAddTimeCacheAdapter::setData] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
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
                        seperatedObjs.at(it->first % numCluster)->data.insert(make_pair<Ice::Long, Ice::ObjectPtr>(it->first,it->second));
                }
                for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
                        if( seperatedObjs.at(posCluster)->data.size() > 0 ){
                                vector<BuddyByAddTimeCacheManagerPrx> prxs = getAllProxySeq( posCluster );
                                for( vector<BuddyByAddTimeCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
										ostringstream info;
										for (std::map<long, Ice::ObjectPtr>::const_iterator ui = seperatedObjs.at(posCluster)->data.begin();
											ui != seperatedObjs.at(posCluster)->data.end(); ++ ui) {
											info << ui->first << " ";
										}
                                        try {
											MCE_INFO("[BuddyByAddTimeAdapter::setData] data size = " << seperatedObjs.at(posCluster)->data.size() << 
													" posCluster = " << posCluster);
                                            (*itPrx)->setData( seperatedObjs.at(posCluster) );
                                        } catch( Ice::Exception e ){
                                                MCE_WARN("[BuddyByAddTimeCacheAdapter::setData] Proxy(" << posCluster << ") handle Ice::Exception : " << e.what() << " ids: " << info.str());
                                                continue;
                                        } catch( ... ){
                                                MCE_WARN("[BuddyByAddTimeCacheAdapter::setData] Proxy(" << posCluster << ") handle unkown exception : " << info.str());
                                                continue;
                                        }
                                }
                        }
                }
        }
}

void BuddyByAddTimeCacheAdapter::remove( Ice::Int hostId, Ice::Int guestId ){
        vector<BuddyByAddTimeCacheManagerPrx> prxs = getAllProxySeq( hostId );
        for( vector<BuddyByAddTimeCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
                        (*itPrx)->remove( hostId, guestId );
                } catch( Ice::Exception e ){
                        MCE_WARN("[BuddyByAddTimeCacheAdapter::remove] hostId=" << hostId << " guestId=" << guestId << " found Ice::Exception : " << e.what() );
                        break;
                } catch( ... ){
                        MCE_WARN("[BuddyByAddTimeCacheAdapter::remove] hostId=" << hostId << " guestId=" << guestId << " found unkown exception : " );
                        break;
                }
        }
}

MyUtil::IntSeq BuddyByAddTimeCacheAdapter::checkFriendCounts(const MyUtil::IntSeq& ids) {
	size_t numCluster_sizeT = getCluster();
	std::vector<std::vector<BuddyByAddTimeCacheManagerPrx> > prxs;
	for(size_t i = 0; i < numCluster_sizeT; ++i) {
		prxs.push_back(getAllProxySeq(i));
	}
	BuddyCheckHelper<BuddyByAddTimeCacheManagerPrx> buddyCheck(numCluster_sizeT, "BuddyByNameCacheAdapter", prxs);
	return buddyCheck.checkFriendCounts(ids);
}

// ========= BuddyByAddTimeLoaderAdapter =================================

void BuddyByAddTimeLoaderAdapter::reload( Ice::Int id ){
        MCE_DEBUG( "[BuddyByAddTimeLoaderAdapter::reload] id=" << id );
        getProxy( id )->reload( id );
}

void BuddyByAddTimeLoaderAdapter::load( const MyUtil::Int2IntSeqMap& lists ){
        MCE_DEBUG( "[BuddyByAddTimeLoaderAdapter::load] lists.size()=" << lists.size() );
	try {
	        getProxy( 0 )->load( lists );
	} catch (Ice::Exception& e) {
		ostringstream info;
		for (MyUtil::Int2IntSeqMap::const_iterator ui = lists.begin(); ui != lists.end(); ++ui) {
			info << ui->first << " ";
		}
		MCE_WARN("BuddyByAddTimeLoaderAdapter::load, Ice::Exception: " << e.what() << " ids: " << info.str());
	} catch (std::exception& e) {
		MCE_WARN("BuddyByAddTimeLoaderAdapter::load,Exception: " << e.what());
	} catch (...) {
		MCE_WARN("BuddyByAddTimeLoaderAdapter::load,Exception");
	}
}

