#ifndef __FRIENDFINDER_REPLICA_ADAPTER_H__
#define __FRIENDFINDER_REPLICA_ADAPTER_H__

#include <boost/lexical_cast.hpp>
#include <FriendFinder.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>

namespace xce {
namespace friendfinder {
namespace adapter {

class FriendFinderReplicaAdapter : public MyUtil::ReplicatedClusterAdapterI<com::xiaonei::service::FriendFinderManagerPrx>, public MyUtil::Singleton<FriendFinderReplicaAdapter> {
public:
	FriendFinderReplicaAdapter() :
		MyUtil::ReplicatedClusterAdapterI <com::xiaonei::service::FriendFinderManagerPrx>("ControllerFriendFinderReplica", 120, 300) {
	}
        com::xiaonei::service::CommonFriendSeq getCommonFriend(int userId,int begin,int limit);
		MyUtil::IntSeq getSharedFriend(int userId, int friendId, int begin, int limit);
        void reloadCommonFriend(int userId);
        void rebuildCommonFriend(int userId);
        void removeCommonFriend(int userId, int friendId);
};

/*
class FriendFinderGuideReplicaAdapter : public MyUtil::ReplicatedClusterAdapterI<com::xiaonei::service::FriendFinderManagerPrx>, public MyUtil::Singleton<FriendFinderGuideReplicaAdapter> {
public:
	FriendFinderGuideReplicaAdapter() :
		MyUtil::ReplicatedClusterAdapterI <com::xiaonei::service::FriendFinderManagerPrx>("ControllerFriendFinderGuideReplica", 120, 300) {
	}
        com::xiaonei::service::CommonFriendSeq getCommonFriend(int userId,int begin,int limit);
        void reloadCommonFriend(int userId);
        void rebuildCommonFriend(int userId);
        void removeCommonFriend(int userId, int friendId);
};

class FriendFinderGuideReplicaNAdapter : public MyUtil::ReplicatedClusterAdapterI<com::xiaonei::service::FriendFinderManagerPrx>, public MyUtil::Singleton<FriendFinderGuideReplicaNAdapter> {
public:
	FriendFinderGuideReplicaNAdapter() :
		MyUtil::ReplicatedClusterAdapterI <com::xiaonei::service::FriendFinderManagerPrx>("ControllerFriendFinderGuideReplicaN", 120, 300) {
	}
        com::xiaonei::service::CommonFriendSeq getCommonFriend(int userId,int begin,int limit);
        void reloadCommonFriend(int userId);
        void rebuildCommonFriend(int userId);
        void removeCommonFriend(int userId, int friendId);
};
*/

};
};
};

#endif
