#include "ControllerAdapter.h"
#include "IceLogger.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace xce::clustercontroller;
using namespace std;

//---------------------------------------------------------------------------
xce::clusterstate::ClusterStateInfoPtr ControllerAdapter::getClusterState() {

	ControllerManagerPrx prx = getControllerManager();
	if ( prx ) {
		try {
			return prx->GetServerState();
		} catch (Ice::Exception& e) {
			MCE_WARN("[ControllerAdapter::getClusterStateInfo] get ServerState Ice::Exception : " << e.what());
			return NULL;
		} catch (...) {
			MCE_WARN("[ControllerAdapter::getClusterStateInfo] get ServerState exception");
			return NULL;
		}
	}
	else {
		return NULL;
	}
}

int ControllerAdapter::getCluster() {
	
	xce::clusterstate::ClusterStateInfoPtr state = getClusterState();
	if ( state ) {
		if ( state->states.size() > 0 ) {
			return (*(state->states[0].begin()))->cluster;
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}

ControllerManagerPrx ControllerAdapter::getControllerManager() {

	IceUtil::Mutex::Lock lock(*this);	
	if ( prx_ ) {
		return prx_;
	} else {
		try {
			Ice::ObjectPrx prxObject = channel_.locate("M@", endpoints_, TWO_WAY, timeout_);
			prx_ = ControllerManagerPrx::uncheckedCast(prxObject); 
		} catch (Ice::Exception& e) {
			MCE_WARN("[ControllerAdapter::getControllerManager] get proxy of " 
					<< endpoints_ << " Ice::Exception : " << e.what());
			prx_ = NULL;
		} catch (...) {
			MCE_WARN("[ControllerAdapter::getControllerManager] get proxy of " 
					<< endpoints_ << " exception");
			prx_ = NULL;
		}
		return prx_;
	}
}


