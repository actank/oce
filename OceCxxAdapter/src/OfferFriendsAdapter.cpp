#include "OfferFriendsAdapter.h"

using namespace xce::offerfriends::adapter;
using namespace xce::offerfriends;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= OfferFriendsCacheAdapter =================================

OfferDataNSeq OfferFriendsCacheAdapter::getFastOffer(int userId, int limit) {
	return getProxy(userId)->getFastOffer(userId, limit);
}

OfferDataNSeq OfferFriendsCacheAdapter::getBestOffer(int userId, int limit) {
	return getProxy(userId)->getBestOffer(userId, limit);
}

OfferDataNSeq OfferFriendsCacheAdapter::getBestOfferByStage(int stage, int userId, int limit) {
	return getProxy(userId)->getBestOfferByStage(stage, userId, limit);;
}

OfferDataNSeq OfferFriendsCacheAdapter::getRandomOffer(int userId, int limit) {
	return getProxy(userId)->getRandomOffer(userId, limit);
}

RmInfo OfferFriendsCacheAdapter::apply(int applicant,int acceptor) {
	RmInfo res;
	res.userId = applicant;
	res.hit = false;
	res.found = false;
	res.data = OfferDataN();
	res.data.userId = -1;
	res.bestCount = 0;
	res.offerCount = 0;
	res.timeout = 0;

	vector<OfferFriendsCacheNManagerPrx> prxs = getAllProxySeq( applicant );
	if( prxs.size() <= 0 ){
		return res;
	}
	for( vector<OfferFriendsCacheNManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		int trys = 2;
		do{
			try {
				RmInfo tmp = (*itPrx)->apply( applicant, acceptor );
				if( !res.hit ){
					res = tmp;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[OfferFriendsCacheAdapter::apply("<<trys<<")] applicant=" << applicant
						<< " acceptor=" << acceptor 
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[OfferFriendsCacheAdapter::apply("<<trys<<")] applicant=" << applicant
						<< " acceptor=" << acceptor 
						<< " found unknown exception" );
			}
		} while( --trys>0 );
	}
	return res;
}

RmInfo OfferFriendsCacheAdapter::accept(int acceptor,int applicant) {
	RmInfo res;
	res.userId = acceptor;
	res.hit = false;
	res.found = false;
	res.data = OfferDataN();
	res.data.userId = -1;
	res.bestCount = 0;
	res.offerCount = 0;
	res.timeout = 0;

	vector<OfferFriendsCacheNManagerPrx> prxs = getAllProxySeq( acceptor );
	if( prxs.size() <= 0 ){
		return res;
	}
	for( vector<OfferFriendsCacheNManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		int trys = 2;
		do{
			try {
				RmInfo tmp = (*itPrx)->accept( acceptor, applicant );
				if( !res.hit ){
					res = tmp;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[OfferFriendsCacheAdapter::accept("<<trys<<")] acceptor=" << acceptor
						<< " applicant=" << applicant 
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[OfferFriendsCacheAdapter::accept("<<trys<<")] acceptor=" << acceptor
						<< " applicant=" << applicant 
						<< " found unknown exception" );
			}
		} while( --trys>0 );
	}
	return res;
}

RmInfo OfferFriendsCacheAdapter::denyOffer(int hostId,int guestId) {
	RmInfo res;
	res.userId = hostId;
	res.hit = false;
	res.found = false;
	res.data = OfferDataN();
	res.data.userId = -1;
	res.bestCount = 0;
	res.offerCount = 0;
	res.timeout = 0;

	vector<OfferFriendsCacheNManagerPrx> prxs = getAllProxySeq( hostId );
	if( prxs.size() <= 0 ){
		return res;
	}
	for( vector<OfferFriendsCacheNManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		int trys = 2;
		do{
			try {
				RmInfo tmp = (*itPrx)->denyOffer( hostId, guestId );
				if( !res.hit ){
					res = tmp;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[OfferFriendsCacheAdapter::denyOffer("<<trys<<")] hostId=" << hostId
						<< " guestId=" << guestId 
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[OfferFriendsCacheAdapter::denyOffer("<<trys<<")] hostId=" << hostId
						<< " guestId=" << guestId 
						<< " found unknown exception" );
			}
		} while( --trys>0 );
	}
	return res;
}

