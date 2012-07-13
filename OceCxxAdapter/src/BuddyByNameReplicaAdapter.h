#ifndef __BUDDY_BYNAME_REPLICA_ADAPTER_H__
#define __BUDDY_BYNAME_REPLICA_ADAPTER_H__

#include <boost/lexical_cast.hpp>
#include <IceUtil/RWRecMutex.h>
#include <BuddyByNameCache.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>
#include <TaskManager.h>

namespace xce {
namespace buddy {
namespace adapter {

class BuddyByNameCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyByNameCacheManagerPrx>, public MyUtil::Singleton<BuddyByNameCacheAdapter> {
public:
	BuddyByNameCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI <BuddyByNameCacheManagerPrx>("ControllerBuddyByNameCache", 120, 300) {
	}
        Ice::Int getFriendCount(Ice::Int userId);
        MyUtil::Int2IntMap getFriendCounts(const MyUtil::IntSeq& ids);
        MyUtil::IntSeq getFriendList(Ice::Int userId,Ice::Int limit);
        MyUtil::IntSeq getFriendList(Ice::Int userId,Ice::Int begin,Ice::Int limit);
	MyUtil::Int2IntSeqMap getFriendLists(const MyUtil::IntSeq& ids);
        void load(Ice::Int,const Ice::ObjectPtr&);
        void remove(Ice::Int,Ice::Int);
        void setData(const MyUtil::ObjectResultPtr&);
	MyUtil::IntSeq checkFriendCounts(const MyUtil::IntSeq& ids);
};

class BuddyByNameLoaderAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyByNameLoaderPrx>, public MyUtil::Singleton<BuddyByNameLoaderAdapter> {
public:
	BuddyByNameLoaderAdapter() :
		MyUtil::ReplicatedClusterAdapterI <BuddyByNameLoaderPrx>("ControllerBuddyByNameLoader", 120, 300) {
	}
	void reload(Ice::Int);
        void load(const MyUtil::Int2IntSeqMap&);
        void rename(Ice::Int,const std::string&);
};

};
};
};

#endif
