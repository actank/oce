#include "BuddyLogicI.h"
#include "ServiceI.h"
#include "FriendDistAdapter.h"
#include "BatchUpdaterAdapter.h"
//#include "OnlineStatDispatchAdapter.h"
//#include "TalkLogicAdapter.h"
#include "FriendFinderAdapter.h"
#include "TalkRightAdapter.h"
#include "XFeedDispatcherAdapter.h"
#include "BuddyGroupAdapter.h"
#include "BuddyApplyCacheAdapter.h"
#include "OfferFriendsAdapter.h"
#include "BuddyCacheAdapter.h"
#include "BuddyCacheByIdAdapter.h"
#include "RecentlyOnlineBuddyAdapter.h"
#include "BuddyCacheByNameAdapter.h"
#include "RecommendFriendAdapter.h"
#include "UserCountAdapter.h"

#include "OnlineCenterAdapter.h"
#include "PresenceMsgHandlerAdapter.h"
#include "IqMsgHandlerAdapter.h"

#include "BuddyCacheByIdAdapter.h"
#include "BuddyByIdReplicaAdapter.h"
#include "BuddyRelationReplicaAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"


using namespace MyUtil;
using namespace mop::hi::svc::adapter;
using namespace xce::buddy;
using namespace talk::online::adapter;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::service; 
using namespace com::xiaonei::talk::adapter;
using namespace xce::xfeed;
using namespace xce::buddyapplycache;
using namespace xce::recentlyonlinebuddy;
using namespace xce::buddycachebyname;
using namespace talk::adapter;
using namespace xce::recommendfriend::adapter;
using namespace xce::usercount::adapter;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	BuddyLogicI* manager = new BuddyLogicI;
	service.getAdapter()->add(manager, service.createIdentity(BUDDY_CORE_LOGIC, ""));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyLogic.Mod");
        int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyLogic.Interval", 5);
        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerBuddyLogic",
                        &BuddyLogicI::instance(), mod, sInterval);

	TaskManager::instance().config(TASK_LEVEL_NORMAL, ThreadPoolConfig(1, 1));

}

void BuddyLogicI::addApply(const Relationship& apply,const BuddyRelationPtr& localbrp,
                const BuddyRelationPtr& remotebrp, const ApplyInfo& applyInfo, const Ice::Current& current) {
	MCE_INFO("addApply -> " << apply.from << " apply " << apply.to);

	TaskManager::instance().execute(new PresenceSubscribeTask(const_cast<Ice::Context&>(current.ctx)["id"], apply.from, apply.to, const_cast<Ice::Context&>(current.ctx)["status"]));
	TaskManager::instance().execute(new RebuildBuddyApplyCacheTask(apply.to, applyInfo.props));
	TaskManager::instance().execute(new RemoveCommonFriendTask(apply.from, apply.to));
	TaskManager::instance().execute(new RemoveOfferTask(apply.from, apply.to));
	TaskManager::instance().execute(new RemoveRecommendFriendTask(apply.from, apply.to));

	TaskManager::instance().execute(new LoadBuddyRelationTask(apply.from, localbrp));
	TaskManager::instance().execute(new LoadBuddyRelationTask(apply.to, remotebrp));
	MCE_DEBUG("addApply -> " << apply.from << " apply " << apply.to << " done");
}

