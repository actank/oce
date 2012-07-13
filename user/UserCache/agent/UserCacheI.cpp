#include "UserCacheI.h"
#include <ServiceI.h>
#include <QueryRunner.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"


using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::usercache;
using namespace xce::serverstate;

// ========== MyUtil::initialize ==========================================
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(USER_CACHE, "UC", new UserCacheFactoryI, false);
	service.getAdapter()->add(&UserCacheManagerI::instance(),service.createIdentity(USER_CACHE_MANAGER, ""));

	TaskManager::instance().config(TASK_LEVEL_UPDATE, MyUtil::ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_SEND, MyUtil::ThreadPoolConfig(1, 10));
	TaskManager::instance().config(TASK_LEVEL_LOAD, MyUtil::ThreadPoolConfig(1, 10));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserCache.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserCache.Interval", 5);
	int cluster = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserCache.Cluster", 1);

	// 加入向Controller注册
	ServerStateSubscriber::instance().initialize("ControllerUserCacheN", &UserCacheManagerI::instance(), mod, interval,new MyUtil::OceChannel, "3000", "3000", "1048576");

	UserCacheManagerI::instance().setValid(true);
	UserCacheManagerI::instance().set_mod(mod);
	UserCacheManagerI::instance().set_cluster(cluster);
	
}

// =========== UserCacheManagerI =============================================

::MyUtil::Str2StrMap UserCacheManagerI::getUserCache(::Ice::Int userId,
		const Ice::Current& current) {
	MCE_WARN("UserCacheManagerI::getUserCache -> id: " << userId << " Oh, no!"
		<< current.con->toString().substr(current.con->toString().find("remote",25)));
	return Str2StrMap();
}

UserCacheData3Map UserCacheManagerI::getUserCacheMap3(
		const ::MyUtil::IntSeq& userIds, const Ice::Current& current) {

	ostringstream msg;
	msg << "UserCacheManagerI::getUserCacheMap3 -> size: " << userIds.size() << " "
	    << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(msg.str());
	
	std::map<int, UserCachePtr> objres = xce::usercache::UserCacheAdapter::instance().GetUserCache(userIds);

	UserCacheData3Map result;

	MyUtil::IntSeq rebuildIds;

	for (std::map<int, UserCachePtr>::iterator it = objres.begin(); it != objres.end(); ++it) {
		if( it->second != NULL )
		{
			result.insert(make_pair(it->first, trans(UserCachePtr::dynamicCast((*it).second))));
		}
	}

	return result;
}

UserCacheData3Ptr UserCacheManagerI::trans( xce::usercache::UserCachePtr data )
{
	UserCacheData3Ptr res = new UserCacheData3;
	res->id = data->id();
	res->status = data->status();
	res->univ = data->univ();
	res->state = data->state();
	res->level = data->level();
	res->name = data->name();
	res->tinyUrl = data->tinyurl();
	res->online = data->online();
	if( data->gender() == "男生" )
	{
		res->gender = 1; 
	}else if ( data->gender() == "女生" )
	{
		res->gender = 2; 
	}else
	{
		res->gender = 0;
	}
	return res;
}

UserCacheData3Seq UserCacheManagerI::getUserCacheList3(
		const ::MyUtil::IntSeq& userIds, const Ice::Current& current) {
	ostringstream msg;
	msg << "UserCacheManagerI::getUserCacheList3 -> size: " << userIds.size() << " "
	    << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(msg.str());

	return UserCacheData3Seq();
}

::MyUtil::Str2StrMapSeq UserCacheManagerI::getUserCacheList(
		const ::MyUtil::IntSeq& userIds, const Ice::Current& current) {
	MCE_WARN("UserCacheManagerI::getUserCacheList -> Oh, no!" << current.con->toString().substr(current.con->toString().find("remote",25)));
	return MyUtil::Str2StrMapSeq();
}

void UserCacheManagerI::load(const ::MyUtil::IntSeq& ids, const Ice::Current&) {
	MCE_WARN("[UserCacheManagerI::load] INVALID impl UserCacheManagerI::load");
}

bool UserCacheManagerI::isValid(const Ice::Current&) {
	IceUtil::RWRecMutex::RLock lock(_validMutex);
	return _valid;
}

void UserCacheManagerI::setValid(bool valid, const Ice::Current&) {
	MCE_INFO("UserCacheManagerI::setValid -> "<<valid);
	IceUtil::RWRecMutex::WLock lock(_validMutex);
	_valid = valid;
	if (_valid) {
		ServerStateSubscriber::instance().setStatus(1);
	} else {
		ServerStateSubscriber::instance().setStatus(0);
	}
}

void UserCacheManagerI::setOnline(const ::MyUtil::Int2IntMap& data,
		const Ice::Current&) {
	MCE_DEBUG("UserCacheManagerI::setOnline -> size: "<<data.size());
}

void UserCacheManagerI::reloadSetOnline(const ::MyUtil::Int2IntMap& data,
		const Ice::Current&) {
	MCE_DEBUG("UserCacheManagerI::reloadSetOnline -> size: "<<data.size());
}

void UserCacheManagerI::update(const UserCacheUpdateNewDataPtr& data, const Ice::Current& current) {
	MCE_INFO("UserCacheManagerI::update -> userid: "<<data->userid << " " <<current.con->toString().substr(current.con->toString().find("remote",25)));
}

void UserCacheManagerI::setData(const ::MyUtil::ObjectResultPtr& userData, const Ice::Current& current) {

        MCE_INFO("UserCacheManagerI::setData -> size: " << userData->data.size()<< " "<< current.con->toString().substr(current.con->toString().find("remote",25)));
}

Ice::ObjectPtr UserCacheFactoryI::create(Ice::Long id) {
	MCE_INFO("UserCacheFactoryI::create, id=" << id);
	return NULL;
}

MyUtil::ObjectResultPtr UserCacheFactoryI::create(const MyUtil::LongSeq& ids) {
	MCE_DEBUG("Loading " << ids.size() << " objects");
	return NULL;
}
