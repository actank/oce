/*
 * ILikeItCacheAdapter.cpp
 *
 *  Created on: 2010-01-28
 *      Author: zhanghan
 */

#include "OceCxxAdapter/src/ILikeItCacheAdapter.h"
#include "app/ILikeIt/share/ILikeItUtil.h"

using namespace MyUtil;
using namespace xce::clusterstate;
using namespace xce::ilikeit;

// ========= ILikeItCacheAdapter =================================
ILikeItCacheAdapter::ILikeItCacheAdapter() :	
	_clientCS("ControllerILikeItCacheM", 120, 300) {

	_clientCS.intialize();
	// _cluster = _clientCS.getCluster();
	_cluster = CACHE_CLUSTER;
	TaskManager::instance().config(TASK_LEVEL_LOADSHIPS, MyUtil::ThreadPoolConfig(1, 30));
	TaskManager::instance().config(TASK_LEVEL_ADDSHIPS, MyUtil::ThreadPoolConfig(1, 30));
	TaskManager::instance().config(TASK_LEVEL_REMOVESHIPS, MyUtil::ThreadPoolConfig(1, 30));
}

int ILikeItCacheAdapter::getLikeCount(int type, long id) {

	GlobalIdPtr gid = new GlobalId;
	gid->type = type;
	gid->id = id;
	int count = 0;
	try {
		count = _clientCS.getProxy(gid->id)->getLikeCount(gid);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItCacheAdapter::getLikeCount] " << type << ":" << id  << " Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItCacheAdapter::getLikeCount] " << type << ":" << id  << " std::excetpion : " << e.what()); 
	} catch (...) {
		MCE_WARN("[ILikeItCacheAdapter::getLikeCount] " << type << ":" << id << " exception");
	}
	return count;
}

MyUtil::IntSeq ILikeItCacheAdapter::getLikeCache(int type, long id) {

	GlobalIdPtr gid = new GlobalId;
	gid->type = type;
	gid->id = id;
	
	try {
		return _clientCS.getProxy(gid->id)->getLikeCache(gid);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItCacheAdapter::getLikeCount] " << type << ":" << id  << " Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItCacheAdapter::getLikeCount] " << type << ":" << id  << " std::excetpion : " << e.what()); 
	} catch (...) {
		MCE_WARN("[ILikeItCacheAdapter::getLikeCount] " << type << ":" << id << " exception");
	}

	MyUtil::IntSeq result;
	return result;

}

bool ILikeItCacheAdapter::isValid(long id){
	try {
		return _clientCS.getProxy(id)->isValid();
	}catch (Ice::Exception& e) {
                MCE_WARN("[ILikeItCacheAdapter::isValid] " << ":" << id  << " Ice::Exception : " << e.what());
        } catch (std::exception& e) {
                MCE_WARN("[ILikeItCacheAdapter::isValid] " << ":" << id  << " std::excetpion : " << e.what());
        } catch (...) {
                MCE_WARN("[ILikeItCacheAdapter::isValid] " << ":" << id << " exception");
        }
	
	return false;
}

LikeInfoPtr ILikeItCacheAdapter::getLikeDetail(const string& gidStr, int uid, int limit, bool showStrangers) {

	GlobalIdPtr gid = GlobalIdTrans::Str2GlobalId(gidStr);	
	return getLikeDetail(gid, uid, limit, showStrangers);
}

LikeInfo2Ptr ILikeItCacheAdapter::getLikeDetail2(const string& gidStr, int uid, int limit, bool showStrangers) {

	GlobalIdPtr gid = GlobalIdTrans::Str2GlobalId(gidStr);	
	return getLikeDetail2(gid, uid, limit, showStrangers);
}

LikeInfoPtr ILikeItCacheAdapter::getLikeDetail(const GlobalIdPtr& gid, int uid, int limit, bool showStrangers) {
	LikeInfoPtr result = ILikeItUtil::emptyLikeInfo(limit, showStrangers);

	try {
		result = _clientCS.getProxy(gid->id)->getLikeDetail(gid, uid, limit, showStrangers);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItCacheAdapter::getLikeDetail] " << gid->type << ":" << gid->id << " for" << uid << " Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItCacheAdapter::getLikeDetail] " << gid->type << ":" << gid->id << " for " << uid << " std::excetpion : " << e.what()); 
	} catch (...) {
		MCE_WARN("[ILikeItCacheAdapter::getLikeDetail] " << gid->type << ":" << gid->id << " for " << uid << " exception");
	}

	return result;	
}

