#include "FriendDistAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace mop::hi::svc::model;
using namespace std;

//---------------------------------------------------------------------------
FriendDistManagerPrx FriendDistAdapter::getFriendDistManager(int id) {
	return locate<FriendDistManagerPrx>(_managers, "FDM", id, TWO_WAY);
}

FriendDistManagerPrx FriendDistAdapter::getFriendDistManagerOneway(int id) {
	return locate<FriendDistManagerPrx>(_managersOneway, "FDM", id,
			ONE_WAY);
}

MyUtil::Int2IntMap FriendDistAdapter::getFriendDist(int userId) {
	return getFriendDistManager(userId)->getFriendDist(userId);
}

void FriendDistAdapter::rebuildFriendDist(int userId) {
	getFriendDistManagerOneway(userId)->rebuildFriendDist(userId);
}

void FriendDistAdapter::seperateFriendDistIds(const MyUtil::IntSeq& userIds,
		IntSeqSeq& seperatedFriendDistIds) {
	seperatedFriendDistIds.clear();

	for (size_t i=0; i<_cluster; ++i) {
		seperatedFriendDistIds.push_back(MyUtil::IntSeq());
	}

	for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it
			!= userIds.end(); ++it) {
		int pos = *it % _cluster;
		seperatedFriendDistIds.at(pos).push_back(*it);
	}
}

