/*
 * =====================================================================================
 *
 *       Filename:  FootprintViewAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年07月07日 18时28分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "FootprintViewAdapter.h"

using namespace std;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace xce::footprint;

FootprintViewAdapter::FootprintViewAdapter() : _clientCS("ControllerFootprintView", 120, 300) {
	_clientCS.intialize();
	_cluster = _clientCS.getCluster();
}

bool FootprintViewAdapter::verifyVisitCount(const Int2IntMap& counts) {
	std::map<int, Int2IntMap> tmpSeq;

	try {
		for (map<int, int>::const_iterator it = counts.begin(); it != counts.end(); ++it) {
			tmpSeq[it->first%CLUSTER_SIZE][it->first]=it->second;
		}
	} catch (...) {
			MCE_WARN("FootprintViewAdapter::verifyVisitCount Exception");
	}

	bool return_value = true;
	for(uint i = 0; i < tmpSeq.size(); i++) {
		FootprintViewManagerPrx prxTwoway = _clientCS.getProxy(i)->ice_twoway();
		try {
			if (!tmpSeq[i].empty()) {
				bool value = prxTwoway->verifyVisitCount(tmpSeq[i]);
				if (!value) {
					return_value = false;
				}
			}
		} catch (...) {
			MCE_WARN("FootprintViewAdapter::verifyVisitCount Exception");
		}
	}
	return return_value;
}

void FootprintViewAdapter::setVisitCountLimit(int owner) {
	try {
		FootprintViewManagerPrx prxTwoway = _clientCS.getProxy(owner)->ice_twoway();
		prxTwoway->setVisitCountLimit(owner);
	} catch (...) {
		MCE_WARN("FootprintViewAdapter::setVisitCountLimit Exception, id: " << owner);
	}
}