void BuddyLogicI::acceptApply(const Relationship& apply, const BuddyRelationPtr& localbrp,
                const BuddyRelationPtr& remotebrp, const Ice::Current& current) {
	MCE_INFO("acceptApply -> " << apply.to << " accept " << apply.from);

	TaskManager::instance().execute(new RemoveBuddyApplyCacheTask(apply.to, apply.from));
	TaskManager::instance().execute(new PresenceSubscribedTask(const_cast<Ice::Context&>(current.ctx)["id"],apply.from, apply.to));
	TaskManager::instance().execute(new IncUserFriendCountTask(apply.to));
	TaskManager::instance().execute(new IncUserFriendCountTask(apply.from));
	TaskManager::instance().execute(new RemoveRecommendFriendTask(apply.from, apply.to));

	TaskManager::instance().execute(new RebuildFriendDistTask(apply.to));
	TaskManager::instance().execute(new RebuildFriendDistTask(apply.from));
	TaskManager::instance().execute(new LoadBuddyCacheTask(apply.to));
	TaskManager::instance().execute(new LoadBuddyCacheTask(apply.from));
	TaskManager::instance().execute(new LoadBuddyCacheByIdTask(apply.to));
	TaskManager::instance().execute(new LoadBuddyCacheByIdTask(apply.from));
//	TaskManager::instance().execute(new RebuildRecentlyOnlineBuddyTask(apply.to));
//	TaskManager::instance().execute(new RebuildRecentlyOnlineBuddyTask(apply.from));
	TaskManager::instance().execute(new AddRecentlyOnlineBuddyTask(apply.to,apply.from));
	TaskManager::instance().execute(new AddRecentlyOnlineBuddyTask(apply.from,apply.to));
	TaskManager::instance().execute(new AddBuddyCacheByNameTask(apply.to,apply.from));
	TaskManager::instance().execute(new AddBuddyCacheByNameTask(apply.from,apply.to));

	TaskManager::instance().execute(new AddBuddyOnlineStatTask(apply.to, apply.from));
	TaskManager::instance().execute(new RemoveCommonFriendTask(apply.to, apply.from));
	TaskManager::instance().execute(new RemoveCommonFriendTask(apply.from, apply.to));
	TaskManager::instance().execute(new RemoveOfferTask(apply.to, apply.from));
	TaskManager::instance().execute(new RemoveOfferTask(apply.from, apply.to));
	TaskManager::instance().execute(new DeliverSupplyTask(apply.to, apply.from, current.ctx));
	TaskManager::instance().execute(new DeliverSupplyTask(apply.from, apply.to, current.ctx));

	TaskManager::instance().execute(new LoadBuddyRelationTask(apply.to, localbrp));
        TaskManager::instance().execute(new LoadBuddyRelationTask(apply.from, remotebrp));
	TaskManager::instance().execute(new LoadBuddyByIdTask(apply.to, localbrp->friends));
	TaskManager::instance().execute(new LoadBuddyByIdTask(apply.from, remotebrp->friends));

	MCE_DEBUG("acceptApply -> " << apply.to << " accept " << apply.from << " done");
}

void BuddyLogicI::denyApply(const Relationship& apply, const BuddyRelationPtr& localbrp,
                const BuddyRelationPtr& remotebrp, const Ice::Current& current) {
	MCE_INFO("denyApply -> " << apply.to << " deny " << apply.from);

	TaskManager::instance().execute(new RemoveBuddyApplyCacheTask(apply.to, apply.from));
	TaskManager::instance().execute(new PresenceUnsubscribedTask(const_cast<Ice::Context&>(current.ctx)["id"], apply.from, apply.to));
	TaskManager::instance().execute(new RemoveCommonFriendTask(apply.to, apply.from));
	TaskManager::instance().execute(new RemoveOfferTask(apply.to, apply.from));

	TaskManager::instance().execute(new LoadBuddyRelationTask(apply.to, localbrp));
        TaskManager::instance().execute(new LoadBuddyRelationTask(apply.from, remotebrp));

	MCE_DEBUG("denyApply -> " << apply.to << " deny " << apply.from << " done");
}

