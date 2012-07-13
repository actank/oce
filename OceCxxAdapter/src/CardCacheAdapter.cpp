/*
 * QuestCacheAdapter.h
 *
 *  Created on: 2009-12-24
 *      Author: zhanghan
 */

#include "OceCxxAdapter/src/QuestCacheAdapter.h"

using namespace xce::quest;

QuestCacheAdapter::QuestCacheAdapter():
		clientCS_("ControllerQuestCache", 120, 300) {
	
	managersOneway_.resize(DEFAULT_CLUSTER > 0 ? DEFAULT_CLUSTER : 1);
	managersDatagram_.resize(DEFAULT_CLUSTER > 0 ? DEFAULT_CLUSTER : 1);
	clientCS_.intialize();
}
QuestCachePrx QuestCacheAdapter::getQuestCacheDatagram(int id){
	return locate<QuestCachePrx>(managersDatagram_, "M", id, MyUtil::DATAGRAM);
}
QuestCachePrx QuestCacheAdapter::getQuestCacheOneway(int id) {
	return locate<QuestCachePrx>(managersOneway_, "M", id, MyUtil::ONE_WAY);
}

bool QuestCacheAdapter::newFlow(int userid) {

	try {
		clientCS_.getProxy(userid)->newFlow(userid);			
	} catch (Ice::Exception& e) {
		MCE_WARN("[QuestCacheAdapter::newFlow] create new tip-flow of " << userid<<" Ice::Exception : "<<e.what());
		return false;
	} catch (...) {
		MCE_WARN("[QuestCacheAdapter::newFlow] create new tip-flow of " << userid<<" unknown exception");
		return false;
	}
	return true;
}

TipPtr QuestCacheAdapter::showTip(int userid) {

	try {
		return clientCS_.getProxy(userid)->show(userid);
	} catch (Ice::Exception& e) {
		MCE_WARN("[QuestCacheAdapter::showTip] show tip of " << userid << " Ice::Exception : "<<e.what());
		return NULL;
	} catch (std::exception& e) {
		MCE_WARN("[QuestCacheAdapter::showTip] show tip of " << userid << " std::exception : "<<e.what());
		return NULL;
	} catch (...) {
		MCE_WARN("[QuestCacheAdapter::showTip] show tip of " << userid << " unknown exception");
		return NULL;
	}
}

void QuestCacheAdapter::click(int tipid, int userid) {

	try {
		clientCS_.getProxy(userid)->click(tipid, userid);
	} catch (Ice::Exception& e) {
		MCE_WARN("[QuestCacheAdapter::click] " << userid << " click tip " << tipid << " Ice::Exception : "<<e.what());
	} catch (std::exception& e) {
		MCE_WARN("[QuestCacheAdapter::click] " << userid << " click tip " << tipid << " std::exception : "<<e.what());
	} catch (...) {
		MCE_WARN("[QuestCacheAdapter::click] " << userid << " click tip " << tipid << " unknown exception");
	}
}

void QuestCacheAdapter::renewTips() {

	map<int, vector<QuestCachePrx> > prxs = clientCS_.getAvailableProxies();
	for ( map<int, vector<QuestCachePrx> >::iterator it = prxs.begin(); it != prxs.end(); ++it ) {
		for (vector<QuestCachePrx>::iterator itPrx = (*it).second.begin(); itPrx != (*it).second.end(); ++itPrx) {
			try {
				(*itPrx)->renewTips();
			} catch (Ice::Exception& e) {
				MCE_WARN("[QuestCacheAdapter::renewTips] renewTip " << (*itPrx)->ice_toString() 
					<< " Ice::Exception : "<<e.what());
			} catch (std::exception& e) {
				MCE_WARN("[QuestCacheAdapter::renewTips] renewTip " << (*itPrx)->ice_toString() 
					<< " std::exception : "<<e.what());
			} catch (...) {
				MCE_WARN("[QuestCacheAdapter::renewTips] renewTip " << (*itPrx)->ice_toString() 
					<< " unknown exception");
			}
		}
	}
}

void QuestCacheAdapter::renewFlow() {

	map<int, vector<QuestCachePrx> > prxs = clientCS_.getAvailableProxies();
	for ( map<int, vector<QuestCachePrx> >::iterator it = prxs.begin(); it != prxs.end(); ++it ) {
		for (vector<QuestCachePrx>::iterator itPrx = (*it).second.begin(); itPrx != (*it).second.end(); ++itPrx) {
			try {
				(*itPrx)->renewFlow();
			} catch (Ice::Exception& e) {
				MCE_WARN("[QuestCacheAdapter::renewFlow] renewTip " << (*itPrx)->ice_toString() 
					<< " Ice::Exception : "<<e.what());
			} catch (std::exception& e) {
				MCE_WARN("[QuestCacheAdapter::renewFlow] renewTip " << (*itPrx)->ice_toString() 
					<< " std::exception : "<<e.what());
			} catch (...) {
				MCE_WARN("[QuestCacheAdapter::renewFlow] renewTip " << (*itPrx)->ice_toString() 
					<< " unknown exception");
			}
		}
	}
}

void QuestCacheAdapter::responseUrl(int userid,const string& url){

		getQuestCacheDatagram(userid)->responseUrl(userid, url);
}
