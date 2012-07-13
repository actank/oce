#ifndef __BUDDY_FRIENDS_REPLICA_ADAPTER_H__
#define __BUDDY_FRIENDS_REPLICA_ADAPTER_H__

#include <boost/lexical_cast.hpp>
#include <IceUtil/RWRecMutex.h>
#include <BuddyFriendsCache.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>
#include <TaskManager.h>

namespace xce {
  namespace buddy {
    namespace adapter {

      class BuddyFriendsCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyFriendsCacheManagerPrx>, public MyUtil::Singleton<BuddyFriendsCacheAdapter> {
        public:
          BuddyFriendsCacheAdapter() :
            MyUtil::ReplicatedClusterAdapterI <BuddyFriendsCacheManagerPrx>("ControllerBuddyFriendsCache", 120, 300) {
            }

	  bool isFriendsFriend(int hostId, int guestId);
          MyUtil::Int2IntMap isFriendsFriendBatch(int hostId, const MyUtil::IntSeq& ids);
          
	  MyUtil::IntSeq getCommonFriends(int hostId, int guestId);
 	  MyUtil::Str2IntSeqMap getCommonFriendsBatch( MyUtil::Int2IntMap& idpairs );

	  //-------------------------------------------------------------------------------
	  Ice::Int getFriendCount(Ice::Int userId);
          MyUtil::Int2IntMap getFriendCounts(const MyUtil::IntSeq& ids);
          MyUtil::IntSeq getFriendListAsc(Ice::Int userId,Ice::Int begin, Ice::Int limit);
          MyUtil::Int2IntSeqMap getFriendLists(const MyUtil::IntSeq& ids);
          void load(Ice::Int,const Ice::ObjectPtr&);
          void remove(Ice::Int,Ice::Int);
          void add(Ice::Int,Ice::Int);
          void setData(const MyUtil::ObjectResultPtr&);
      };

      class BuddyFriendsLoaderAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyFriendsLoaderPrx>, public MyUtil::Singleton<BuddyFriendsLoaderAdapter> {
        public:
          BuddyFriendsLoaderAdapter() :
            MyUtil::ReplicatedClusterAdapterI <BuddyFriendsLoaderPrx>("ControllerBuddyFriendsLoader", 120, 300) {
            }
          void load(Ice::Int,const MyUtil::IntSeq&);
          void reload(Ice::Int);
          void add(Ice::Int,Ice::Int);
      };

    };
  };
};

#endif
