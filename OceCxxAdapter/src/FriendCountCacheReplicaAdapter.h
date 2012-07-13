#ifndef __FRIEND_COUNT_CACHE_REPLICA_ADAPTER_H__
#define __FRIEND_COUNT_CACHE_REPLICA_ADAPTER_H__

#include <boost/lexical_cast.hpp>
#include <IceUtil/RWRecMutex.h>
#include <FriendCountCache.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>
#include <TaskManager.h>

namespace xce {
namespace buddy {
namespace adapter {

class FriendCountCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<FriendCountCacheManagerPrx>, public MyUtil::Singleton<FriendCountCacheAdapter> {
public:
	FriendCountCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI <FriendCountCacheManagerPrx>("ControllerFriendCountCache", 120, 300) {
	}
        Ice::Int getFriendCount(Ice::Int id);
        MyUtil::Int2IntMap getFriendCounts(const MyUtil::IntSeq& ids);
	void setFriendCount(Ice::Int id, Ice::Int count);
	void setFriendCounts(const MyUtil::Int2IntMap& counts);
	void dec(Ice::Int id);
	void reload(Ice::Int id);
};

};
};
};

#endif
