#include "DistUserApiCacheReloaderAdapter.h"


xce::distcache::userapicache::DistUserApiCacheReloaderAdapter::DistUserApiCacheReloaderAdapter() : MyUtil::ReplicatedClusterAdapterI <xce::userapicache::UserApiCacheReloaderPrx>("ControllerDistUserApiCacheReloader", 120, 300) {
}

void xce::distcache::userapicache::DistUserApiCacheReloaderAdapter::reload(MyUtil::IntSeq userIds, const Ice::Context& ctx) {
	if(userIds.size() > 0) {
		try {
			getManager( 0 )->reload(userIds);
		}catch(...) {
			MCE_WARN("DistUserApiCacheReloaderAdapter::reloader Exception!");
		}
	}
}

void xce::distcache::userapicache::DistUserApiCacheReloaderAdapter::reload(const int userid, const Ice::Context& ctx) {
	MyUtil::IntSeq ids;
	ids.push_back(userid);
	reload(ids);
}

void xce::distcache::userapicache::DistUserApiCacheReloaderAdapter::update(const int userid, const Ice::Byte table, const MyUtil::Str2StrMap prop, const Ice::Context& ctx) {
	xce::userapicache::UserApiCacheUpdateOldDataPtr data = new xce::userapicache::UserApiCacheUpdateOldData;
	data->userid = userid;
	data->table = table;
	data->data = prop;
	try {
		getManager( 0 )->update(data);
	}catch(...) {
		MCE_WARN("DistUserApiCacheReloaderAdapter::update Exception!");
	}
}


xce::userapicache::UserApiCacheReloaderPrx xce::distcache::userapicache::DistUserApiCacheReloaderAdapter::getManager(int id) {
	return getProxy(id);	
}
