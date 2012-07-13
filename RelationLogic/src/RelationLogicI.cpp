#include "RelationLogicI.h"
#include "ServiceI.h"
#include "FriendDistAdapter.h"
#include "BatchUpdaterAdapter.h"
#include "MainAdapter.h"
#include "FriendFinderAdapter.h"
#include "BuddyCoreAdapter.h"

using namespace MyUtil;
using namespace mop::hi::svc::relation;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::service;
using namespace com::xiaonei::service::buddy;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(new RelationLogicManagerI, service.createIdentity(
			RELATION_LOGIC_MANAGER, ""));

	//UserAdapter::instance().initializeWith(service.getAdapter());
	//FriendDistAdapter::instance().initializeWith(service.getAdapter());
	//MainAdapter::instance().initializeWith(service.getAdapter());
	//BatchUpdaterAdapter::instance().initializeWith(service.getAdapter());
	//CountCacheAdapter::instance().initializeWith(service.getAdapter());
	//FriendFinderAdapter::instance().initializeWith(service.getAdapter());
	//BuddyCoreAdapter::instance().initializeWith(service.getAdapter());
}

void mop::hi::svc::relation::RelationLogicManagerI::addApply(::Ice::Int fromId,
		::Ice::Int toId, const Ice::Current& current) {
	MCE_DEBUG("addApply");
	// add GuestRequestCount to table "users"
	UserAdapter::instance().incGuestRequestCount(toId); /* This will change 'users' table only.*/
	MainAdapter::instance().addBuddyApply(fromId, toId);
	CountCacheAdapter::instance().incGuestRequestCount(toId); /* This will change 'count' table only. */
	BuddyCoreAdapter::instance().reload(fromId);
	BuddyCoreAdapter::instance().reload(toId);
	FriendFinderAdapter::instance().reloadCommonFriend(fromId);
	MCE_DEBUG("addApply.done");
}

void mop::hi::svc::relation::RelationLogicManagerI::acceptRequest(
		::Ice::Int toId, ::Ice::Int fromId, const Ice::Current& current) {
	MCE_DEBUG("acceptRequest");
	UserAdapter::instance().decGuestRequestCount(toId);
	UserAdapter::instance().incFriendCount(toId);
	UserAdapter::instance().incFriendCount(fromId);
	CountCacheAdapter::instance().decGuestRequestCount(toId);

	int hostUniversity = UserAdapter::instance().getUserUniv(toId);
	int guestUniversity = UserAdapter::instance().getUserUniv(fromId);

	FriendDistAdapter::instance().incFriendDist(toId, guestUniversity);
	FriendDistAdapter::instance().incFriendDist(fromId, hostUniversity);

	MainAdapter::instance().addBuddy(toId, fromId);
	MainAdapter::instance().addBuddy(fromId, toId);

	BuddyCoreAdapter::instance().reload(fromId);
	BuddyCoreAdapter::instance().reload(toId);

	FriendFinderAdapter::instance().reloadCommonFriend(fromId);
	FriendFinderAdapter::instance().reloadCommonFriend(toId);

	MCE_DEBUG("acceptRequest.done");
}

void mop::hi::svc::relation::RelationLogicManagerI::denyRequest(
		::Ice::Int toId, ::Ice::Int fromId, const Ice::Current& current) {
	MCE_DEBUG("denyRequest");
	UserAdapter::instance().decGuestRequestCount(toId);
	CountCacheAdapter::instance().decGuestRequestCount(toId);

	BuddyCoreAdapter::instance().reload(fromId);
	BuddyCoreAdapter::instance().reload(toId);

	FriendFinderAdapter::instance().reloadCommonFriend(fromId);
	FriendFinderAdapter::instance().reloadCommonFriend(toId);
	MCE_DEBUG("denyRequest.done");
}

