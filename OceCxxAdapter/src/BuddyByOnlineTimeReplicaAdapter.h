#ifndef __BUDDY_BYONLINETIME_REPLICA_ADAPTER_H__
#define __BUDDY_BYONLINETIME_REPLICA_ADAPTER_H__

#include <boost/lexical_cast.hpp>
#include <IceUtil/RWRecMutex.h>
#include <BuddyByOnlineTimeCache.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>
#include <TaskManager.h>

namespace xce {
namespace buddy {
namespace adapter {

class BuddyByOnlineTimeCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyByOnlineTimeCacheManagerPrx>, public MyUtil::Singleton<BuddyByOnlineTimeCacheAdapter> {
public:
	BuddyByOnlineTimeCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI <BuddyByOnlineTimeCacheManagerPrx>("ControllerBuddyByOnlineTimeCache", 120, 300) {
	}
        Ice::Int getFriendCount(Ice::Int userId);
        MyUtil::Int2IntMap getFriendCounts(const MyUtil::IntSeq& ids);
        MyUtil::IntSeq getFriendList(Ice::Int userId,Ice::Int limit);
        MyUtil::IntSeq getFriendList(Ice::Int userId,Ice::Int begin,Ice::Int limit);
		MyUtil::Int2IntSeqMap getFriendLists(const MyUtil::IntSeq& ids);
		void notifySomebodyOnline(Ice::Int userId,Ice::Int onlineId);
		void notifyOnlineBatch(const OnlineNoteSeq& notes);
        void load(Ice::Int,const Ice::ObjectPtr&);
        void remove(Ice::Int,Ice::Int);
        void setData(const MyUtil::ObjectResultPtr&);
		MyUtil::IntSeq checkFriendCounts(const MyUtil::IntSeq& ids);
};

class BuddyByOnlineTimeLoaderAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyByOnlineTimeLoaderPrx>, public MyUtil::Singleton<BuddyByOnlineTimeLoaderAdapter> {
public:
        BuddyByOnlineTimeLoaderAdapter() :
                MyUtil::ReplicatedClusterAdapterI <BuddyByOnlineTimeLoaderPrx>("ControllerBuddyByOnlineTimeLoader", 120, 300) {
        }
        void reload(Ice::Int);
        void load(const MyUtil::Int2IntSeqMap&);
        void online(Ice::Int);
};

};
};
};

#endif
