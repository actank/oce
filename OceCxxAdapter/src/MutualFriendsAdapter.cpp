#include "MutualFriendsAdapter.h"

using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

//------------------------MutualFriendsCacheAdapter-----------------------------
MyUtil::IntSeq MutualFriendsCacheAdapter::getMutualFriends(int uid, const MyUtil::GlobalIdPtr& gid, int begin, int limit) {
	long globalId = GlobalIdTransfer::get(gid);
	//getProxy(globalId % 10000)是为了兼容java端，java端只支持getProxy(int)
	return getProxy(globalId % 10000)->getMutualFriends(uid, gid, begin, limit);
}

MyUtil::IntSeq MutualFriendsCacheAdapter::getUnMutualFriends(int uid, const MyUtil::GlobalIdPtr& gid, int begin, int limit) {
	long globalId = GlobalIdTransfer::get(gid);
	return getProxy(globalId % 10000)->getUnMutualFriends(uid, gid, begin, limit);
}

int MutualFriendsCacheAdapter::getMutualFriendsCount(int uid, const MyUtil::GlobalIdPtr& gid) {
	long globalId = GlobalIdTransfer::get(gid);
	return getProxy(globalId % 10000)->getMutualFriendsCount(uid, gid);
}

int MutualFriendsCacheAdapter::getUnMutualFriendsCount(int uid, const MyUtil::GlobalIdPtr& gid) {
	long globalId = GlobalIdTransfer::get(gid);
	return getProxy(globalId % 10000)->getUnMutualFriendsCount(uid, gid);
}

int MutualFriendsCacheAdapter::getFansCount(const MyUtil::GlobalIdPtr& gid) {
	Ice::Long globalId = GlobalIdTransfer::get(gid);
	return getProxy(globalId % 10000)->getFansCount(gid);
}

bool MutualFriendsCacheAdapter::isFans(int uid, const MyUtil::GlobalIdPtr& gid) {
	Ice::Long globalId = GlobalIdTransfer::get(gid);
	return getProxy(globalId % 10000)->isFans(uid, gid);
}

MyUtil::IntSeq MutualFriendsCacheAdapter::getFans(const MyUtil::GlobalIdPtr& gid, int begin, int limit) {
	Ice::Long globalId = GlobalIdTransfer::get(gid);
	return getProxy(globalId % 10000)->getFans(gid, begin, limit);
}

void MutualFriendsCacheAdapter::add(int uid, const MyUtil::GlobalIdPtr& gid) {
	Ice::Long globalId = GlobalIdTransfer::get(gid);
	vector<MutualFriendsCacheManagerPrx> prxs = getAllProxySeq(globalId % 10000);
	for (vector<MutualFriendsCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
		try {
			(*itPrx)->add(uid, gid);
		} catch (Ice::TimeoutException e) {
			MCE_WARN("[MutualFriendsCacheAdapter::setData] Proxy(" << globalId % 10000 << ") handle Ice::TimeoutException : " << e.what() );
			continue;
		} catch (Ice::Exception e) {
			MCE_WARN("[MutualFriendsCacheAdapter::setData] Proxy(" << globalId % 10000 << ") handle Ice::Exception : " << e.what() );
			continue;
		} catch (...) {
			MCE_WARN("[MutualFriendsCacheAdapter::setData] Proxy(" << globalId % 10000 << ") handle unkown exception : " );
			continue;
		}
	}
}

void MutualFriendsCacheAdapter::remove(int uid, const MyUtil::GlobalIdPtr& gid) {
	Ice::Long globalId = GlobalIdTransfer::get(gid);
	vector<MutualFriendsCacheManagerPrx> prxs = getAllProxySeq(globalId % 10000);
	for (vector<MutualFriendsCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
		try {
			(*itPrx)->add(uid, gid);
		} catch (Ice::TimeoutException e) {
			MCE_WARN("[MutualFriendsCacheAdapter::setData] Proxy(" << globalId % 10000 << ") handle Ice::TimeoutException : " << e.what() );
			continue;
		} catch (Ice::Exception e) {
			MCE_WARN("[MutualFriendsCacheAdapter::setData] Proxy(" << globalId % 10000 << ") handle Ice::Exception : " << e.what() );
			continue;
		} catch (...) {
			MCE_WARN("[MutualFriendsCacheAdapter::setData] Proxy(" << globalId % 10000 << ") handle unkown exception : " );
			continue;
		}
	}
}

//setData提供给MutualFriendsLoader调用
void MutualFriendsCacheAdapter::setData(const MyUtil::ObjectResultPtr& datas) {
	if (datas->data.empty()) {
		return;
	}
	size_t numCluster_sizeT = getCluster();
	int numCluster = (int) numCluster_sizeT;
	if (numCluster < 0) {
		MCE_WARN("[MutualFriendsCacheAdapter::setData] this should not be seen! Why cluster=" << numCluster_sizeT << " ???" );
		return;
	}
	if (numCluster == 0 || numCluster == 1) {
		try {
			getProxy(0)->setData(datas);
		} catch (...) {
			return;
		}
	} else {
		vector<MyUtil::ObjectResultPtr> seperatedObjs;
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			seperatedObjs.push_back(new MyUtil::ObjectResult());
		}
		for (std::map<Ice::Long, Ice::ObjectPtr>::const_iterator it = datas->data.begin(); it != datas->data.end(); ++it) {
			seperatedObjs.at(it->first % numCluster)->data.insert(make_pair<Ice::Long, Ice::ObjectPtr> (it->first, it->second));
		}
		for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
			if (seperatedObjs.at(posCluster)->data.size() > 0) {
				vector<MutualFriendsCacheManagerPrx> prxs = getAllProxySeq(posCluster);
				for (vector<MutualFriendsCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
					try {
						(*itPrx)->setData(seperatedObjs.at(posCluster));
					} catch (Ice::TimeoutException e) {
						MCE_WARN("[MutualFriendsCacheAdapter::setData] Proxy(" << posCluster << ") handle Ice::TimeoutException : " << e.what() );
						continue;
					} catch (Ice::Exception e) {
						MCE_WARN("[MutualFriendsCacheAdapter::setData] Proxy(" << posCluster << ") handle Ice::Exception : " << e.what() );
						continue;
					} catch (...) {
						MCE_WARN("[MutualFriendsCacheAdapter::setData] Proxy(" << posCluster << ") handle unkown exception : " );
						continue;
					}
				}
			}
		}
	}
}

//-----------------------MutualFriendsLoaderAdapter-----------------------------
//reload由MutualFriendsCache的ReloadTask调用
void MutualFriendsLoaderAdapter::reload(Ice::Long gid) {
	MCE_DEBUG( "[MutualFriendsLoaderAdapter::reload] gid = " << gid );
	getProxy(gid % 10000)->reload(gid);
}
