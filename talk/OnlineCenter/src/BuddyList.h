#ifndef BUDDYLIST_H_
#define BUDDYLIST_H_

#include <IceUtil/RWRecMutex.h>
#include <set>
#include "User.h"
#include "OnlineCenter.h"
using namespace talk::online;

class BuddyList : public Ice::Object {
public:
	BuddyList(int userId) :
		_userId(userId) {
		;
	}

	void join(int userId);
	void leave(int userId);

	bool load();

	int size();

//	MyUtil::IntSeq buddyIds();
	set<int> buddyIdSet();

private:
	set<int> _buddies;
	int _userId;
	IceUtil::Mutex _mutex;
};
typedef IceUtil::Handle<BuddyList> BuddyListPtr;

#endif
