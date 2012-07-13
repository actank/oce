/*
 * =====================================================================================
 *
 *       Filename:  LoveFootprintByInterviewerAdapter.cpp
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

#include "LoveFootprintByInterviewerAdapter.h"
#include "FootprintFacadeAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>

using namespace std;
using namespace xce::footprint;
using namespace MyUtil;

//---------------------------------------------------------------------------

LoveFootprintByInterviewerAdapter::LoveFootprintByInterviewerAdapter() :
	_clientCS("ControllerLoveFootprintByInterviewer", 120, 300) {
	
	_clientCS.intialize();
	_cluster = _clientCS.getCluster();
	
	MCE_INFO("get controllerlovefootprintbyinterviewer size:"<<_cluster);
}

void LoveFootprintByInterviewerAdapter::visit(const FootprintInfoPtr& fpt) {
	if ( fpt ) {
		FootprintInfoPtr tmp = new FootprintInfo();
		tmp->owner = fpt->visitor;
		tmp->visitor = fpt->owner;
		tmp->gid = fpt->gid;
		tmp->timestamp = fpt->timestamp;
		CommonFootprintCachePrx prxOneway = _clientCS.getProxy(tmp->owner)->ice_oneway();
		try {
			prxOneway->visit(tmp);
		} catch (Ice::Exception& e){
			MCE_WARN("[LoveFootprintByInterviewerAdapter::visitVisitpage.Twoway] fpt " << tmp->visitor << "->" << tmp->owner << " Ice::Exception : " << e.what());
		} catch (std::exception& e){
			MCE_WARN("[LoveFootprintByInterviewerAdapter::LoveLovepage.Twoway] fpt " << tmp->visitor << "->" << tmp->owner << " std::exception : " << e.what());
		} catch (...){
			MCE_WARN("[LoveFootprintByInterviewerAdapter::LoveLovepage.Twoway] fpt " << tmp->visitor << "->" << tmp->owner << " UNKNOWN exception");
		}
	}
}

