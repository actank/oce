/*
 * ILikeItCacheOldAdapter.cpp
 *
 *  Created on: 2009-07-06
 *      Author: zhanghan
 */

#include "OceCxxAdapter/src/ILikeItCacheOldAdapter.h"
#include "ILikeIt/share/ILikeItUtil.h"

using namespace MyUtil;
using namespace xce::clusterstate;
using namespace xce::ilikeit;

// ========= ILikeItCacheOldAdapter =================================
ILikeItCacheOldAdapter::ILikeItCacheOldAdapter() :
	_clientCS("ControllerILikeItCache", 120, 300) {

	_clientCS.intialize();
	// _cluster = _clientCS.getCluster();
	_cluster = 10;
	TaskManager::instance().config(TASK_LEVEL_LOADSHIPS_OLD, MyUtil::ThreadPoolConfig(1, 30));
	TaskManager::instance().config(TASK_LEVEL_ADDSHIPS_OLD, MyUtil::ThreadPoolConfig(1, 30));
	TaskManager::instance().config(TASK_LEVEL_REMOVESHIPS_OLD, MyUtil::ThreadPoolConfig(1, 30));
}

int ILikeItCacheOldAdapter::getLikeCount(int type, long id) {

	GlobalIdPtr gid = new GlobalId;
	gid->type = type;
	gid->id = id;
	int count = 0;
	try {
		count = _clientCS.getProxy(gid->id)->getLikeCount(gid);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItCacheOldAdapter::getLikeCount] " << type << ":" << id  << " Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItCacheOldAdapter::getLikeCount] " << type << ":" << id  << " std::excetpion : " << e.what()); 
	} catch (...) {
		MCE_WARN("[ILikeItCacheOldAdapter::getLikeCount] " << type << ":" << id << " exception");
	}
	return count;
}

LikeInfoPtr ILikeItCacheOldAdapter::getLikeDetail(const string& gidStr, int uid, int limit, bool showStrangers) {

	GlobalIdPtr gid = GlobalIdTrans::Str2GlobalId(gidStr);	
	return getLikeDetail(gid, uid, limit, showStrangers);
}

LikeInfoPtr ILikeItCacheOldAdapter::getLikeDetail(const GlobalIdPtr& gid, int uid, int limit, bool showStrangers) {
	LikeInfoPtr result = ILikeItUtil::emptyLikeInfo(limit, showStrangers);

	try {
		result = _clientCS.getProxy(gid->id)->getLikeDetail(gid, uid, limit, showStrangers);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItCacheOldAdapter::getLikeDetail] " << gid->type << ":" << gid->id << " for" << uid << " Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItCacheOldAdapter::getLikeDetail] " << gid->type << ":" << gid->id << " for " << uid << " std::excetpion : " << e.what()); 
	} catch (...) {
		MCE_WARN("[ILikeItCacheOldAdapter::getLikeDetail] " << gid->type << ":" << gid->id << " for " << uid << " exception");
	}

	return result;	
}

LikeInfoSeq ILikeItCacheOldAdapter::getLikeDetailBatch(const std::vector<std::string>& gidStrs, int uid, int limit, int detailNum) {
	
	LikeInfoSeq results;
	map<int, GlobalIdSeq> gidGroups;
	map<int, LikeInfoSeq> resultMap;
	vector< pair<int, int> > vecOrder;
	for (size_t i = 0; i < gidStrs.size(); ++i ) {
		GlobalIdPtr gid = GlobalIdTrans::Str2GlobalId(gidStrs[i]);
		int mod = gid->id % _cluster;
		if ( gidGroups.find(mod) == gidGroups.end() ) {
			GlobalIdSeq group;
			group.push_back(gid);
			gidGroups.insert(make_pair<int, GlobalIdSeq>(mod, group));
		} else {
			gidGroups[mod].push_back(gid);
		}
		vecOrder.push_back(make_pair<int, int>(mod, gidGroups[mod].size()-1));
	}
	for (map<int, GlobalIdSeq>::iterator it = gidGroups.begin(); it != gidGroups.end(); ++it) {
		LikeInfoSeq result = _clientCS.getProxy((*it).first)->getLikeDetailBatch((*it).second, uid, limit, detailNum);
		resultMap.insert(make_pair<int, LikeInfoSeq>((*it).first, result));
	}
	// 重新组合
	for ( size_t i = 0; i < vecOrder.size(); ++i ) {
		LikeInfoPtr result = ILikeItUtil::emptyLikeInfo(limit);
		int mod = vecOrder[i].first;
		int pos = vecOrder[i].second;
		if ( resultMap.find(mod) != resultMap.end() ) {
			if ( size_t(pos) < resultMap[mod].size() ) {
				result = resultMap[mod][pos];
			}				
		}
		results.push_back(result);
	}
	return results;
}

