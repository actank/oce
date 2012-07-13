#include "XceAlertAdapter.h"

using com::xiaonei::xce::XceAlertAdapter;

XceAlertAdapter::XceAlertAdapter() : MyUtil::ReplicatedClusterAdapterI <XceAlertPrx>("ControllerXceAlert", 120, 300) {
}

void XceAlertAdapter::notifyAlert(const MyUtil::StrSeq& typeSeq, const std::string& msg) {
	XceAlertPrx prx = getProxy(0);
	if(prx)
	{
		try {
			getProxy(0)->notifyAlert(typeSeq, msg);
		} catch (const Ice::Exception& e) {
			MCE_WARN("XceAlertAdapter::notifyAlert Ice::Exception " << e);
		}
	}else
	{ 
		MCE_WARN("XceAlertAdapter::notifyAlert no prx");
	}
}

void XceAlertAdapter::notifyAlertN(const MyUtil::StrSeq& typeSeq, const std::string& msg, int sendType) {
	try {
		getProxy(0)->notifyAlertN(typeSeq, msg, sendType);
	} catch (const Ice::Exception& e) {
		MCE_WARN("XceAlertAdapter::notifyAlertN Ice::Exception " << e);
	}
}

void XceAlertAdapter::reloadAlertMap() {
	try {
		getProxy(0)->reloadAlertMap();
	} catch (const Ice::Exception& e) {
		MCE_WARN("XceAlertAdapter::reload Ice::Exception " << e);
	}
}	
