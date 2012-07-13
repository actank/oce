#ifndef __FRIEND_DIST_H__
#define __FRIEND_DIST_H__

#include "FriendDist.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace mop {
namespace hi {
namespace svc {
namespace model {
using namespace MyUtil;
using namespace mop::hi::svc::model;

class FriendDistAdapter: public AdapterI, public AdapterISingleton<
		MyUtil::OceChannel, FriendDistAdapter> {
public:
	FriendDistAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	
	MyUtil::Int2IntMap getFriendDist(int userId);
	void rebuildFriendDist(int userId);

private:
	virtual string name() {
		return "FriendDist";
	}
	virtual string endpoints() {
		return "@FriendDist";
	}
	virtual size_t cluster() {
		return 10;
	}
	FriendDistManagerPrx getFriendDistManager(int userId);
	FriendDistManagerPrx getFriendDistManagerOneway(int userId);

	typedef vector<FriendDistManagerPrx> FriendDistManagerSeq;
	typedef vector<MyUtil::IntSeq> IntSeqSeq;

	void seperateFriendDistIds(const MyUtil::IntSeq& userIds,
			IntSeqSeq& seperatedFriendDistIds);

	FriendDistManagerSeq _managers;
	FriendDistManagerSeq _managersOneway;
};

}
}
}
}

#endif

