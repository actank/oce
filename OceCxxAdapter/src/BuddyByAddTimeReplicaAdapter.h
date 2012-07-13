#ifndef __BUDDY_BYADDTIME_REPLICA_ADAPTER_H__
#define __BUDDY_BYADDTIME_REPLICA_ADAPTER_H__

#include <boost/lexical_cast.hpp>
#include <IceUtil/RWRecMutex.h>
#include <BuddyByAddTimeCache.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>
#include <TaskManager.h>

namespace xce {
namespace buddy {
namespace adapter {

class BuddyByAddTimeCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyByAddTimeCacheManagerPrx>, public MyUtil::Singleton<BuddyByAddTimeCacheAdapter> {
public:
	BuddyByAddTimeCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI <BuddyByAddTimeCacheManagerPrx>("ControllerBuddyByAddTimeCache", 120, 300) {
	}
        Ice::Int getFriendCount(Ice::Int userId);
        MyUtil::Int2IntMap getFriendCounts(const MyUtil::IntSeq& ids);
        MyUtil::IntSeq getFriendList(Ice::Int userId,Ice::Int limit);
        MyUtil::IntSeq getFriendList(Ice::Int userId,Ice::Int begin,Ice::Int limit);
	MyUtil::Int2IntSeqMap getFriendLists(const MyUtil::IntSeq& ids);
        //void load(Ice::Int,const Ice::ObjectPtr&,const std::string&="");
        void remove(Ice::Int,Ice::Int);
        void setData(const MyUtil::ObjectResultPtr&);
	MyUtil::IntSeq checkFriendCounts(const MyUtil::IntSeq& ids);
};

class BuddyByAddTimeLoaderAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyByAddTimeLoaderPrx>, public MyUtil::Singleton<BuddyByAddTimeLoaderAdapter> {
public:
        BuddyByAddTimeLoaderAdapter() :
                MyUtil::ReplicatedClusterAdapterI <BuddyByAddTimeLoaderPrx>("ControllerBuddyByAddTimeLoader", 120, 300) {
        }
        void reload(Ice::Int);
        void load(const MyUtil::Int2IntSeqMap&);
};

};
};
};

#endif
