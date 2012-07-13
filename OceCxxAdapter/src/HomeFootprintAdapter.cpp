#include "HomeFootprintAdapter.h"
#include "boost/lexical_cast.hpp"
#include "IceExt/src/Channel.h"
#include "ObjectCache.h"
#include "OceCxxAdapter/src/BatchUpdaterReplicaAdapter.h"
#include <iostream>

using namespace std;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace xce::footprint;

//---------------------------------------------------------------------------
HomeFootprintAdapter::HomeFootprintAdapter() :
	_clientCS("ControllerFootprintUnite", 120, 300) {
	
	_clientCS.intialize();
	_cluster = _clientCS.getCluster();
	
	MCE_INFO("get controllerfootprintUnite size:"<<_cluster);
}

HomeVisitHistoryPtr HomeFootprintAdapter::getHomeFootprintObj(long id, int category) {
  try {
    return _clientCS.getProxy(id)->getHomeFootprintObj(id, category);
  } catch (Ice::Exception& e){
    MCE_WARN("[HomeFootprintAdapter::getHomeFootprintObj] " << id << " Ice::Exception : " << e.what());
  } catch (std::exception& e){
    MCE_WARN("[HomeFootprintAdapter::getHomeFootprintObj] " << id << " std::exception : " << e.what());
  } catch (...){
    MCE_WARN("[HomeFootprintAdapter::getHomeFootprintObj] " << id << " UNKNOWN exception");
  }
  return 0;
}

UserVisitCountPtr HomeFootprintAdapter::getUserVisitCountObj(int id) {
  try {
    return _clientCS.getProxy(id)->getUserVisitCountObj(id);
  } catch (Ice::Exception& e){
    MCE_WARN("[HomeFootprintAdapter::getUserVisitCountObj] " << id << " Ice::Exception : " << e.what());
  } catch (std::exception& e){
    MCE_WARN("[HomeFootprintAdapter::getUserVisitCountObj] " << id << " std::exception : " << e.what());
  } catch (...){
    MCE_WARN("[HomeFootprintAdapter::getUserVisitCountObj] " << id << " UNKNOWN exception");
  }
  return 0;
}

void HomeFootprintAdapter::visit(const FootprintInfoPtr& fpt, const map<string, string>& ctx) {
	
	if ( fpt ) {
		HomeFootprintCachePrx prxOneway = _clientCS.getProxy(fpt->owner)->ice_oneway();
		try {
			prxOneway->visit(fpt, ctx);
		} catch (Ice::Exception& e){
			MCE_WARN("[HomeFootprintAdapter::visitHomepage.Twoway] fpt " << fpt->visitor << "->" << fpt->owner << " Ice::Exception : " << e.what());
		} catch (std::exception& e){
			MCE_WARN("[HomeFootprintAdapter::visitHomepage.Twoway] fpt " << fpt->visitor << "->" << fpt->owner << " std::exception : " << e.what());
		} catch (...){
			MCE_WARN("[HomeFootprintAdapter::visitHomepage.Twoway] fpt " << fpt->visitor << "->" << fpt->owner << " UNKNOWN exception");
		}
	}
}

void HomeFootprintAdapter::remove(const FootprintInfoPtr& fpt, const map<string, string>& current) {
	
	if ( fpt ) {
		HomeFootprintCachePrx prxOneway = _clientCS.getProxy(fpt->owner)->ice_oneway();
		try {
			prxOneway->remove(fpt, current);
		} catch (Ice::Exception& e){
			MCE_WARN("[HomeFootprintAdapter::visitHomepage.Oneway] fpt " << fpt->visitor << "->" << fpt->owner << " Ice::Exception : " << e.what());
		} catch (std::exception& e){
			MCE_WARN("[HomeFootprintAdapter::visitHomepage.Oneway] fpt " << fpt->visitor << "->" << fpt->owner << " std::exception : " << e.what());
		} catch (...){
			MCE_WARN("[HomeFootprintAdapter::visitHomepage.Oneway] fpt " << fpt->visitor << "->" << fpt->owner << " UNKNOWN exception");
		}
	}
}

HomeInfoPtr HomeFootprintAdapter::visitAndGet(const FootprintInfoPtr& fpt, int begin, int nLimit, int tLimit, const map<string, string>& ctx) {
	
	HomeInfoPtr result = NULL;
	if ( fpt ) {
		HomeFootprintCachePrx prxTwoway = _clientCS.getProxy(fpt->owner);
		try {
			result = prxTwoway->visitAndGet(fpt, begin, nLimit, tLimit, ctx);
		} catch (Ice::Exception& e){
			MCE_WARN("[HomeFootprintAdapter::visitHomepage.Twoway] fpt " << fpt->visitor << "->" << fpt->owner << " Ice::Exception : " << e.what());
		} catch (std::exception& e){
			MCE_WARN("[HomeFootprintAdapter::visitHomepage.Twoway] fpt " << fpt->visitor << "->" << fpt->owner << " std::exception : " << e.what());
		} catch (...){
			MCE_WARN("[HomeFootprintAdapter::visitHomepage.Twoway] fpt " << fpt->visitor << "->" << fpt->owner << " UNKNOWN exception");
		}
	}

	if ( !result ) {
		result = new HomeInfo;
		result->visitcount = 0;
	}
	return result;
}


int HomeFootprintAdapter::getSize(int uid){
	try {	
		return _clientCS.getProxy(uid)->getSize(uid);
	} catch (Ice::Exception& e){
		MCE_WARN("[HomeFootprintAdapter::getSize] " << uid << " Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_WARN("[HomeFootprintAdapter::getSize] " << uid << " std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[HomeFootprintAdapter::getSize] " << uid << " UNKNOWN exception");
	}
	return 0;
}

int HomeFootprintAdapter::getUserVisitCount(int uid){
	try {	
		return _clientCS.getProxy(uid)->getUserVisitCount(uid);
	} catch (Ice::Exception& e){
		MCE_WARN("[HomeFootprintAdapter::getUserVisitCount] " << uid << " Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_WARN("[HomeFootprintAdapter::getUserVisitCount] " << uid << " std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[HomeFootprintAdapter::getUserVisitCount] " << uid << " UNKNOWN exception");
	}
	return 0;
}

void HomeFootprintAdapter::setUserVisitCount(int uid, int count){
	try {	
		_clientCS.getProxy(uid)->setUserVisitCount(uid, count);
	} catch (Ice::Exception& e){
		MCE_WARN("[HomeFootprintAdapter::setUserVisitCount] " << uid << " Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_WARN("[HomeFootprintAdapter::setUserVisitCount] " << uid << " std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[HomeFootprintAdapter::setUserVisitCount] " << uid << " UNKNOWN exception");
	}
}

HomeInfoPtr HomeFootprintAdapter::getAll(int uid,int begin,int nLimit,int tLimit){

	HomeInfoPtr result = new HomeInfo;
	result->visitcount = 0;
	try {			
		result = _clientCS.getProxy(uid)->getAll(uid,begin,nLimit,tLimit);
	
	} catch (Ice::Exception& e){
		MCE_WARN("[HomeFootprintAdapter::getAll] " << uid << " Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_WARN("[HomeFootprintAdapter::getAll] " << uid << " std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[HomeFootprintAdapter::getAll] " << uid << " exception");
	}
	return result;
}


