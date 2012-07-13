#ifndef __FRIEND_FINDER_ADAPTER_H__
#define __FRIEND_FINDER_ADAPTER_H__

#include "FriendFinder.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace com {
namespace xiaonei {
namespace service {
using namespace MyUtil;

class FriendFinderAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::OceChannel, FriendFinderAdapter> {
public:
	FriendFinderAdapter() {
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	CommonFriendSeq getCommonFriend(int userId,int begin,int limit);
	IntSeq getSharedFriend(int userId1, int userId2, int begin, int limit);
	int getSharedFriendCount(int userId1, int userId2);
  	Int2ShareFriendMap getSharedFriends(int fromId, MyUtil::IntSeq toIds, int limit);
	void reloadCommonFriend(int userId);
	void rebuildCommonFriend(int userId);
	void removeCommonFriend(int userId, int friendId);
private:
	virtual string name() {
		return "FriendFinder";
	}
	;
	virtual string endpoints() {
		return "@FriendFinder";
	}
	;
	virtual size_t cluster() {
		return 10;
	}
	;

	FriendFinderManagerPrx getManager(int id);
	FriendFinderManagerPrx getOnewayManager(int id);

	std::vector<FriendFinderManagerPrx> _managers;
	std::vector<FriendFinderManagerPrx> _managersOneway;
};

}
}
}
#endif