void mop::hi::svc::relation::RelationLogicManagerI::removeBuddy(
		::Ice::Int hostId, ::Ice::Int guestId, const Ice::Current& current) {
	MCE_DEBUG("removeBuddy");
	ConnectionQuery conn("feedwb", CDbWServer);
	try {
		ostringstream sqlFeedHigh;
		sqlFeedHigh << "DELETE FROM relation_feed_high"
		<< " WHERE SOURCE=" << hostId << " AND TARGET=" << guestId
		<< " OR SOURCE=" << guestId << " AND TARGET=" << hostId;

		conn.doDelete(sqlFeedHigh.str());
	} catch (...) {

	}
	try {
		ostringstream sqlFeedLow;
		sqlFeedLow << "DELETE FROM relation_feed_low"
		<< " WHERE SOURCE=" << hostId << " AND TARGET=" << guestId
		<< " OR SOURCE=" << guestId << " AND TARGET=" << hostId;
		conn.doDelete( sqlFeedLow.str());
	} catch (...) {

	}

	UserAdapter::instance().decFriendCount(hostId);
	UserAdapter::instance().decFriendCount(guestId);

	int hostUniversity = UserAdapter::instance().getUserUniv(hostId);
	int guestUniversity = UserAdapter::instance().getUserUniv(guestId);
	MCE_DEBUG("removeBuddy decFriendDist" << hostId << ", " << hostUniversity);
	FriendDistAdapter::instance().decFriendDist(hostId, guestUniversity);
	MCE_DEBUG("removeBuddy decFriendDist" << guestId << ", " << guestUniversity);
	FriendDistAdapter::instance().decFriendDist(guestId, hostUniversity);

	//CAUTION: this pair of notifiction is based on policy of FrontServer
	MCE_DEBUG("context[\"SOURCE\"] is " << const_cast<Ice::Context&>(current.ctx)["SOURCE"]);
	if (const_cast<Ice::Context&>(current.ctx)["SOURCE"] != "IM") {
		MCE_DEBUG("IM TEST: context SOURCE is NOT IM");
		MainAdapter::instance().removeBuddy(hostId, guestId);
	}
	MCE_DEBUG("IM TEST 2");
	MainAdapter::instance().removeBuddy(guestId, hostId);
	//CAUTION: see above
	BuddyCoreAdapter::instance().reload(hostId);
	BuddyCoreAdapter::instance().reload(guestId);
	FriendFinderAdapter::instance().reloadCommonFriend(hostId);
	FriendFinderAdapter::instance().reloadCommonFriend(guestId);
	MCE_DEBUG("removeBuddy.done");
}

void mop::hi::svc::relation::RelationLogicManagerI::addBlock(::Ice::Int hostId,
		::Ice::Int guestId, const Ice::Current& current) {
	MCE_DEBUG("addBlock");
	MainAdapter::instance().setBlock(hostId, guestId);
	// Here is no need increasing/decreasing the FriendDist,
	// because, Relation2Service will call removeBuddy itself.
	BuddyCoreAdapter::instance().reload(hostId);
	BuddyCoreAdapter::instance().reload(guestId);
	FriendFinderAdapter::instance().reloadCommonFriend(hostId);
	FriendFinderAdapter::instance().reloadCommonFriend(guestId);
	MCE_DEBUG("addBlock.done");
}

void mop::hi::svc::relation::RelationLogicManagerI::removeBlock(
		::Ice::Int hostId, ::Ice::Int guestId, const Ice::Current& current) {
	MCE_DEBUG("removeBlock");
	// remove block is no need to invoke MainAdapter.
	MainAdapter::instance().removeBuddy(hostId,guestId);
	BuddyCoreAdapter::instance().reload(hostId);
	BuddyCoreAdapter::instance().reload(guestId);
	FriendFinderAdapter::instance().reloadCommonFriend(hostId);
	FriendFinderAdapter::instance().reloadCommonFriend(guestId);
	MCE_DEBUG("removeBlock.done");
}

