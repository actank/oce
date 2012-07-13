#include "BuddyByOnlineTimeReplicaAdapter.h"
#include "BuddyCheckHelper.h"

using namespace xce::buddy;
using namespace xce::buddy::common;
using namespace xce::buddy::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= BuddyByOnlineTimeCacheAdapter =================================

Ice::Int BuddyByOnlineTimeCacheAdapter::getFriendCount( Ice::Int userId ){
	return getProxy( userId )->getFriendCount( userId );
}

MyUtil::Int2IntMap BuddyByOnlineTimeCacheAdapter::getFriendCounts(const MyUtil::IntSeq& ids){
	if (ids.empty()){
		return MyUtil::Int2IntMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByOnlineTimeCacheAdapter::getFriendCounts] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
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
					MCE_WARN( "BuddyByOnlineTimeCacheAdapter::getFriendCounts from Proxy(" << posCluster << ") handle unknown exception." );
				}
				result.insert( someResult.begin(), someResult.end() );
			}
		}
		return result;
	}
}

MyUtil::IntSeq BuddyByOnlineTimeCacheAdapter::getFriendList( Ice::Int userId, Ice::Int limit ){
	return getProxy( userId )->getFriendList( userId, limit );
}

MyUtil::IntSeq BuddyByOnlineTimeCacheAdapter::getFriendList( Ice::Int userId, Ice::Int begin, Ice::Int limit ){
        return getProxy( userId )->getFriendListN( userId, begin, limit );
}

MyUtil::Int2IntSeqMap BuddyByOnlineTimeCacheAdapter::getFriendLists( const MyUtil::IntSeq& ids ){
	if( ids.empty() ){
		return MyUtil::Int2IntSeqMap();
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByOnlineTimeCacheAdapter::getFriendLists] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
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
                                        MCE_WARN( "BuddyByOnlineTimeCacheAdapter::getFriendLists from Proxy(" << posCluster << ") handle unknown exception." );
                                }
                                result.insert( someResult.begin(), someResult.end() );
                        }
                }
                return result;
	}
}

void BuddyByOnlineTimeCacheAdapter::notifySomebodyOnline( Ice::Int userId, Ice::Int onlineId ){
        vector<BuddyByOnlineTimeCacheManagerPrx> prxs = getAllProxySeq( userId );
        for( vector<BuddyByOnlineTimeCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		try {
                	(*itPrx)->notifySomebodyOnline( userId, onlineId );
		} catch( Ice::Exception e ){
			MCE_WARN("[BuddyByOnlineTimeCacheAdapter::notifySomebodyOnline("<<userId<<","<<onlineId<<"] found Ice::Exception : " << e.what() );
			break;
		} catch( ... ){
			MCE_WARN("[BuddyByOnlineTimeCacheAdapter::notifySomebodyOnline("<<userId<<","<<onlineId<<"] found unknown exception !" );
			break;
		}
        }
}

void BuddyByOnlineTimeCacheAdapter::notifyOnlineBatch( const OnlineNoteSeq& notes ){
	if( notes.empty() ){
		return;
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByOnlineTimeCacheAdapter::notifyOnlineBatch] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return;
	}
	if( numCluster == 0 || numCluster == 1 ){
		vector<BuddyByOnlineTimeCacheManagerPrx> prxs = getAllProxySeq( 0 );
		for( vector<BuddyByOnlineTimeCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
			try {
				(*itPrx)->notifyOnlineBatch( notes );
			} catch( Ice::Exception e ){
                        	MCE_WARN("[BuddyByOnlineTimeCacheAdapter::notifyOnlineBatch] found Ice::Exception : " << e.what() );
				break;
			} catch( ... ){
                        	MCE_WARN("[BuddyByOnlineTimeCacheAdapter::notifyOnlineBatch] found unknown exception !" );
				break;
			}
		}
	} else {
		vector< OnlineNoteSeq > seperatedNotes;
		seperatedNotes.resize( numCluster );
		for( OnlineNoteSeq::const_iterator it = notes.begin(); it
				!= notes.end(); ++it ){
			seperatedNotes.at( it->userId % numCluster ).push_back( *it );
		}
		for( int posCluster = 0; posCluster < numCluster; ++posCluster ){
			vector<BuddyByOnlineTimeCacheManagerPrx> prxs = getAllProxySeq( posCluster );
			for( vector<BuddyByOnlineTimeCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
				try {
					(*itPrx)->notifyOnlineBatch( seperatedNotes.at( posCluster ) );
				} catch( Ice::Exception e ){
					MCE_WARN("[BuddyByOnlineTimeCacheAdapter::notifyOnlineBatch] found Ice::Exception : " << e.what() );
					break;
				} catch( ... ){
					MCE_WARN("[BuddyByOnlineTimeCacheAdapter::notifyOnlineBatch] found unknown exception !" );
					break;
				}
			}
		}
	}
}

