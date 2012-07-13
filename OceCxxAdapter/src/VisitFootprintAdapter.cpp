/*
 * =====================================================================================
 *
 *       Filename:  VisitFootprintAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年08月11日 17时31分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "VisitFootprintAdapter.h"
#include "FootprintFacadeAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>

using namespace std;
using namespace xce::footprint;
using namespace MyUtil;

//---------------------------------------------------------------------------

VisitFootprintAdapter::VisitFootprintAdapter() :
	_clientCS("ControllerVisitFootprint", 120, 300) {
	
	_clientCS.intialize();
	_cluster = _clientCS.getCluster();
	
	MCE_INFO("get controllerVisitfootprint size:"<<_cluster);
}

void VisitFootprintAdapter::visit(FootprintInfoPtr fpt) {
	if ( fpt ) {
		swap(fpt->visitor,fpt->owner);
		UgcFootprintCachePrx prxOneway = _clientCS.getProxy(fpt->owner)->ice_oneway();
		try {
			prxOneway->visit(fpt);
			FootprintFacadeAdapter::instance().interview(fpt);
		} catch (Ice::Exception& e){
			MCE_WARN("[VisitFootprintAdapter::visitVisitpage.Twoway] fpt " << fpt->visitor << "->" << fpt->owner << " Ice::Exception : " << e.what());
		} catch (std::exception& e){
			MCE_WARN("[VisitFootprintAdapter::visitVisitpage.Twoway] fpt " << fpt->visitor << "->" << fpt->owner << " std::exception : " << e.what());
		} catch (...){
			MCE_WARN("[VisitFootprintAdapter::visitVisitpage.Twoway] fpt " << fpt->visitor << "->" << fpt->owner << " UNKNOWN exception");
		}
	}
}

int VisitFootprintAdapter::getSize(Ice::Long id) {
	UgcFootprintCachePrx prxTwoway = _clientCS.getProxy(id)->ice_twoway();
	try {
		return prxTwoway->getSize(id);
	} catch (...){
		MCE_WARN("[VisitFootprintAdapter::getSize.Twoway] UNKNOWN exception");
	}
}