void BuddyLogicI::removeFriend(const Relationship& buddy, const BuddyRelationPtr& localbrp,
                const BuddyRelationPtr& remotebrp, const Ice::Current& current) {
	MCE_INFO("removeFriend -> " << buddy.from << " remove " << buddy.to);

	TaskManager::instance().execute(new DelInSqlTask("feedwb", buddy.from, buddy.to));

	TaskManager::instance().execute(new RemoveBuddyOnlineStatTask(buddy.from, buddy.to));
	TaskManager::instance().execute(new RemoveBuddyGroupTask(buddy.from, buddy.to));
	TaskManager::instance().execute(new RemoveBuddyNotifyTask(buddy.from, buddy.to));

	TaskManager::instance().execute(new DecUserFriendCountTask(buddy.from));
	TaskManager::instance().execute(new DecUserFriendCountTask(buddy.to));

	TaskManager::instance().execute(new RebuildFriendDistTask(buddy.from));
	TaskManager::instance().execute(new RebuildFriendDistTask(buddy.to));
	TaskManager::instance().execute(new LoadBuddyCacheTask(buddy.from));
	TaskManager::instance().execute(new LoadBuddyCacheTask(buddy.to));
	TaskManager::instance().execute(new LoadBuddyCacheByIdTask(buddy.from));
	TaskManager::instance().execute(new LoadBuddyCacheByIdTask(buddy.to));
//	TaskManager::instance().execute(new RebuildRecentlyOnlineBuddyTask(buddy.from));
//	TaskManager::instance().execute(new RebuildRecentlyOnlineBuddyTask(buddy.to));
	TaskManager::instance().execute(new RemoveRecentlyOnlineBuddyTask(buddy.from,buddy.to));
	TaskManager::instance().execute(new RemoveRecentlyOnlineBuddyTask(buddy.to,buddy.from));
	TaskManager::instance().execute(new RemoveBuddyCacheByNameTask(buddy.from,buddy.to));
	TaskManager::instance().execute(new RemoveBuddyCacheByNameTask(buddy.to,buddy.from));

	TaskManager::instance().execute(new LoadBuddyRelationTask(buddy.from, localbrp));
        TaskManager::instance().execute(new LoadBuddyRelationTask(buddy.to, remotebrp));	
	TaskManager::instance().execute(new LoadBuddyByIdTask(buddy.from, localbrp->friends));
        TaskManager::instance().execute(new LoadBuddyByIdTask(buddy.to, remotebrp->friends));	

	MCE_DEBUG("removeFriend -> " << buddy.from << " remove " << buddy.to << " done");
}

void BuddyLogicI::addBlock(const Relationship& block, const BuddyRelationPtr& localbrp,
                const BuddyRelationPtr& remotebrp, const Ice::Current& current) {
	MCE_INFO("addBlock -> " << block.from << " block " << block.to);

	TaskManager::instance().execute(new RemoveBuddyApplyCacheTask(block.from, block.to));
	TaskManager::instance().execute(new RemoveBuddyApplyCacheTask(block.to, block.from));

	TaskManager::instance().execute(new RemoveBuddyOnlineStatTask(block.from, block.to));
	TaskManager::instance().execute(new ReloadBlockUserTask(block.from));

	TaskManager::instance().execute(new RemoveCommonFriendTask(block.from, block.to));
	TaskManager::instance().execute(new RemoveCommonFriendTask(block.to, block.from));
	TaskManager::instance().execute(new RebuildCommonFriendTask(block.from));
	TaskManager::instance().execute(new RebuildCommonFriendTask(block.to));

	TaskManager::instance().execute(new RebuildFriendDistTask(block.from));
	TaskManager::instance().execute(new RebuildFriendDistTask(block.to));
	TaskManager::instance().execute(new LoadBuddyCacheTask(block.from));
	TaskManager::instance().execute(new LoadBuddyCacheTask(block.to));
	TaskManager::instance().execute(new LoadBuddyCacheByIdTask(block.from));
	TaskManager::instance().execute(new LoadBuddyCacheByIdTask(block.to));
//	TaskManager::instance().execute(new RebuildRecentlyOnlineBuddyTask(block.from));
//	TaskManager::instance().execute(new RebuildRecentlyOnlineBuddyTask(block.to));
	TaskManager::instance().execute(new RemoveRecentlyOnlineBuddyTask(block.from,block.to));
	TaskManager::instance().execute(new RemoveRecentlyOnlineBuddyTask(block.to,block.from));
	TaskManager::instance().execute(new RemoveBuddyCacheByNameTask(block.from,block.to));
	TaskManager::instance().execute(new RemoveBuddyCacheByNameTask(block.to,block.from));

	TaskManager::instance().execute(new LoadBuddyRelationTask(block.from, localbrp));
        TaskManager::instance().execute(new LoadBuddyRelationTask(block.to, remotebrp));
	TaskManager::instance().execute(new LoadBuddyByIdTask(block.from, localbrp->friends));
        TaskManager::instance().execute(new LoadBuddyByIdTask(block.to, remotebrp->friends));
	
	MCE_DEBUG("addBlock -> " << block.from << " block " << block.to << " done");
}

