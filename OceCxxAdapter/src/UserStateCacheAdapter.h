#ifndef _USERSTATECACHEADAPTER_H_
#define _USERSTATECACHEADAPTER_H_

#include <Ice/Ice.h>
#include <UserStateCache.h>
#include <AdapterI.h>
//#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce{
namespace userstatecache {

class UserStateCache : virtual public Ice::Object {
private:
	int id_;
	int state_;
	int level_;
	std::string nickname_;
	std::string name_;
public:
	UserStateCache(const UserStateCacheNewDataPtr& data) : id_(data->id), state_(data->state),
		level_(data->level), nickname_(data->nickname), name_(data->name) {}
	virtual ~UserStateCache(){}
	int id() {
		return id_;
	}
	int state() {
		return state_;
	}
	int level() {
		return level_;
	}
	bool vip(){
		return state_ & 0x2;
	}
	bool ever_vip(){
		return state_ & 0x10;
	}
	std::string nickname() {
		return nickname_;
	}
	std::string name() {
		return name_;
	}
};

typedef IceUtil::Handle<UserStateCache> UserStateCachePtr;

class UserStateCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<UserStateCacheManagerPrx>,
	public MyUtil::Singleton<UserStateCacheAdapter> {
public:
	UserStateCacheAdapter() : MyUtil::ReplicatedClusterAdapterI <UserStateCacheManagerPrx> ("ControllerUserStateCache", 120, 300) {}
	virtual ~UserStateCacheAdapter() {}
	std::map<int, UserStateCachePtr> GetUserStateCache(const std::vector<int>& ids);
};

}
}

#endif