LikeInfo2Ptr ILikeItCacheAdapter::getLikeDetail2(const GlobalIdPtr& gid, int uid, int limit, bool showStrangers) {
	LikeInfo2Ptr result = ILikeItUtil::emptyLikeInfo2(limit, showStrangers);

	try {
		result = _clientCS.getProxy(gid->id)->getLikeDetail2(gid, uid, limit, showStrangers);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItCacheAdapter::getLikeDetail2] " << gid->type << ":" << gid->id << " for" << uid << " Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItCacheAdapter::getLikeDetail2] " << gid->type << ":" << gid->id << " for " << uid << " std::excetpion : " << e.what()); 
	} catch (...) {
		MCE_WARN("[ILikeItCacheAdapter::getLikeDetail2] " << gid->type << ":" << gid->id << " for " << uid << " exception");
	}

	return result;	
}

LikeInfoSeq ILikeItCacheAdapter::getLikeDetailBatch(const std::vector<std::string>& gidStrs, int uid, int limit, int detailNum) {

	// 如果只散一份，则不重新组合
	if ( _cluster == 1 ) {
		
		GlobalIdSeq gids;
		for ( size_t i = 0; i < gidStrs.size(); ++i ) {
			GlobalIdPtr gid = GlobalIdTrans::Str2GlobalId(gidStrs[i]);
			gids.push_back(gid);
		}
		return _clientCS.getProxy(0)->getLikeDetailBatch(gids, uid, limit, detailNum);
	}

	// 多份的话, 则需要重新组合	
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

LikeInfo2Seq ILikeItCacheAdapter::getLikeDetailBatch2(const std::vector<std::string>& gidStrs, int uid, int limit, int detailNum) {

	// 如果只散一份，则不重新组合
	if ( _cluster == 1 ) {
		
		GlobalIdSeq gids;
		for ( size_t i = 0; i < gidStrs.size(); ++i ) {
			GlobalIdPtr gid = GlobalIdTrans::Str2GlobalId(gidStrs[i]);
			gids.push_back(gid);
		}
		return _clientCS.getProxy(0)->getLikeDetailBatch2(gids, uid, limit, detailNum);
	}

	// 多份的话, 则需要重新组合	
	LikeInfo2Seq results;
	map<int, GlobalIdSeq> gidGroups;
	map<int, LikeInfo2Seq> resultMap;
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
		LikeInfo2Seq result = _clientCS.getProxy((*it).first)->getLikeDetailBatch2((*it).second, uid, limit, detailNum);
		resultMap.insert(make_pair<int, LikeInfo2Seq>((*it).first, result));
	}
	// 重新组合
	for ( size_t i = 0; i < vecOrder.size(); ++i ) {
		LikeInfo2Ptr result = ILikeItUtil::emptyLikeInfo2(limit);
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

LikeInfoSeq ILikeItCacheAdapter::getLikeDetailBatchG(const std::vector<GlobalIdPtr>& gids, int uid, int limit, int detailNum) {

	// 如果只散一份，则不重新组合
	if ( _cluster == 1 ) {		
		return _clientCS.getProxy(0)->getLikeDetailBatch(gids, uid, limit, detailNum);
	}

	// 多份的话, 则需要重新组合	
	LikeInfoSeq results;
	map<int, GlobalIdSeq> gidGroups;
	map<int, LikeInfoSeq> resultMap;
	vector< pair<int, int> > vecOrder;
	for (size_t i = 0; i < gids.size(); ++i ) {
		int mod = gids[i]->id % _cluster;
		if ( gidGroups.find(mod) == gidGroups.end() ) {
			GlobalIdSeq group;
			group.push_back(gids[i]);
			gidGroups.insert(make_pair<int, GlobalIdSeq>(mod, group));
		} else {
			gidGroups[mod].push_back(gids[i]);
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

LikeInfo2Seq ILikeItCacheAdapter::getLikeDetailBatchG2(const std::vector<GlobalIdPtr>& gids, int uid, int limit, int detailNum) {

	// 如果只散一份，则不重新组合
	if ( _cluster == 1 ) {		
		return _clientCS.getProxy(0)->getLikeDetailBatch2(gids, uid, limit, detailNum);
	}

	// 多份的话, 则需要重新组合	
	LikeInfo2Seq results;
	map<int, GlobalIdSeq> gidGroups;
	map<int, LikeInfo2Seq> resultMap;
	vector< pair<int, int> > vecOrder;
	for (size_t i = 0; i < gids.size(); ++i ) {
		int mod = gids[i]->id % _cluster;
		if ( gidGroups.find(mod) == gidGroups.end() ) {
			GlobalIdSeq group;
			group.push_back(gids[i]);
			gidGroups.insert(make_pair<int, GlobalIdSeq>(mod, group));
		} else {
			gidGroups[mod].push_back(gids[i]);
		}
		vecOrder.push_back(make_pair<int, int>(mod, gidGroups[mod].size()-1));
	}
	for (map<int, GlobalIdSeq>::iterator it = gidGroups.begin(); it != gidGroups.end(); ++it) {
		LikeInfo2Seq result = _clientCS.getProxy((*it).first)->getLikeDetailBatch2((*it).second, uid, limit, detailNum);
		resultMap.insert(make_pair<int, LikeInfo2Seq>((*it).first, result));
	}
	// 重新组合
	for ( size_t i = 0; i < vecOrder.size(); ++i ) {
		LikeInfo2Ptr result = ILikeItUtil::emptyLikeInfo2(limit);
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

void ILikeItCacheAdapter::setData(const ::MyUtil::ObjectResultPtr& objects, int mod, ILikeItCachePrx& localPrx) {

	// 获取该mod的所有prx
	//map<int, vector<ILikeItCachePrx> > prxsMap = _clientCS.getAvailableProxies();
	//map<int, vector<ILikeItCachePrx> >::iterator itSeq = prxsMap.find(mod);
	vector<ILikeItCachePrx> itSeq = _clientCS.getInvalidProxySeq(mod);
	

	//if ( itSeq == prxsMap.end() ) {
	if(itSeq.size()==0){
		MCE_WARN("[ILikeItCacheAdapter::setData] no prx for mod : " << mod << ". Can't setData " << objects->data.size()<<"(s)");
		return;
	}
	bool checkSelf = true;
	::Ice::Identity localIdentity;
	if ( localPrx ) {
		localIdentity = localPrx->ice_getIdentity();
	} else {
		checkSelf = false;
	}

	int setEd = 0;
	for (vector<ILikeItCachePrx>::iterator it = itSeq.begin(); it != itSeq.end(); ++it) {
		if ( ! (*it) ) {
			continue;
		}
		if ( checkSelf ) {
			if ( (*it)->ice_getIdentity() == localIdentity ) {
				// 不再通知自己
				continue;
			}
		}
		try {
			(*it)->setData(objects);
			++setEd;
			MCE_DEBUG("[ILikeItCacheAdapter::setData] setData " << objects->data.size() << "(s) for " << (*it)->ice_toString() << " OK");
		} catch (Ice::Exception& e) {
			MCE_WARN("[ILikeItCacheAdapter::setData] setData " << objects->data.size() << "(s) for " << (*it)->ice_toString() << " Ice::Exception : " << e.what());
		} catch (std::exception& e) {
			MCE_WARN("[ILikeItCacheAdapter::setData] setData " << objects->data.size() << "(s) for " << (*it)->ice_toString() << " std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[ILikeItCacheAdapter::setData] setData " << objects->data.size() << "(s) for " << (*it)->ice_toString() << " UNKNOWN exception");
		}
	}
	MCE_INFO("[[ILikeItCacheAdapter::setData] setData " << objects->data.size() << "(s) for " << setEd << " prxs");

}

void ILikeItCacheAdapter::load(const GlobalIdSeq& gids)  {

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

void ILikeItCacheAdapter::addLikeship(const LikeshipPtr& ship)  {

	// 获取所有符合mod的prx
	map<int, vector<ILikeItCachePrx> > prxs = _clientCS.getAvailableProxies();
	map<int, vector<ILikeItCachePrx> >::iterator it0 = prxs.find(ship->gid->id % _cluster);
	if ( it0 == prxs.end() ) {
		return;
	}

	for ( vector<ILikeItCachePrx>::iterator it1 = (*it0).second.begin(); it1 != (*it0).second.end(); ++it1 ) {
		try {
			(*it1)->addLikeship(ship);
			MCE_DEBUG("[ILikeItCacheAdapter::addLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString() << " OK");
		} catch (Ice::Exception& e) {
			MCE_DEBUG("[ILikeItCacheAdapter::addLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString() << " Ice::Exception : " << e.what());
			continue;
		} catch (std::exception& e) {
			MCE_DEBUG("[ILikeItCacheAdapter::addLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString() << " std::exception : " << e.what());
			continue;
		} catch (...) {
			MCE_WARN("[ILikeItCacheAdapter::addLikeship] addship(s) for " << (*it1)->ice_toString()<< " Exception");
			continue;
		}
		MCE_DEBUG("[ILikeItCacheAdapter::addLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString());
	}	
}


void ILikeItCacheAdapter::removeLikeship(const LikeshipPtr& ship)  {

	// 获取所有符合mod的prx
	map<int, vector<ILikeItCachePrx> > prxs = _clientCS.getAvailableProxies();
	map<int, vector<ILikeItCachePrx> >::iterator it0 = prxs.find(ship->gid->id % _cluster);
	if ( it0 == prxs.end() ) {
		return;
	}

	for ( vector<ILikeItCachePrx>::iterator it1 = (*it0).second.begin(); it1 != (*it0).second.end(); ++it1 ) {
		try {
			(*it1)->removeLikeship(ship);
			MCE_DEBUG("[ILikeItCacheAdapter::removeLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString() << " OK");
		} catch (Ice::Exception& e) {
			MCE_DEBUG("[ILikeItCacheAdapter::removeLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString() << " Ice::Exception : " << e.what());
			continue;
		} catch (std::exception& e) {
			MCE_DEBUG("[ILikeItCacheAdapter::removeLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString() << " std::exception : " << e.what());
			continue;
		} catch (...) {
			MCE_WARN("[ILikeItCacheAdapter::removeLikeship] addship(s) for " << (*it1)->ice_toString()<< " Exception");
			continue;
		}
		MCE_DEBUG("[ILikeItCacheAdapter::removeLikeship] add user " << ship->uid << " to gid " << ship->gid->type << ":" << ship->gid->id << " for " << (*it1)->ice_toString());
	}	
}




void ILikeItCacheAdapter::loadSync(const GlobalIdSeq& gids)  {

	map<int, vector<ILikeItCachePrx> > prxs = _clientCS.getAvailableProxies();
	MCE_DEBUG("[ILikeItCacheAdapter::loadSync] need reload " << prxs.size() << " prxMap(s)");
	for (map<int, vector<ILikeItCachePrx> >::iterator it0 = prxs.begin(); 
			it0 != prxs.end(); ++it0)   {
		MCE_DEBUG("[ILikeItCacheAdapter::loadSync] need reload " << (*it0).second.size() << " prx(s)");
		for (vector<ILikeItCachePrx>::iterator it1 = (*it0).second.begin();
				it1 != (*it0).second.end(); ++it1 ) {
			try {
				(*it1)->load(gids);
			} catch (Ice::Exception& e) {
				MCE_WARN("[ILikeItCacheAdapter::loadSync] load for " << (*it1)->ice_toString() << " Ice::Exception : " << e.what());
				continue;
			} catch (std::exception& e) {
				MCE_WARN("[ILikeItCacheAdapter::loadSync] load for " << (*it1)->ice_toString() << " std::exception : " << e.what());
				continue;
			} catch (...) {
				MCE_WARN("[ILikeItCacheAdapter::loadSync] load for " << (*it1)->ice_toString() << " Exception");
				continue;
			}
			MCE_INFO("[ILikeItCacheAdapter::loadSync] reload " << gids.size() << " for " << (*it1)->ice_toString());
		}
	}
}
