/*
 * =====================================================================================
 *
 *       Filename:  LoveFootprintByIntervieweeAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年09月17日 17时44分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "LoveFootprintByIntervieweeAdapter.h"
#include "FootprintFacadeAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>

using namespace std;
using namespace xce::footprint;
using namespace MyUtil;

//---------------------------------------------------------------------------

LoveFootprintByIntervieweeAdapter::LoveFootprintByIntervieweeAdapter() :
	_clientCS("ControllerLoveFootprintByInterviewee", 120, 300) {
	
	_clientCS.intialize();
	_cluster = _clientCS.getCluster();
	
	MCE_INFO("get controllerlovefootprintbyinterviewee size:"<<_cluster);
}

void LoveFootprintByIntervieweeAdapter::visit(const FootprintInfoPtr& fpt) {
	if ( fpt ) {
		CommonFootprintCachePrx prxOneway = _clientCS.getProxy(fpt->owner)->ice_twoway();
		try {
			prxOneway->visit(fpt);
		} catch (Ice::Exception& e){
			MCE_WARN("[LoveFootprintByIntervieweeAdapter::visitVisitpage.Twoway] fpt " << fpt->visitor << "->" << fpt->owner << " Ice::Exception : " << e.what());
		} catch (std::exception& e){
			MCE_WARN("[LoveFootprintByIntervieweeAdapter::LoveLovepage.Twoway] fpt " << fpt->visitor << "->" << fpt->owner << " std::exception : " << e.what());
		} catch (...){
			MCE_WARN("[LoveFootprintByIntervieweeAdapter::LoveLovepage.Twoway] fpt " << fpt->visitor << "->" << fpt->owner << " UNKNOWN exception");
		}
	}
}