void BuddyLogicI::removeBlock(const Relationship& block,  const BuddyRelationPtr& localbrp,
                const BuddyRelationPtr& remotebrp, const Ice::Current& current) {
	MCE_INFO("removeBlock -> " << block.from << " remove block " << block.to);

	TaskManager::instance().execute(new ReloadBlockUserTask(block.from));

	TaskManager::instance().execute(new LoadBuddyRelationTask(block.from, localbrp));
        TaskManager::instance().execute(new LoadBuddyRelationTask(block.to, remotebrp));

	MCE_INFO("removeBlock -> " << block.from << " remove block " << block.to << " done");
}

// **************************************************************************
void PresenceSubscribeTask::handle() {
	PresenceMsgHandlerAdapter::instance().presenceSubscribe(_id, _from, _to, _status);
}

void PresenceSubscribedTask::handle() {
	PresenceMsgHandlerAdapter::instance().presenceSubscribed(_id, _from, _to);
}

void PresenceUnsubscribedTask::handle() {
	PresenceMsgHandlerAdapter::instance().presenceUnsubscribed(_id, _from, _to);
}

void RebuildBuddyApplyCacheTask::handle() {
	BuddyApplyCacheAdapter::instance().rebuildBuddyApplyCache(_id,_props);
}

void RemoveBuddyApplyCacheTask::handle() {
	BuddyApplyCacheAdapter::instance().removeBuddyApplyCache(_id, _applicant);
}

void IncUserFriendCountTask::handle() {
	UserAdapter::instance().incFriendCount(_id);
}

void DecUserFriendCountTask::handle() {
	UserAdapter::instance().decFriendCount(_id);
}

void RemoveCommonFriendTask::handle() {
	FriendFinderAdapter::instance().removeCommonFriend(_userId, _friendId);
}

void RebuildCommonFriendTask::handle() {
	FriendFinderAdapter::instance().rebuildCommonFriend(_userId);
}

void RemoveOfferTask::handle() {
	OfferFriendsAdapter::instance().removeOffer(_userId, _offerId);
}

void AddBuddyOnlineStatTask::handle() {
	OnlineCenterAdapter::instance().addBuddy(_host, _guest);
}

void RemoveBuddyOnlineStatTask::handle() {
	OnlineCenterAdapter::instance().removeBuddy(_host, _guest);
}

void DeliverSupplyTask::handle() {
	XFeedDispatcherAdapter::instance().deliverSupply(_supplier, _accepter, _ctx);
}

void RemoveBuddyGroupTask::handle() {
	BuddyGroupAdapter::instance().removeBuddy(_from, _to);
}

void RemoveBuddyNotifyTask::handle() {
	IqMsgHandlerAdapter::instance().removeBuddyNotify(_from, _to);
}

