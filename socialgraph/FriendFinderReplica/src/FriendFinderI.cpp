#include <algorithm>
#include <Ice/Ice.h>
#include "FriendFinderI.h"
#include "CommonFriendFinder.h"
#include "ServiceI.h"
#include "FriendUtils.h"
#include "FriendFinderByCommonReplicaAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace ::com::xiaonei::service;
using namespace ::com::xiaonei::xce;
using namespace ::MyUtil;
using namespace ::xce::friendfinder::adapter;
using namespace xce::clusterstate;
using namespace xce::serverstate;


//---------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.registerObjectCache(COMMON_FRIEND_SET, "CFS", new CommonFriendSetFactory);
	service.getAdapter()->add(&FriendFinderManagerI::instance(), service.createIdentity(
			"FFM", ""));

	rebuildInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("rebuildInterval", 24*3600);

	// This is default. Working config stored in SvcDescriptor
	TaskManager::instance().config(TASK_LEVEL_REBUILD_LATER, ThreadPoolConfig(1, 5));
	TaskManager::instance().config(TASK_LEVEL_REBUILD_NOW, ThreadPoolConfig(1, 20));
	TaskManager::instance().config(TASK_LEVEL_REBUILD_CREATE, ThreadPoolConfig(1, 40));
	TaskManager::instance().config(TASK_LEVEL_REBUILD_NORMAL, ThreadPoolConfig(1, 10));

	TaskManager::instance().config(TASK_LEVEL_REMOVE, ThreadPoolConfig(1, 10));
	TaskManager::instance().config(TASK_LEVEL_RECORD, ThreadPoolConfig(1, 100));

	service.registerXceObserver(new FriendFinderServiceSchemaI);
	srand(time(NULL));

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Controller.Mod");
        int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Controller.Interval", 5);
        string ctlEndpoint = service.getCommunicator()->getProperties()->getProperty("Controller.ctlEndpoint");
        // 向Controller注册
        ServerStateSubscriber::instance().initialize(ctlEndpoint,
                        &FriendFinderManagerI::instance(), mod, sInterval);

}

//*********************************************************************************
Ice::ObjectPtr CommonFriendSetFactory::create(Ice::Long id) {
	TaskManager::instance().execute(new CreateTask(id));
	return new CommonFriendSet(CommonFriendSeq());
}

void CreateTask::handle(){
	MCE_INFO("CommonFriendDelayCreateTask::create -> " << _id);
	Statement oss;
	oss << "SELECT friend_id, weight, shares FROM "
			<< TableName::common_friends_table(_id) << " WHERE owner_id=" << _id;
	CommonFriendSeq commFriends;
	CommonFriendSetResultHandler handler(commFriends);
	QueryRunner("friendfinder_common_friends", CDbRServer,
			TableName::common_friends_patten(_id)).query(oss, handler);

	sort(commFriends.begin(), commFriends.end(), CommonFriendWeight);
	MCE_INFO("CommonFriendCreateTask::create -> id=" << _id << " size="
                                        << commFriends.size());
	if (commFriends.size() < CF_REBUILD_THRESHOLD) {
		if ((commFriends.empty() && (_id < 100000000 || _id > 300000000)) || 
				(!commFriends.empty() && commFriends.size() < CF_WEBPAGE_THRESHOLD) ) {
			MCE_INFO("CommonFriendCreateTask::create-> normal user, id=" << _id << ", commFriends.size()=" << commFriends.size());
			TaskManager::instance().execute(
					new CommonFriendBuilderTask(_id, TASK_LEVEL_REBUILD_NORMAL));
		} else {
			MCE_INFO("CommonFriendCreateTask::create -> old user, id=" << _id << " size="
					<< commFriends.size());
			TaskManager::instance().execute(
					new CommonFriendBuilderTask(_id, TASK_LEVEL_REBUILD_LATER));
		}
	}
	CommonFriendSetPtr result = new CommonFriendSet(commFriends);	
	ServiceI::instance().getObjectCache()->addObject(COMMON_FRIEND_SET, _id, result);
}

//*********************************************************************************

