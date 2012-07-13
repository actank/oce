#include "FavoriteMenuLogicManagerAdapter.h"
#include <iostream>
#include <Ice/Ice.h>
#include <sstream>
using namespace com::xiaonei::service;
using namespace ::xce::menu;
using namespace std;
using namespace MyUtil;
//---------------------------------------------------------------------------
FavoriteMenuLogicManagerPrx FavoriteMenuLogicManagerAdapter::getFavoriteMenuLogicManager() {	
	return locate<FavoriteMenuLogicManagerPrx>(name(), endpoints(), TWO_WAY,300);
}

FavoriteMenuLogicManagerPrx FavoriteMenuLogicManagerAdapter::getFavoriteMenuLogicManagerOneway() {
	return locate<FavoriteMenuLogicManagerPrx>(name(), endpoints(), ONE_WAY,300);
}

UserId2AppIdSeqMap FavoriteMenuLogicManagerAdapter::get(UserIdSeq userIds) {

	return getFavoriteMenuLogicManager()->get(userIds);
}

void FavoriteMenuLogicManagerAdapter::preload(const MenuCacheManagerPrx& menuCacheManager,int mod,int cluster) {
	Ice::Context ctx;
	ostringstream oss;
	oss<<mod;
	ctx["mod"]=oss.str();

	ostringstream os;
	os<<cluster;
	ctx["cluster"]=os.str();

	try{
		getFavoriteMenuLogicManagerOneway()->preload(menuCacheManager,ctx);
	}catch(Ice::Exception e){
		MCE_WARN("FavoriteMenuLogicManagerAdapter::preload" << e.what());
	}
}

