#include "UserApiCacheReloaderAdapter.h"

using namespace xce::userapicache;

xce::userapicache::UserApiCacheReloaderAdapter::UserApiCacheReloaderAdapter() : MyUtil::ReplicatedClusterAdapterI <xce::userapicache::UserApiCacheReloaderPrx>("ControllerUserApiCacheReloader", 120, 300) {
}

void UserApiCacheReloaderAdapter::reload(MyUtil::IntSeq userIds, const Ice::Context& ctx) {
	if(userIds.size() > 0) {
		getManager( 0 )->reload(userIds);
	}
}

UserApiCacheReloaderPrx UserApiCacheReloaderAdapter::getManager(int id) {
	return getProxy(id);	
}