CommonFriendSeq FriendFinderManagerI::getCommonFriend(::Ice::Int id,
		::Ice::Int begin, ::Ice::Int limit, const ::Ice::Current& current) {
	MCE_INFO("[FriendFinderManagerI] getCommonFriend(" << id << ") " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	CommonFriendSetPtr friendset = ServiceI::instance().locateObject<
			CommonFriendSetPtr> (COMMON_FRIEND_SET, id);
	CommonFriendSeq res;
	if (current.ctx.count(RANDOM_COMMON_FRIEND)) {
		res = friendset->getItems(id, limit);
	} else {
		res = friendset->getItems(id, begin, limit);
	}
	return res;
}

void FriendFinderManagerI::reloadCommonFriend(::Ice::Int userId,
		const ::Ice::Current& current) {
	MCE_INFO("[FriendFinderManagerI] reloadCommonFriend(" << userId << ") " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	ServiceI::instance().removeObject(COMMON_FRIEND_SET, userId);
}

void FriendFinderManagerI::removeCommonFriend(::Ice::Int userId,
		::Ice::Int friendId, const ::Ice::Current& current) {
	MCE_INFO("[FriendFinderManagerI] removeCommonFriend(" << userId << ", " << friendId << ") " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	if (current.ctx.count(REMOVE_WITH_BLOCK)) {
		TaskManager::instance().execute(
				new CommonFriendRemoveTask(userId, friendId, true));
	} else {
		TaskManager::instance().execute(
				new CommonFriendRemoveTask(userId, friendId, false));
	}
}

::Ice::Int FriendFinderManagerI::getSharedFriendCount(::Ice::Int id1,
		::Ice::Int id2, const ::Ice::Current& current) {
	MCE_INFO("[FriendFinderManagerI] getSharedFriendCount " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return getSharedFriend(id1, id2, 0, -1, current).size();
}

MyUtil::IntSeq FriendFinderManagerI::getSharedFriend(::Ice::Int id1, ::Ice::Int id2,
		::Ice::Int begin, ::Ice::Int limit, const ::Ice::Current& current) {
	MCE_INFO("[FriendFinderManagerI] getSharedFriend " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	MyUtil::IntSeq ids;
	ids.push_back(id1);
	ids.push_back(id2);
		MyUtil::Int2IntSeqMap friends = RelationBatchHelper::getFriendSeqBatch(ids);
	MyUtil::IntSeq f1 = friends[id1];
	MyUtil::IntSeq f2 = friends[id2];
	MyUtil::IntSeq tmp = calc_shares(f1,f2);
	MyUtil::IntSeq result;
	for (MyUtil::IntSeq::iterator it = tmp.begin(); it != tmp.end(); ++it) {
		if (begin > 0) {
			begin--;
			continue;
		}
		result.push_back(*it);
		if (limit < 0) {
			continue;
		}
		if ((int) (result.size()) >= limit) {
			break;
		}
	}
	MCE_INFO("getSharedFriend(or size) user1=" << id1 << "(" << f1.size() << " friends)  user2=" << id2 << "(" << f2.size() << " friends)  shares=" << tmp.size() << "  limit=" << limit );
	return result;
}

MyUtil::IntSeq FriendFinderManagerI::calc_shares(const MyUtil::IntSeq& f1, const MyUtil::IntSeq& f2){
	MyUtil::IntSeq tmp;
	MyUtil::IntSeq::size_type i = 0, j = 0;
	while (i < f1.size() && j < f2.size()) {
		if (f1[i] > f2[j]) {
			++j;
		} else {
			if (f1[i] == f2[j]) {
				tmp.push_back(f1[i]);
				++i;
				++j;
			} else {
				++i;
			}
		}
	}
	random_shuffle(tmp.begin(), tmp.end());
	return tmp;
}

Int2ShareFriendMap FriendFinderManagerI::getSharedFriends(::Ice::Int fromId,
		const MyUtil::IntSeq& toIds, ::Ice::Int limit, const ::Ice::Current& current) {
	MCE_INFO("[FriendFinderManagerI] getSharedFriends " << current.con->toString().substr(current.con->toString().find("remote",25)));
	MyUtil::IntSeq ids(toIds);
	ids.push_back(fromId);
	Int2IntSeqMap raw = RelationBatchHelper::getFriendSeqBatch(ids);

	Int2ShareFriendMap result;
	for (MyUtil::IntSeq::const_iterator it = toIds.begin(); it != toIds.end(); ++it) {
		MyUtil::IntSeq share_result = calc_shares(raw[fromId],raw[*it]);
		ShareFriend share_friend;
		share_friend.count = share_result.size();
		for (MyUtil::IntSeq::const_iterator resIt = share_result.begin(); resIt
				!= share_result.end(); ++resIt) {
			share_friend.shares.push_back(*resIt);
			if ((int)share_friend.shares.size() >= limit) {
				break;
			}
		}
		result.insert(make_pair(*it, share_friend));
	}
	return result;
}

void FriendFinderManagerI::rebuildCommonFriend(::Ice::Int userId,
		const ::Ice::Current& current) {
	int level = TASK_LEVEL_REBUILD_LATER;
	if (current.ctx.count(REBUILD_IMMEDIATELY)) {
		level = TASK_LEVEL_REBUILD_NORMAL;
		MCE_INFO("rebuildCommonFriend NORMAL userId=" << userId << " " << current.con->toString().substr(current.con->toString().find("remote",25)));
	} else{
		MCE_INFO("rebuildCommonFriend LATER userId=" << userId << " " << current.con->toString().substr(current.con->toString().find("remote",25)));
	}
	TaskManager::instance().execute(new CommonFriendBuilderTask(userId, level));
}

//*********************************************************************************

void CommonFriendBuilderTask::handle() {
	try {
		CommonFriendCalculator calc(_id);
		CommonFriendSeq comm(calc.getComm());

		// set this CommonFriendSeq to FriendFinderByCommon
		try {
			FriendFinderByCommonAdapter::instance().setCommonFriendSeq( _id, comm );
		} catch (Ice::Exception& e) {
			MCE_WARN("CommonFriendSetCreationTask::handle::FriendFinderByCommonAdapter " << _id
					<< " ice exception " << e.what());
		} catch (std::exception& e) {
			MCE_WARN("CommonFriendSetCreationTask::handle::FriendFinderByCommonAdapter " << _id
					<< " std exception " << e.what());
		} catch (...) {
			MCE_WARN("CommonFriendSetCreationTask::handle::FriendFinderByCommonAdapter " << _id << " exception");
		}

		MCE_INFO("CommonFriendBuilderTask::handle: _id, comm.size(): " << _id
				<< ", " << comm.size());
		if (comm.empty()) {
			return;
		}

		Statement sqlDelete;
		sqlDelete << "DELETE FROM " << TableName::common_friends_table(_id)
				<< " where owner_id=" << _id;
		QueryRunner("friendfinder_common_friends", CDbWServer,
				TableName::common_friends_patten(_id)).execute(sqlDelete);
		
		Statement sqlInsert;
		ostringstream sqllog;
		sqlInsert << "INSERT INTO " << TableName::common_friends_table(_id)
				<< " (owner_id, friend_id, create_time, weight, shares) VALUES ";
		sqllog << "SAVE " << _id << " FOR";
		mysqlpp::DateTime now(time(NULL));
		for (CommonFriendSeq::iterator it = comm.begin(); (it != comm.end())
				&& (it < comm.begin() + CF_SAVE_THRESHOLD); ++it) {
			sqllog << " " << it->userId;
			if (it != comm.begin()) {
				sqlInsert << ", ";
			}
			sqlInsert << "(" << _id << ", " << it->userId << ", "
					<< mysqlpp::quote << now << ", " << it->weight << ", ";
			sqlInsert << mysqlpp::quote << IntSeqSerialize::serialize(
					it->shares) << ")";
		}
		sqlInsert << " ON DUPLICATE KEY UPDATE " << "create_time=VALUES(create_time), weight=VALUES(weight), shares=VALUES(shares);"; 
		
		QueryRunner("friendfinder_common_friends", CDbWServer,
				TableName::common_friends_patten(_id)).execute(sqlInsert, sqllog.str());

		CommonFriendSetPtr result = new CommonFriendSet(comm);
		ServiceI::instance().addObject(result, COMMON_FRIEND_SET, _id);
	} catch (Ice::Exception& e) {
		MCE_WARN("CommonFriendSetCreationTask::handle " << _id
				<< " ice exception " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("CommonFriendSetCreationTask::handle " << _id
				<< " std exception " << e.what());
	} catch (...) {
		MCE_WARN("CommonFriendSetCreationTask::handle " << _id << " exception");
	}
}

//*********************************************************************************

void CommonFriendRemoveTask::handle() {
	try {
		Statement sqlDelete;
		sqlDelete << "DELETE FROM " << TableName::common_friends_table(_userId)
				<< " WHERE owner_id=" << _userId << " AND friend_id=" << _friendId;
		QueryRunner("friendfinder_common_friends", CDbWServer,
				TableName::common_friends_patten(_userId)).execute(sqlDelete);
		if (_block) {
			Statement sqlWithBlock;
			sqlWithBlock << "INSERT INTO "
					<< TableName::common_friends_block_table(_userId)
					<< " (owner_id, block_id) VALUES (" << _userId << ", "
					<< _friendId << ")";
			sqlWithBlock << " ON DUPLICATE KEY UPDATE " << "block_id=" << _friendId << ";";
			QueryRunner("friendfinder_common_friends", CDbWServer,
					TableName::common_friends_block_patten(_userId)).execute(
					sqlWithBlock);
		}
		CommonFriendSetPtr friendset = ServiceI::instance().findObject<
				CommonFriendSetPtr> (COMMON_FRIEND_SET, _userId);
		if (!friendset) {
			return;
		}
		uint exSize = friendset->size();
		friendset->removeItem(_friendId);
		if (friendset->size() < CF_WEBPAGE_THRESHOLD && 0 != exSize) {
			MCE_INFO("CommonFriendRemoveTask::handle ->CommonFriendBuilderTask NORMAL, friends->size()=" 
					<< friendset->size() << ", exSize=" << exSize);
			TaskManager::instance().execute(
					new CommonFriendBuilderTask(_userId, TASK_LEVEL_REBUILD_NORMAL));
		} else if (friendset->size() < CF_REBUILD_THRESHOLD && exSize >= CF_REBUILD_THRESHOLD) {
			MCE_INFO("CommonFriendRemoveTask::handle ->CommonFriendBuilderTask LATER, friends->size()=" 
					<< friendset->size() << ", exSize=" << exSize);
			TaskManager::instance().execute(
					new CommonFriendBuilderTask(_userId, TASK_LEVEL_REBUILD_LATER));
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("CommonFriendRemoveTask::handle " << _userId
				<< " ice exception " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("CommonFriendRemoveTask::handle " << _userId
				<< " std exception " << e.what());
	} catch (...) {
		MCE_WARN("CommonFriendRemoveTask::handle " << _userId << " exception");
	}
}

//*********************************************************************************

void CommonFriendRecordTask::handle() {
	try {
		FriendFinderByCommonAdapter::instance().setCommonFriendSeq(_userId,_cfs);
	} catch (Ice::Exception& e) {
		MCE_WARN("CommonFriendRecordTask::handle " << _userId
				<< " ice exception " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("CommonFriendRecordTask::handle " << _userId
				<< " std exception " << e.what());
	} catch (...) {
		MCE_WARN("CommonFriendRecordTask::handle " << _userId << " exception");
	}
}

//***************************************************************************

CommonFriendSetResultHandler::CommonFriendSetResultHandler(
		CommonFriendSeq& results) :
	_results(results) {
}

CommonFriendSetResultHandler::~CommonFriendSetResultHandler() {
}

bool CommonFriendSetResultHandler::handle(mysqlpp::Row& row) const {
	CommonFriend cf;
	cf.userId = (int) row["friend_id"];
	cf.weight = (int) row["weight"];
/*
#ifndef NEWARCH
	string sharesstr = row["shares"].get_string();
#else
	string sharesstr = row["shares"].c_str();
#endif
*/
	cf.shares = IntSeqSerialize::unserialize(row["shares"]);
	_results.push_back(cf);
	return true;
}

//***************************************************************************

FriendStatusResultHandler::FriendStatusResultHandler(CommonFriendSeq& results):_results(results){

}
FriendStatusResultHandler::~FriendStatusResultHandler(){

}
bool FriendStatusResultHandler::handle( mysqlpp::Row& row ) const {
	int id=row["id"];
	int status=row["status"];
	if( status != 0 ) {
		for( CommonFriendSeq::iterator it=_results.begin(); it!=_results.end(); ++it ) {
			if( (*it).userId == id ) {
				_results.erase(it);
				break;
			}
		}
	}
	return true;
}

//***************************************************************************

void FriendFinderServiceSchemaI::configure(const Ice::PropertiesPtr& props) {

	int later_min = props->getPropertyAsIntWithDefault(
			"Service.ThreadPool.TaskHandler.RebuildLater.Size", 1);
	int later_max = props->getPropertyAsIntWithDefault(
			"Service.ThreadPool.TaskHandler.RebuildLater.SizeMax", 1);
	ThreadPoolConfig later_cfg(later_min, later_max);
	TaskManager::instance().config(TASK_LEVEL_REBUILD_LATER, later_cfg);

	int now_min = props->getPropertyAsIntWithDefault(
			"Service.ThreadPool.TaskHandler.RebuildNow.Size", 1);
	int now_max = props->getPropertyAsIntWithDefault(
			"Service.ThreadPool.TaskHandler.RebuildNow.SizeMax", 1);
	ThreadPoolConfig now_cfg(now_min, now_max);
	TaskManager::instance().config(TASK_LEVEL_REBUILD_NOW, now_cfg);
}
