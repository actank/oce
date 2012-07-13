#include "BuddyList.h"
#include "IceLogger.h"
#include "OnlineManagerI.h"
#include "BuddyByIdReplicaAdapter.h"

//using namespace com::xiaonei::service::buddy;
using namespace xce::buddy::adapter;

bool BuddyList::load() {
	IceUtil::Mutex::Lock lock(_mutex);
	StatFunc statF("BuddyList::load --> ***");
	MCE_DEBUG("BuddyList::load --> load online buddies " << _userId);
	MyUtil::IntSeq ids;
	try {
		MCE_DEBUG("BuddyList::load --> invoke BuddyCore " << _userId);
		ids = BuddyByIdCacheAdapter::instance().getFriendListAsc(_userId, 5000);
		MCE_DEBUG("BuddyList::load --> buddies count " << ids.size());
	} catch (Ice::Exception& e) {
		MCE_WARN("BuddyList::load -> get buddies error" << e);
		return false;
	}
	for(size_t i=0; i<ids.size(); i++){
		if(OnlineManagerI::instance().has(ids.at(i))){
			_buddies.insert(ids.at(i));
		}
	}
	MCE_DEBUG("BuddyList::load --> online buddies count " << _buddies.size());
	return true;
}

void BuddyList::join(int userId){
	IceUtil::Mutex::Lock lock(_mutex);
	//MCE_WARN("BuddyList::join --> " << userId);
	_buddies.insert(userId);

	MCE_DEBUG("BuddyList::join --> " << userId << "join, " << _userId << "'s buddies count " << _buddies.size());
}

void BuddyList::leave(int userId){
	IceUtil::Mutex::Lock lock(_mutex);
	//MCE_WARN("BuddyList::leave --> " << userId);

	_buddies.erase(userId);
	MCE_DEBUG("BuddyList::leave --> " << userId << "leave, " << _userId << "'s buddies count " << _buddies.size());
}

int BuddyList::size() {
	IceUtil::Mutex::Lock lock(_mutex);
	//MCE_WARN("BuddyList::size --> ");
	return _buddies.size();
}

set<int> BuddyList::buddyIdSet() {
	IceUtil::Mutex::Lock lock(_mutex);
	//MCE_WARN("BuddyList::buddyIdSet --> ");
	return _buddies;
}
