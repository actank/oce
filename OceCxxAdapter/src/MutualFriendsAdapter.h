#ifndef _MUTUAL_FRIENDS_ADAPTER_H_
#define _MUTUAL_FRIENDS_ADAPTER_H_

#include <boost/lexical_cast.hpp>
#include <IceUtil/RWRecMutex.h>
#include <MutualFriendsCache.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>
#include <TaskManager.h>

//GlobalId.h没定义，在CacheI中也需要用到
#ifndef _GLOBALID_H_
#define _GLOBALID_H_
#include "UtilCxx/src/GlobalId.h"
#endif

namespace xce {
namespace buddy {
namespace adapter {

//------------------------MutualFriendsCacheAdapter-----------------------------
class MutualFriendsCacheAdapter: public MyUtil::ReplicatedClusterAdapterI<MutualFriendsCacheManagerPrx>, public MyUtil::Singleton<MutualFriendsCacheAdapter> {
public:
	MutualFriendsCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI<MutualFriendsCacheManagerPrx>("ControllerMutualFriendsCache", 120, 300) {
	}

	MyUtil::IntSeq getMutualFriends(int uid, const MyUtil::GlobalIdPtr& gid, int begin, int limit);
	MyUtil::IntSeq getUnMutualFriends(int uid, const MyUtil::GlobalIdPtr& gid, int begin, int limit);
	int getMutualFriendsCount(int uid, const MyUtil::GlobalIdPtr& gid);
	int getUnMutualFriendsCount(int uid, const MyUtil::GlobalIdPtr& gid);
	int getFansCount(const MyUtil::GlobalIdPtr& gid);
	bool isFans(int uid, const MyUtil::GlobalIdPtr& gid);
	MyUtil::IntSeq getFans(const MyUtil::GlobalIdPtr& gid, int begin, int limit);

//	bool isValid();
//	void setValid(bool valid);
	void add(int uid, const MyUtil::GlobalIdPtr& gid);
	void remove(int uid, const MyUtil::GlobalIdPtr& gid);
	void setData(const ::MyUtil::ObjectResultPtr& data);
};

//-----------------------MutualFriendsLoaderAdapter-----------------------------
class MutualFriendsLoaderAdapter: public MyUtil::ReplicatedClusterAdapterI<MutualFriendsLoaderPrx>, public MyUtil::Singleton<MutualFriendsLoaderAdapter> {
public:
	MutualFriendsLoaderAdapter() :
		MyUtil::ReplicatedClusterAdapterI<MutualFriendsLoaderPrx>("ControllerMutualFriendsLoader", 120, 300) {
	}
	void reload(Ice::Long);
};

}//end of namespace adapter
}//end of namespace buddy
}//end of namespace xce

#endif