void ILikeItCacheOldAdapter::load(const GlobalIdSeq& gids)  {

	// 获取所有prx
	map<int, vector<ILikeItCachePrx> > prxs = _clientCS.getAvailableProxies();
	MCE_DEBUG("[LoadTask::handle] need reload " << prxs.size() << " prxMap(s)");
	for (map<int, vector<ILikeItCachePrx> >::iterator it0 = prxs.begin(); 
			it0 != prxs.end(); ++it0)   {
		MCE_DEBUG("[LoadTask::handle] need reload " << (*it0).second.size() << " prx(s)");
		for (vector<ILikeItCachePrx>::iterator it1 = (*it0).second.begin();
				it1 != (*it0).second.end(); ++it1 ) {
			try {
				(*it1)->load(gids);
				MCE_DEBUG("[LoadTask::handle] load for " << (*it1)->ice_toString() << " OK");
			} catch (Ice::Exception& e) {
				MCE_WARN("[LoadTask::handle] load for " << (*it1)->ice_toString() << " Ice::Exception : " << e.what());
				continue;
			} catch (std::exception& e) {
				MCE_WARN("[LoadTask::handle] load for " << (*it1)->ice_toString() << " std::exception : " << e.what());
				continue;
			} catch (...) {
				MCE_WARN("[LoadTask::handle] load for " << (*it1)->ice_toString() << " Exception");
				continue;
			}
			MCE_INFO("[LoadTask::handle] reload " << gids.size() << " for " << (*it1)->ice_toString());
		}
	}
}

void ILikeItCacheOldAdapter::addLikeship(const LikeshipPtr& ship)  {

	// 获取所有符合mod的prx
	map<int, vector<ILikeItCachePrx> > prxs = _clientCS.getAvailableProxies();
	map<int, vector<ILikeItCachePrx> >::iterator it0 = prxs.find(ship->gid->id % _cluster);
	if ( it0 == prxs.end() ) {
		return;
	}

	for ( vector<ILikeItCachePrx>::iterator it1 = (*it0).second.begin(); it1 != (*it0).second.end(); ++it1 ) {
		try {
			(*it1)->addLikeship(ship);
			MCE_DEBUG("[ILikeItCacheOldAdapter::addLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString() << " OK");
		} catch (Ice::Exception& e) {
			MCE_DEBUG("[ILikeItCacheOldAdapter::addLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString() << " Ice::Exception : " << e.what());
			continue;
		} catch (std::exception& e) {
			MCE_DEBUG("[ILikeItCacheOldAdapter::addLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString() << " std::exception : " << e.what());
			continue;
		} catch (...) {
			MCE_WARN("[ILikeItCacheOldAdapter::addLikeship] addship(s) for " << (*it1)->ice_toString()<< " Exception");
			continue;
		}
		MCE_DEBUG("[ILikeItCacheOldAdapter::addLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString());
	}	
}

void ILikeItCacheOldAdapter::removeLikeship(const LikeshipPtr& ship)  {

	// 获取所有符合mod的prx
	map<int, vector<ILikeItCachePrx> > prxs = _clientCS.getAvailableProxies();
	map<int, vector<ILikeItCachePrx> >::iterator it0 = prxs.find(ship->gid->id % _cluster);
	if ( it0 == prxs.end() ) {
		return;
	}

	for ( vector<ILikeItCachePrx>::iterator it1 = (*it0).second.begin(); it1 != (*it0).second.end(); ++it1 ) {
		try {
			(*it1)->removeLikeship(ship);
			MCE_DEBUG("[ILikeItCacheOldAdapter::removeLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString() << " OK");
		} catch (Ice::Exception& e) {
			MCE_DEBUG("[ILikeItCacheOldAdapter::removeLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString() << " Ice::Exception : " << e.what());
			continue;
		} catch (std::exception& e) {
			MCE_DEBUG("[ILikeItCacheOldAdapter::removeLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString() << " std::exception : " << e.what());
			continue;
		} catch (...) {
			MCE_WARN("[ILikeItCacheOldAdapter::removeLikeship] addship(s) for " << (*it1)->ice_toString()<< " Exception");
			continue;
		}
		MCE_DEBUG("[ILikeItCacheOldAdapter::removeLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString());
	}	
}




void ILikeItCacheOldAdapter::loadSync(const GlobalIdSeq& gids)  {

	map<int, vector<ILikeItCachePrx> > prxs = _clientCS.getAvailableProxies();
	MCE_DEBUG("[ILikeItCacheOldAdapter::loadSync] need reload " << prxs.size() << " prxMap(s)");
	for (map<int, vector<ILikeItCachePrx> >::iterator it0 = prxs.begin(); 
			it0 != prxs.end(); ++it0)   {
		MCE_DEBUG("[ILikeItCacheOldAdapter::loadSync] need reload " << (*it0).second.size() << " prx(s)");
		for (vector<ILikeItCachePrx>::iterator it1 = (*it0).second.begin();
				it1 != (*it0).second.end(); ++it1 ) {
			try {
				(*it1)->load(gids);
			} catch (Ice::Exception& e) {
				MCE_WARN("[ILikeItCacheOldAdapter::loadSync] load for " << (*it1)->ice_toString() << " Ice::Exception : " << e.what());
				continue;
			} catch (std::exception& e) {
				MCE_WARN("[ILikeItCacheOldAdapter::loadSync] load for " << (*it1)->ice_toString() << " std::exception : " << e.what());
				continue;
			} catch (...) {
				MCE_WARN("[ILikeItCacheOldAdapter::loadSync] load for " << (*it1)->ice_toString() << " Exception");
				continue;
			}
			MCE_INFO("[ILikeItCacheOldAdapter::loadSync] reload " << gids.size() << " for " << (*it1)->ice_toString());
		}
	}
}
