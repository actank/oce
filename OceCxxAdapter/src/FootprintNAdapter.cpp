#include "FootprintNAdapter.h"
#include "boost/lexical_cast.hpp"
#include "IceExt/src/Channel.h"
#include "ObjectCache.h"
#include "OceCxxAdapter/src/BatchUpdaterReplicaAdapter.h"
#include <iostream>

using namespace xce::fptn::adapter;
using namespace std;
using namespace xce::clusterstate;
using namespace MyUtil;

//---------------------------------------------------------------------------
FootprintNAdapter::FootprintNAdapter() :
	_clientCS("ControllerFootprint", 120, 100) {
	
	_clientCS.intialize();
	_cluster = _clientCS.getCluster();
	
}

void FootprintNAdapter::visitHomepageOneway(const xce::fptn::FootprintPtr& fpt, map<string, string> ctx) {
	
	if ( fpt ) {
		// 获取所有validPrx和invalid的列表
		vector<xce::fptn::FootprintManagerPrx> prxs = _clientCS.getAllProxySeqOneway(fpt->host);
		for (vector<xce::fptn::FootprintManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
			if ( (*itPrx) ) {
				try {
					(*itPrx)->visitHomepageOneway(fpt, ctx);
				} catch (Ice::Exception& e){
					MCE_WARN("[FootprintNAdapter::visitHomepageOneway] fpt " << fpt->tinyinfo->guest << "->" << fpt->host << " Ice::Exception : " << e.what());
				} catch (std::exception& e){
					MCE_WARN("[FootprintNAdapter::visitHomepageOneway] fpt " << fpt->tinyinfo->guest << "->" << fpt->host << " std::exception : " << e.what());
				} catch (...){
					MCE_WARN("[FootprintNAdapter::visitHomepageOneway] fpt " << fpt->tinyinfo->guest << "->" << fpt->host << " UNKNOWN exception");
				}
			} else {
				MCE_WARN("[FootprintNAdapter::visitHomepageOneway] 1 prx of " << fpt->host << " of " << prxs.size() << " is NULL");
			}
		}
	} else {
		MCE_WARN("[FootprintNAdapter::visitHomepageOneway] footprint-null pointer");
	}
}

xce::fptn::FootprintInfoPtr FootprintNAdapter::visitHomepage(const xce::fptn::FootprintPtr& fpt, map<string, string> ctx) {
	return visitHomepage(fpt, 0, 6, ctx);
}

xce::fptn::FootprintInfoPtr FootprintNAdapter::visitHomepage(const xce::fptn::FootprintPtr& fpt, int begin, int limit, map<string, string> ctx) {
	
	xce::fptn::FootprintInfoPtr result = NULL;
	bool checkIndentity = false;
	::Ice::Identity twowayIdentity;
		

	if ( fpt ) {
		// 获取1个Twoway的prx, 踩脚印并获得结果
		xce::fptn::FootprintManagerPrx prxTwoway = _clientCS.getProxy(fpt->host);
		if ( prxTwoway ) {
			try {
				twowayIdentity = prxTwoway->ice_getIdentity();
				checkIndentity = true;
				result = prxTwoway->visitHomepage(fpt, begin, limit, ctx);
			} catch (Ice::Exception& e){
				MCE_WARN("[FootprintNAdapter::visitHomepage.Twoway] fpt " << fpt->tinyinfo->guest << "->" << fpt->host << " Ice::Exception : " << e.what());
			} catch (std::exception& e){
				MCE_WARN("[FootprintNAdapter::visitHomepage.Twoway] fpt " << fpt->tinyinfo->guest << "->" << fpt->host << " std::exception : " << e.what());
			} catch (...){
				MCE_WARN("[FootprintNAdapter::visitHomepage.Twoway] fpt " << fpt->tinyinfo->guest << "->" << fpt->host << " UNKNOWN exception");
			}
		} else {
			MCE_WARN("[FootprintNAdapter::visitHomepage.Twoway] twoway prx of " << fpt->host << " is NULL");
		}

		// 获取所有Oneway的validPrx和invalid的列表
		vector<xce::fptn::FootprintManagerPrx> prxs = _clientCS.getAllProxySeqOneway(fpt->host);
		if ( prxs.size() > 1 ) {
			for (vector<xce::fptn::FootprintManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx) {
				if ( (*itPrx) ) {
					try {
						if ( checkIndentity ) {
							if ((*itPrx)->ice_getIdentity() == twowayIdentity ) {
								continue;
							}
						}
						(*itPrx)->visitHomepageOneway(fpt, ctx);
					} catch (Ice::Exception& e){
						MCE_WARN("[FootprintNAdapter::visitHomepageOneway] fpt " << fpt->tinyinfo->guest << "->" << fpt->host << " Ice::Exception : " << e.what());
					} catch (...){
						MCE_WARN("[FootprintNAdapter::visitHomepageOneway] fpt " << fpt->tinyinfo->guest << "->" << fpt->host << " UNKNOWN exception");
					}
				} else {
					MCE_WARN("[FootprintNAdapter::visitHomepageOneway] 1 prx of " << fpt->host << " of " << prxs.size() << " is NULL");
				}			
			}
		}
	} else {
		MCE_WARN("[FootprintNAdapter::visitHomepageOneway] footprint-null pointer");
	}

	if ( !result ) {
		result = new xce::fptn::FootprintInfo;
		result->visitcount = 0;
	}

	return result;
}


int FootprintNAdapter::getSize(int uid){
	try {	
		return _clientCS.getProxy(uid)->getSize(uid);
	} catch (Ice::Exception& e){
		MCE_WARN("[FootprintNAdapter::getSize] " << uid << " Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_WARN("[FootprintNAdapter::getSize] " << uid << " std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[FootprintNAdapter::getSize] " << uid << " UNKNOWN exception");
	}
	return 0;
}

int FootprintNAdapter::getUserVisitCount(int uid){
	try {	
		return _clientCS.getProxy(uid)->getUserVisitCount(uid);
	} catch (Ice::Exception& e){
		MCE_WARN("[FootprintNAdapter::getUserVisitCount] " << uid << " Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_WARN("[FootprintNAdapter::getUserVisitCount] " << uid << " std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[FootprintNAdapter::getUserVisitCount] " << uid << " UNKNOWN exception");
	}
	return 0;
}

xce::fptn::FootprintInfoPtr FootprintNAdapter::getAll(int uid,int begin,int limit){

	xce::fptn::FootprintInfoPtr result = new xce::fptn::FootprintInfo;
	result->visitcount = 0;
	try {			
		result = _clientCS.getProxy(uid)->getAll(uid,begin,limit);
	
	} catch (Ice::Exception& e){
		MCE_WARN("[FootprintNAdapter::getAll] " << uid << " Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_WARN("[FootprintNAdapter::getAll] " << uid << " std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[FootprintNAdapter::getAll] " << uid << " exception");
	}
	return result;
}

void FootprintNAdapter::reloadCache(int category, int uid) {

	ostringstream endpoints;
	endpoints << "@FootprintN" << uid % 10;
	reloadCache(category, endpoints.str(), uid);
}

void FootprintNAdapter::reloadCache(int category, const string& endpoints, int uid) {
	if ( endpoints.length() <= 0 ) {
		return;
	}
	string endpointsN;
	if ( endpoints[0] != '@' ) {
		endpointsN = string("@") + endpoints;
	} else {
		endpointsN = endpoints;
	}
	
	ObjectCachePrx cachePrx = ObjectCachePrx::uncheckedCast(_channel.locate("SC", endpointsN, TWO_WAY, 300));
	cachePrx->reloadObject(category, uid);
}