RmInfo OfferFriendsCacheAdapter::ignoreOffer(int hostId,int guestId) {
	RmInfo res;
	res.userId = hostId;
	res.hit = false;
	res.found = false;
	res.data = OfferDataN();
	res.data.userId = -1;
	res.bestCount = 0;
	res.offerCount = 0;
	res.timeout = 0;

	vector<OfferFriendsCacheNManagerPrx> prxs = getAllProxySeq( hostId );
	if( prxs.size() <= 0 ){
		return res;
	}
	for( vector<OfferFriendsCacheNManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		int trys = 2;
		do{
			try {
				RmInfo tmp = (*itPrx)->ignoreOffer( hostId, guestId );
				if( !res.hit ){
					res = tmp;
				}
				break;
			} catch( Ice::Exception e ){
				MCE_WARN("[OfferFriendsCacheAdapter::ignoreOffer("<<trys<<")] hostId=" << hostId
						<< " guestId=" << guestId 
						<< " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[OfferFriendsCacheAdapter::ignoreOffer("<<trys<<")] hostId=" << hostId
						<< " guestId=" << guestId 
						<< " found unknown exception" );
			}
		} while( --trys>0 );
	}
	return res;
}

void OfferFriendsCacheAdapter::load( int userId, const Ice::ObjectPtr& o ){
	vector<OfferFriendsCacheNManagerPrx> prxs = getAllProxySeq( userId );
	for( vector<OfferFriendsCacheNManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
		int trys = 2;
		do{
			try {
				(*itPrx)->load( userId, o );
				break;
			} catch(Ice::TimeoutException e){ 
				MCE_WARN("[OfferFriendsCacheAdapter::load] userId=" << userId << " found Ice::TimeoutException : " << e.what() );
			} catch( Ice::Exception e ){
				MCE_WARN("[OfferFriendsCacheAdapter::load] userId=" << userId << " found Ice::Exception : " << e.what() );
			} catch( ... ){
				MCE_WARN("[OfferFriendsCacheAdapter::load] userId=" << userId << " found unkown exception : " );
			}
		} while( --trys>0 );
	}
}

void OfferFriendsCacheAdapter::setData( const MyUtil::ObjectResultPtr& datas ){
	if (datas->data.empty()){
		return;
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[OfferFriendsCacheAdapter::setData] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return;
	}
	if (numCluster == 0 || numCluster == 1) {
		try {
			getProxy(0)->setData(datas);
		} catch(Ice::TimeoutException e){
			MCE_WARN("[OfferFriendsCacheAdapter::setData] Proxy(0) handle Ice::TimeoutException : " << e.what() );
		} catch( Ice::Exception e ){
			MCE_WARN("[OfferFriendsCacheAdapter::setData] Proxy(0) handle Ice::Exception : " << e.what() );
		} catch( ... ){
			MCE_WARN("[OfferFriendsCacheAdapter::setData] Proxy(0) handle unkown exception : " );
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
				vector<OfferFriendsCacheNManagerPrx> prxs = getAllProxySeq( posCluster );
				for( vector<OfferFriendsCacheNManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
					try {
						(*itPrx)->setData( seperatedObjs.at(posCluster) );
					} catch(Ice::TimeoutException e){
						MCE_WARN("[OfferFriendsCacheAdapter::setData] Proxy(" << posCluster << ") handle Ice::TimeoutException : " << e.what() );
						continue;
					} catch( Ice::Exception e ){
						MCE_WARN("[OfferFriendsCacheAdapter::setData] Proxy(" << posCluster << ") handle Ice::Exception : " << e.what() );
						continue;
					} catch( ... ){
						MCE_WARN("[OfferFriendsCacheAdapter::setData] Proxy(" << posCluster << ") handle unkown exception : " );
						continue;
					}
				}
			}
		}
	}
}


// ========= OfferFriendsLoaderAdapter =================================

