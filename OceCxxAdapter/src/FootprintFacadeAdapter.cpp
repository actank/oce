/*
 * =====================================================================================
 *
 *       Filename:  FootprintFacadeAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年09月07日 11时59分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */
#include "FootprintFacadeAdapter.h"
#include "boost/lexical_cast.hpp"
#include "IceExt/src/Channel.h"
#include "ObjectCache.h"
#include "OceCxxAdapter/src/BatchUpdaterReplicaAdapter.h"
#include <iostream>

using namespace std;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace xce::footprint;

FootprintFacadeAdapter::FootprintFacadeAdapter() :
	_clientCS("ControllerFootprintFacade", 120, 300) {
	
	_clientCS.intialize();
	_cluster = _clientCS.getCluster();
	
	MCE_INFO("get controllerfootprintFacade size:"<<_cluster);
}

void FootprintFacadeAdapter::interview(const FootprintInfoPtr& fpt) {
	
	if ( fpt ) {
		FootprintFacadeManagerPrx prxOneway = _clientCS.getProxy(fpt->owner)->ice_oneway();
		try {
			prxOneway->interview(fpt);
		} catch (Ice::Exception& e){
			MCE_WARN("[FootprintFacadeAdapter::interview.Oneway] fpt " << fpt->visitor << "->" << fpt->owner << " Ice::Exception : " << e.what());
		} catch (std::exception& e){
			MCE_WARN("[FootprintFacadeAdapter::interview.Oneway] fpt " << fpt->visitor << "->" << fpt->owner << " std::exception : " << e.what());
		} catch (...){
			MCE_WARN("[FootprintFacadeAdapter::interview.Oneway] fpt " << fpt->visitor << "->" << fpt->owner << " UNKNOWN exception");
		}
	}
}

