#ifndef USERAPICACHEADAPTER_H_
#define USERAPICACHEADAPTER_H_

#include <AdapterI.h>
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"
#include <UserApiCache.h>
#include <UserApiCache.pb.h>
#include <XceDistCacheAdapter.h>

namespace xce {
namespace userapicache {

class UserApiCacheMemData : public xce::distcache::userapicache::UserApiCacheData, public Ice::Object {
public:
	UserApiCacheData1Ptr getApiCacheData1()
	{
		UserApiCacheData1Ptr data = new UserApiCacheData1;
		data->id = id();
		data->status = status();
		data->name = name();
		data->state = state();
		data->level = level();
		data->auth = auth();
		data->statusconfig = statusconfig();
		data->basicconfig = basicconfig();
		data->profileprivacy = profileprivacy();
		data->stage = stage();
		data->gender = gender();
		data->univ = univ();
		data->birthyear = birthyear();
		data->birthmonth = birthmonth();
		data->birthday = birthday();
		data->homeprovince = homeprovince();
		data->homecity = homecity();
		data->mainurl = mainurl();
		data->headurl = headurl();
		data->tinyurl = tinyurl();
		return data;
	}
};

typedef IceUtil::Handle<UserApiCacheMemData> UserApiCacheMemDataPtr;
	
class UserApiCacheAdapter : public xce::distcache::XceDistCacheRAdapter<UserApiCacheMemData, UserApiCacheMemDataPtr>,
	public MyUtil::ReplicatedClusterAdapterI<UserApiCacheManagerPrx>,
	public MyUtil::Singleton<UserApiCacheAdapter> {
public:
	UserApiCacheAdapter() : MyUtil::ReplicatedClusterAdapterI <UserApiCacheManagerPrx> ("ControllerUserApiCache", 120, 300) {
	}
	void initialize()
	{
		std::string name("UserApiCacheD");
		init(name);
	}
	virtual ~UserApiCacheAdapter() {
	}
	UserApiCacheData1Ptr GetUserApiCache(int userId);
	std::map<int,UserApiCacheData1Ptr> GetUserApiCacheList(MyUtil::IntSeq userIds);
};

}
}

#endif