void OfferFriendsLoaderAdapter::apply( int applicant,int acceptor ){
	getProxy( applicant )->apply( applicant, acceptor );
}
void OfferFriendsLoaderAdapter::accept( int acceptor,int applicant ){
	getProxy( acceptor )->accept( acceptor, applicant );
}
void OfferFriendsLoaderAdapter::denyOffer( int hostId,int guestId ){
	getProxy( hostId )->denyOffer( hostId, guestId );
}
void OfferFriendsLoaderAdapter::ignoreOffers( int userId,const MyUtil::IntSeq& ids ){
	getProxy( userId )->ignoreOffers( userId, ids );
}
void OfferFriendsLoaderAdapter::rebuild( int userId, bool now ){
	getProxy( userId )->rebuild( userId, now );
}
void OfferFriendsLoaderAdapter::report( GetInfo info ){
	getProxy( info.userId )->report( info );
}
void OfferFriendsLoaderAdapter::rebuildBatch( const MyUtil::IntSeq& userIds,bool now ){
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[OfferFriendsLoaderAdapter::rebuildBatch] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return;
	}
	if (numCluster == 0 || numCluster == 1) {
		try {
			getProxy(0)->rebuildBatch(userIds,now);
		} catch(Ice::TimeoutException e){
			MCE_WARN("[OfferFriendsLoaderAdapter::rebuildBatch] Proxy(0) handle Ice::TimeoutException : " << e.what() );
		} catch( Ice::Exception e ){
			MCE_WARN("[OfferFriendsLoaderAdapter::rebuildBatch] Proxy(0) handle Ice::Exception : " << e.what() );
		} catch( ... ){
			MCE_WARN("[OfferFriendsLoaderAdapter::rebuildBatch] Proxy(0) handle unkown exception : " );
		}
	} else {
		vector< MyUtil::IntSeq > seperatedIds;
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			seperatedIds.push_back( MyUtil::IntSeq() );
		}
		for ( MyUtil::IntSeq::const_iterator it = userIds.begin();
				it!= userIds.end(); ++it ){
			seperatedIds.at( (*it) % numCluster).push_back( *it );
		}
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			if( seperatedIds.at(posCluster).size() > 0 ){
				vector<OfferFriendsLoaderNPrx> prxs = getAllProxySeq( posCluster );
				for( vector<OfferFriendsLoaderNPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
					try {
						(*itPrx)->rebuildBatch( seperatedIds.at(posCluster),now );
					} catch(Ice::TimeoutException e){
						MCE_WARN("[OfferFriendsLoaderAdapter::rebuildBatch] Proxy(" << posCluster << ") handle Ice::TimeoutException : " << e.what() );
						continue;
					} catch( Ice::Exception e ){
						MCE_WARN("[OfferFriendsLoaderAdapter::rebuildBatch] Proxy(" << posCluster << ") handle Ice::Exception : " << e.what() );
						continue;
					} catch( ... ){
						MCE_WARN("[OfferFriendsLoaderAdapter::rebuildBatch] Proxy(" << posCluster << ") handle unkown exception : " );
						continue;
					}
				}
			}
		}
	}
}
std::string OfferFriendsLoaderAdapter::debug( int userId ){
	return getProxy( userId )->debug( userId );
}
void OfferFriendsLoaderAdapter::reload( int userId ){
	getProxy( userId )->reload( userId );
}
void OfferFriendsLoaderAdapter::reloadBatch( const MyUtil::IntSeq& userIds ){
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int)numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[OfferFriendsLoaderAdapter::reloadBatch] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return;
	}
	if (numCluster == 0 || numCluster == 1) {
		try {
			getProxy(0)->reloadBatch(userIds);
		} catch(Ice::TimeoutException e){
			MCE_WARN("[OfferFriendsLoaderAdapter::reloadBatch] Proxy(0) handle Ice::TimeoutException : " << e.what() );
		} catch( Ice::Exception e ){
			MCE_WARN("[OfferFriendsLoaderAdapter::reloadBatch] Proxy(0) handle Ice::Exception : " << e.what() );
		} catch( ... ){
			MCE_WARN("[OfferFriendsLoaderAdapter::reloadBatch] Proxy(0) handle unkown exception : " );
		}
	} else {
		vector< MyUtil::IntSeq > seperatedIds;
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			seperatedIds.push_back( MyUtil::IntSeq() );
		}
		for ( MyUtil::IntSeq::const_iterator it = userIds.begin();
				it!= userIds.end(); ++it ){
			seperatedIds.at( (*it) % numCluster).push_back( *it );
		}
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			if( seperatedIds.at(posCluster).size() > 0 ){
				vector<OfferFriendsLoaderNPrx> prxs = getAllProxySeq( posCluster );
				for( vector<OfferFriendsLoaderNPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
					try {
						(*itPrx)->reloadBatch( seperatedIds.at(posCluster) );
					} catch(Ice::TimeoutException e){
						MCE_WARN("[OfferFriendsLoaderAdapter::reloadBatch] Proxy(" << posCluster << ") handle Ice::TimeoutException : " << e.what() );
						continue;
					} catch( Ice::Exception e ){
						MCE_WARN("[OfferFriendsLoaderAdapter::reloadBatch] Proxy(" << posCluster << ") handle Ice::Exception : " << e.what() );
						continue;
					} catch( ... ){
						MCE_WARN("[OfferFriendsLoaderAdapter::reloadBatch] Proxy(" << posCluster << ") handle unkown exception : " );
						continue;
					}
				}
			}
		}
	}
}