void DelInSqlTask::handle() {
//	ConnectionQuery conn(_source, CDbWServer);
//	ostringstream sqlFeedHigh;
//	sqlFeedHigh << "DELETE FROM relation_feed_high"
//		<< " WHERE SOURCE=" << _from << " AND TARGET=" << _to
//		<< " OR SOURCE=" << _to << " AND TARGET=" << _from;
//	conn.doDelete(sqlFeedHigh.str());
//	ostringstream sqlFeedLow;
//		sqlFeedLow << "DELETE FROM relation_feed_low"
//		<< " WHERE SOURCE=" << _from << " AND TARGET=" << _to
//		<< " OR SOURCE=" << _to << " AND TARGET=" << _from;
//	conn.doDelete(sqlFeedLow.str());
	
//	Statement sqlFeedHigh;
//	sqlFeedHigh << "DELETE FROM relation_feed_high"
//		<< " WHERE SOURCE=" << _from << " AND TARGET=" << _to
//		<< " OR SOURCE=" << _to << " AND TARGET=" << _from;
//	QueryRunner(_source, CDbWServer).schedule(sqlFeedHigh);
//	Statement sqlFeedLow;
//		sqlFeedLow << "DELETE FROM relation_feed_low"
//		<< " WHERE SOURCE=" << _from << " AND TARGET=" << _to
//		<< " OR SOURCE=" << _to << " AND TARGET=" << _from;
//	QueryRunner(_source, CDbWServer).schedule(sqlFeedLow);
	{	
	ConnectionHolder conn(_source, CDbWServer," ");
	mysqlpp::Query query = conn.query();
	query << "DELETE FROM relation_feed_high"
			<< " WHERE SOURCE=" << _from << " AND TARGET=" << _to
			<< " OR SOURCE=" << _to << " AND TARGET=" << _from;
//		QueryRunner(_source, CDbWServer).schedule(sqlFeedHigh);
	query.store();
	}
	{
		ConnectionHolder conn(_source, CDbWServer," ");
		mysqlpp::Query query = conn.query();
		query << "DELETE FROM relation_feed_low"
			<< " WHERE SOURCE=" << _from << " AND TARGET=" << _to
			<< " OR SOURCE=" << _to << " AND TARGET=" << _from;
//		QueryRunner(_source, CDbWServer).schedule(sqlFeedLow);
		query.store();
	}
}

void ReloadBlockUserTask::handle() {
	TalkRightAdapter::instance().reloadBlockUser(_id);
}

void RemoveRecommendFriendTask::handle() {
	RecommendFriendAdapter::instance().removeRecommendFriend(_ownerId,_otherId);
}

void RebuildFriendDistTask::handle() {
	FriendDistAdapter::instance().rebuildFriendDist(_id);
}

void LoadBuddyCacheTask::handle() {
	::xce::buddy::BuddyCacheAdapter::instance().load(_id);
}

void LoadBuddyCacheByIdTask::handle() {
	::xce::buddy::BuddyCacheByIdAdapter::instance().load(_id);
	::xce::buddy::BuddyCacheByIdAdapter::instance().load(_id);
	
}

void AddRecentlyOnlineBuddyTask::handle() {
	RecentlyOnlineBuddyAdapter::instance().add(_userid,_friendid);
}

void RemoveRecentlyOnlineBuddyTask::handle() {
	RecentlyOnlineBuddyAdapter::instance().remove(_userid,_friendid);
}

void RebuildRecentlyOnlineBuddyTask::handle() {
	RecentlyOnlineBuddyAdapter::instance().rebuild(_id);
}

void AddBuddyCacheByNameTask::handle() {
	BuddyCacheByNameAdapter::instance().add(_userid,_friendid);
}

void RemoveBuddyCacheByNameTask::handle() {
	BuddyCacheByNameAdapter::instance().remove(_userid,_friendid);
}

void LoadBuddyCacheByNameTask::handle() {
	BuddyCacheByNameAdapter::instance().load(_id);
}

void LoadBuddyRelationTask::handle() {
	if(_brp){
        	::xce::buddy::adapter::BuddyRelationCacheAdapter::instance().load(TYPEBUDDYRELATIONHOLDERS,_id,_brp);
	}
}

void LoadBuddyByIdTask::handle() {
        if(!_friends.empty()){
                ::xce::buddy::adapter::BuddyByIdLoaderAdapter::instance().load(_id,_friends);
        }
}

