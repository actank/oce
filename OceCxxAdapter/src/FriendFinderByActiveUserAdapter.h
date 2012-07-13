#ifndef _FRIENDFINDER_BYACTIVEUSER_ADAPTER__
#define _FRIENDFINDER_BYACTIVEUSER_ADAPTER__

#include "FriendFinderByActiveUser.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace friendfinder {
namespace adapter {

class FriendFinderByActiveUserAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::friendfinder::FriendFinderByActiveUserPrx>,
	public MyUtil::Singleton<FriendFinderByActiveUserAdapter> {
public:
	FriendFinderByActiveUserAdapter() :
		MyUtil::ReplicatedClusterAdapterI<xce::friendfinder::FriendFinderByActiveUserPrx>("ControllerFriendFinderByActiveUser", 120, 1000) {
	}
	
	void setBitMap(int cluster, const MyUtil::Int2IntMap& loginMap);
    void setNegtiveList(int cluster, const MyUtil::IntSeq& userList);
	void clearUserLoginCache(int cluster);

    MyUtil::Int2IntSeqMap getRecommend(int userId, int limit);
    MyUtil::Int2IntSeqMap getRecommendRandom(int userId, int limit);
    MyUtil::IntSeq getWriteRecommendRandom(int userId, int limit);
    void removeRecommendFriends(int userId, int friendId, const MyUtil::IntSeq& ids); 

	MyUtil::IntSeq getActiveUser(int userId, const MyUtil::IntSeq& userList);
};

}
}
}

#endif
