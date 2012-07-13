#include "BuddyByNameReplicaAdapter.h"
#include "BuddyCheckHelper.h"

using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;
using namespace xce::buddy::common;

// ========= BuddyByNameCacheAdapter =================================

Ice::Int BuddyByNameCacheAdapter::getFriendCount( Ice::Int userId ){
	return getProxy( userId )->getFriendCount( userId );
}

MyUtil::Int2IntMap BuddyByNameCacheAdapter::getFriendCounts(const MyUtil::IntSeq& ids){
	if (ids.empty()){
		return MyUtil::Int2IntMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByNameCacheAdapter::getFriendCounts] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
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
					MCE_WARN( "BuddyByNameCacheAdapter::getFriendCounts from Proxy(" << posCluster << ") handle unknown exception." );
				}
				result.insert( someResult.begin(), someResult.end() );
			}
		}
		return result;
	}
}

MyUtil::IntSeq BuddyByNameCacheAdapter::checkFriendCounts(const MyUtil::IntSeq& ids){
	size_t numCluster_sizeT = getCluster();
	std::vector<std::vector<BuddyByNameCacheManagerPrx> > prxs;
	for(size_t i = 0; i < numCluster_sizeT; ++i) {
		prxs.push_back(getAllProxySeq(i));
	}
	BuddyCheckHelper<BuddyByNameCacheManagerPrx> buddyCheck(numCluster_sizeT, "BuddyByNameCacheAdapter", prxs);
	return buddyCheck.checkFriendCounts(ids);
}

MyUtil::IntSeq BuddyByNameCacheAdapter::getFriendList( Ice::Int userId, Ice::Int limit ){
	return getProxy( userId )->getFriendList( userId, limit );
}

MyUtil::IntSeq BuddyByNameCacheAdapter::getFriendList( Ice::Int userId, Ice::Int begin, Ice::Int limit ){
        return getProxy( userId )->getFriendListN( userId, begin, limit );
}

MyUtil::Int2IntSeqMap BuddyByNameCacheAdapter::getFriendLists( const MyUtil::IntSeq& ids ){
	if( ids.empty() ){
		return MyUtil::Int2IntSeqMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByNameCacheAdapter::getFriendLists] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return MyUtil::Int2IntSeqMap();
	}
	if( numCluster == 0 || numCluster == 1 ){
		try {
			return getProxy( ids.at(0) )->getFriendLists( ids );
		} catch( ... ){
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
                                        MCE_WARN( "BuddyByNameCacheAdapter::getFriendLists from Proxy(" << posCluster << ") handle unknown exception." );
                                }
                                result.insert( someResult.begin(), someResult.end() );
                        }
                }
                return result;
	}
}

void BuddyByNameCacheAdapter::load( Ice::Int id, const Ice::ObjectPtr& o ){
        vector<BuddyByNameCacheManagerPrx> prxs = getAllProxySeq( id );
        for( vector<BuddyByNameCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
                        (*itPrx)->load( id, o );
                } catch( Ice::Exception e ){
                        MCE_WARN("[BuddyByNameCacheAdapter::load] id=" << id << " found Ice::Exception : " << e.what() );
                        continue;
                } catch( ... ){
                        MCE_WARN("[BuddyByNameCacheAdapter::load] id=" << id << " found unkown exception : " );
                        continue;
                }
        }
}

void BuddyByNameCacheAdapter::setData( const MyUtil::ObjectResultPtr& datas ){
	MCE_INFO("[BuddyByNameAdapter::setData] total data size = " << datas->data.size());
        if (datas->data.empty()){
                return;
        }
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByNameCacheAdapter::setData] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
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
                                vector<BuddyByNameCacheManagerPrx> prxs = getAllProxySeq( posCluster );
                                for( vector<BuddyByNameCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
										ostringstream info;
										for (std::map<long, Ice::ObjectPtr>::const_iterator ui = seperatedObjs.at(posCluster)->data.begin();
											ui != seperatedObjs.at(posCluster)->data.end(); ++ ui) {
											info << ui->first << " ";
										}
                                        try {
												MCE_INFO("[BuddyByNameAdapter::setData] data size = " << seperatedObjs.at(posCluster)->data.size() << " posCluster = " << posCluster);
                                                (*itPrx)->setData( seperatedObjs.at(posCluster) );
                                        } catch( Ice::Exception e ){
                                                MCE_WARN("[BuddyByNameCacheAdapter::setData] Proxy(" << posCluster << ") handle Ice::Exception : " << e.what() << info.str());
                                                continue;
                                        } catch( ... ){
                                                MCE_WARN("[BuddyByNameCacheAdapter::setData] Proxy(" << posCluster << ") handle unkown exception : " << info.str());
                                                continue;
                                        }
                                }
                        }
                }
        }
}

void BuddyByNameCacheAdapter::remove( Ice::Int hostId, Ice::Int guestId ){
        vector<BuddyByNameCacheManagerPrx> prxs = getAllProxySeq( hostId );
        for( vector<BuddyByNameCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
                        (*itPrx)->remove( hostId, guestId );
                } catch( Ice::Exception e ){
                        MCE_WARN("[BuddyByNameCacheAdapter::remove] hostId=" << hostId << " guestId=" << guestId << " found Ice::Exception : " << e.what() );
                        break;
                } catch( ... ){
                        MCE_WARN("[BuddyByNameCacheAdapter::remove] hostId=" << hostId << " guestId=" << guestId << " found unkown exception : " );
                        break;
                }
        }
}

// ========= BuddyByNameLoaderAdapter =================================

void BuddyByNameLoaderAdapter::reload( Ice::Int id ){
	MCE_DEBUG( "[BuddyByNameLoaderAdapter::reload] id=" << id );
	getProxy( id )->reload( id );
}

void BuddyByNameLoaderAdapter::load( const MyUtil::Int2IntSeqMap& lists ){
	MCE_DEBUG( "[BuddyByNameLoaderAdapter::load] lists.size()=" << lists.size() );
	getProxy( 0 )->load( lists );
}

void BuddyByNameLoaderAdapter::rename( Ice::Int theRenameOne, const std::string& name ){
	MCE_DEBUG( "[BuddyByNameLoaderAdapter::rename] theRenameOne=" << theRenameOne );
	getProxy( theRenameOne )->rename( theRenameOne,name );
}

