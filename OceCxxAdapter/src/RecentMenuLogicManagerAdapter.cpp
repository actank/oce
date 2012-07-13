#include "RecentMenuLogicManagerAdapter.h"
#include <iostream>
#include <Ice/Ice.h>

using namespace com::xiaonei::service;
using namespace xce::menu;
using namespace std;

//---------------------------------------------------------------------------
RecentMenuLogicManagerPrx RecentMenuLogicManagerAdapter::getRecentMenuLogicManager() {	
	return locate<RecentMenuLogicManagerPrx>(name(), endpoints(), TWO_WAY, 300);
}

RecentMenuLogicManagerPrx RecentMenuLogicManagerAdapter::getRecentMenuLogicManagerOneway() {
	return locate<RecentMenuLogicManagerPrx>(name(), endpoints(), ONE_WAY, 300);
}

UserId2AppIdSeqMap RecentMenuLogicManagerAdapter::get(UserIdSeq userIds) {
	
	return getRecentMenuLogicManager()->get(userIds);
}

void RecentMenuLogicManagerAdapter::preload(const MenuCacheManagerPrx& menuCacheManager,int mod,int cluster) {
	Ice::Context ctx;

	ostringstream oss;
	oss<<mod;
	ctx["mod"]=oss.str();

	ostringstream os;
	os<<cluster;
	ctx["cluster"]=os.str();
	try{
	//	cout << "RecentMenuLogicManagerAdapter::preload,isValid" << menuCacheManager->isValid() << endl;
		getRecentMenuLogicManagerOneway()->preload(menuCacheManager,ctx);
	}catch(Ice::Exception& e){
		MCE_WARN("RecentMenuLogicManagerAdapter::preload  " << e.what());
	}
}
