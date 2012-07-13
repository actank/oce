/*
 * ILikeItCacheAdapter.cpp
 *
 *  Created on: 2010-01-28
 *      Author: zhanghan
 */

#include "OceCxxAdapter/src/MenuCacheReplicaAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace MyUtil;
using namespace xce::clusterstate;
using namespace xce::menu;

// ========= MenuCacheReplicaAdapter =================================
MenuCacheReplicaAdapter::MenuCacheReplicaAdapter() :	
	_clientCS("ControllerMenuCacheN", 120, 300) {
	_clientCS.intialize();
	_cluster = CACHE_CLUSTER;
}

bool MenuCacheReplicaAdapter::isValid(long id){
	try {
		return _clientCS.getProxy(id)->isValid();
	}catch (Ice::Exception& e) {
                MCE_WARN("[MenuCacheReplicaAdapter::isValid] " << ":" << id  << " Ice::Exception : " << e.what());
        } catch (std::exception& e) {
                MCE_WARN("[MenuCacheReplicaAdapter::isValid] " << ":" << id  << " std::excetpion : " << e.what());
        } catch (...) {
                MCE_WARN("[MenuCacheReplicaAdapter::isValid] " << ":" << id << " exception");
        }
	
	return false;
}

void MenuCacheReplicaAdapter::setData(const ::MyUtil::ObjectResultPtr& objects, int mod, MenuCacheManagerPrx& localPrx, CacheType type) {

	// 获取该mod的所有prx
//	map<int, vector<MenuCacheManagerPrx> > prxsMap = _clientCS.getAvailableProxies();
//	map<int, vector<MenuCacheManagerPrx> >::iterator itSeq = prxsMap.find(mod);
	bool isValid = false;
	try{
		isValid = localPrx->isValid();
	}catch(...){
		return;
	}
	vector<MenuCacheManagerPrx> itSeq ;
	if(isValid){
		itSeq = _clientCS.getProxySeq(mod);
	}else{
		itSeq = _clientCS.getInvalidProxySeq(mod);
	}
	

	if ( itSeq.size() == 0){ 
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
	for (vector<MenuCacheManagerPrx>::iterator it = itSeq.begin(); it != itSeq.end(); ++it) {
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
			(*it)->setData(objects,type);
			++setEd;
			MCE_DEBUG("[MenuCacheReplicaAdapter::setData] setData " << objects->data.size() << "(s) for " << (*it)->ice_toString() << " OK");
		} catch (exception& ex){
			MCE_WARN("[MenuCacheReplicaAdapter::setData] setData " << objects->data.size() << "(s) for " << (*it)->ice_toString() << " exception:" << ex.what());
		} catch (...) {
			MCE_WARN("[MenuCacheReplicaAdapter::setData] setData " << objects->data.size() << "(s) for " << (*it)->ice_toString() << " UNKNOWN exception");
		}
	}
	MCE_INFO("[[MenuCacheReplicaAdapter::setData] setData " << objects->data.size() << "(s) for " << setEd << " prxs");

}

