#ifndef __BUDDY_BYID_REPLICA_ADAPTER_H__
#define __BUDDY_BYID_REPLICA_ADAPTER_H__
#include <bitset>
#include <boost/lexical_cast.hpp>
#include <IceUtil/RWRecMutex.h>
#include <BuddyByIdCache.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>
#include <TaskManager.h>

namespace xce {
  namespace buddy {
    namespace adapter {

      const int STATUS_CACHE_SIZE = 2147483647; 

      class BuddyByIdCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyByIdCacheManagerPrx>, public MyUtil::Singleton<BuddyByIdCacheAdapter> {
        public:
          BuddyByIdCacheAdapter() :
            MyUtil::ReplicatedClusterAdapterI <BuddyByIdCacheManagerPrx>("ControllerBuddyByIdCache", 120, 300) {
            }
          Ice::Int getFriendCount(Ice::Int userId);
          MyUtil::Int2IntMap getFriendCounts(const MyUtil::IntSeq& ids);
          MyUtil::IntSeq getFriendListAsc(Ice::Int userId,Ice::Int limit);
          MyUtil::IntSeq getFriendListAsc(Ice::Int userId,Ice::Int begin, Ice::Int limit);
          MyUtil::Int2IntSeqMap getFriendLists(const MyUtil::IntSeq& ids);
          void load(Ice::Int,const Ice::ObjectPtr&);
          void remove(Ice::Int,Ice::Int);
          void setData(const MyUtil::ObjectResultPtr&);
          MyUtil::IntSeq checkFriendCounts(const MyUtil::IntSeq& ids);
      };

      class BuddyByIdLoaderAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyByIdLoaderPrx>, public MyUtil::Singleton<BuddyByIdLoaderAdapter> {
        public:
          BuddyByIdLoaderAdapter() :
            MyUtil::ReplicatedClusterAdapterI <BuddyByIdLoaderPrx>("ControllerBuddyByIdLoader", 120, 300) {
            }
          void load(Ice::Int,const MyUtil::IntSeq&);
          void reload(Ice::Int);
          void add(Ice::Int,Ice::Int);
          void statusUpdate(Ice::Int,Ice::Int);
          int filter(Ice::Int,const MyUtil::IntSeq&);
          void getAllStatus(std::bitset<STATUS_CACHE_SIZE>& status, bool& success);
      };

    };
  };
};

#endif