void BuddyByOnlineTimeCacheAdapter::load( Ice::Int id, const Ice::ObjectPtr& o ){
        vector<BuddyByOnlineTimeCacheManagerPrx> prxs = getAllProxySeq( id );
        for( vector<BuddyByOnlineTimeCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		try {
			(*itPrx)->load( id, o );
		} catch( Ice::Exception e ){
			MCE_WARN("[BuddyByOnlineTimeCacheAdapter::load] id=" << id << " found Ice::Exception : " << e.what() );
			break;
		} catch( ... ){
			MCE_WARN("[BuddyByOnlineTimeCacheAdapter::load] id=" << id << " found unkown exception : " );
			break;
		}
        }
}

void BuddyByOnlineTimeCacheAdapter::setData( const MyUtil::ObjectResultPtr& datas ){
	MCE_INFO("[BuddyByOnlineTimeAdapter::setData] total data size = " << datas->data.size());
        if (datas->data.empty()){
                return;
        }
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[BuddyByOnlineTimeCacheAdapter::setData] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
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
                                vector<BuddyByOnlineTimeCacheManagerPrx> prxs = getAllProxySeq( posCluster );
                                for( vector<BuddyByOnlineTimeCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
										ostringstream info;
										for (std::map<long, Ice::ObjectPtr>::const_iterator ui = seperatedObjs.at(posCluster)->data.begin();
											ui != seperatedObjs.at(posCluster)->data.end(); ++ ui) {
											info << ui->first << " ";
										}
                                        try {
												MCE_INFO("[BuddyByOnlineTimeAdapter::setData] data size = " << seperatedObjs.at(posCluster)->data.size()
															 << " posCluster = " << posCluster);
                                                (*itPrx)->setData( seperatedObjs.at(posCluster) );
                                        } catch( Ice::Exception e ){
                                        	MCE_WARN("[BuddyByOnlineTimeCacheAdapter::setData] Proxy(" << posCluster << ") handle Ice::Exception : " << e.what() << info.str());
                                                continue;
                                        } catch( ... ){
                                                MCE_WARN("[BuddyByOnlineTimeCacheAdapter::setData] Proxy(" << posCluster << ") handle unkown exception : " << info.str());
                                                continue;
                                        }
                                }
                        }
                }
        }
}

void BuddyByOnlineTimeCacheAdapter::remove( Ice::Int hostId, Ice::Int guestId ){
        vector<BuddyByOnlineTimeCacheManagerPrx> prxs = getAllProxySeq( hostId );
        for( vector<BuddyByOnlineTimeCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		try {
			(*itPrx)->remove( hostId, guestId );
		} catch( Ice::Exception e ){
			MCE_WARN("[BuddyByOnlineTimeCacheAdapter::remove] hostId=" << hostId << " guestId=" << guestId << " found Ice::Exception : " << e.what() );
			break;
		} catch( ... ){
			MCE_WARN("[BuddyByOnlineTimeCacheAdapter::remove] hostId=" << hostId << " guestId=" << guestId << " found unkown exception : " );
			break;
		}
        }
}

MyUtil::IntSeq BuddyByOnlineTimeCacheAdapter::checkFriendCounts(const MyUtil::IntSeq& ids) {
	size_t numCluster_sizeT = getCluster();
	std::vector<std::vector<BuddyByOnlineTimeCacheManagerPrx> > prxs;
	for(size_t i = 0; i < numCluster_sizeT; ++i) {
		prxs.push_back(getAllProxySeq(i));
	}
	BuddyCheckHelper<BuddyByOnlineTimeCacheManagerPrx> buddyCheck(numCluster_sizeT, "BuddyByNameCacheAdapter", prxs);
	return buddyCheck.checkFriendCounts(ids);
}

// ========= BuddyByOnlineTimeLoaderAdapter =================================

void BuddyByOnlineTimeLoaderAdapter::reload( Ice::Int id ){
        MCE_DEBUG( "[BuddyByOnlineTimeLoaderAdapter::reload] id=" << id );
        getProxy( id )->reload( id );
}

void BuddyByOnlineTimeLoaderAdapter::load( const MyUtil::Int2IntSeqMap& lists ){
        MCE_DEBUG( "[BuddyByOnlineTimeLoaderAdapter::load] lists.size()=" << lists.size() );
        getProxy( 0 )->load( lists );
}

void BuddyByOnlineTimeLoaderAdapter::online( Ice::Int theOnlineOne ){
	MCE_DEBUG( "[BuddyByOnlineTimeLoaderAdapter::online] theOnlineOne=" << theOnlineOne );
        getProxy( theOnlineOne )->online( theOnlineOne );
}

