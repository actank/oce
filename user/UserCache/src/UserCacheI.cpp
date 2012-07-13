#include "UserCacheI.h"
#include "RebuildOnline.h"
#include <ServiceI.h>
#include <QueryRunner.h>
#include <UserCacheReloaderAdapter.h>
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
	ServerStateSubscriber::instance().initialize("ControllerUserCacheN", &UserCacheManagerI::instance(), mod, interval);

	UserCacheManagerI::instance().setValid(false);
	UserCacheManagerI::instance().set_mod(mod);
	UserCacheManagerI::instance().set_cluster(cluster);
	
	UserCacheReloaderAdapter::instance().start(128*1024).detach();
	
	RebuildOnline::instance().start(65535).detach();
}

// =========== UserCacheManagerI =============================================

::MyUtil::Str2StrMap UserCacheManagerI::getUserCache(::Ice::Int userId,
		const Ice::Current& current) {
	MCE_WARN("UserCacheManagerI::getUserCache -> id: " << userId << " Oh, no!"
		<< current.con->toString().substr(current.con->toString().find("remote",25)));
	int cluster = cluster_;
	if (userId % cluster == mod_) {
		return ServiceI::instance().locateObject<UserCacheEntryPtr> (USER_CACHE,userId)->GetUserCacheEntryProp();
	}
	return Str2StrMap();
}

UserCacheData3Map UserCacheManagerI::getUserCacheMap3(
		const ::MyUtil::IntSeq& userIds, const Ice::Current& current) {

	ostringstream msg;
	msg << "UserCacheManagerI::getUserCacheMap3 -> size: " << userIds.size() << " "
	    << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(msg.str());
	
	ObjectResultPtr objres = getUserCacheData(userIds);

	UserCacheData3Map result;

	MyUtil::IntSeq rebuildIds;

	for (ObjectMap::iterator it = objres->data.begin(); it != objres->data.end(); ++it) {
		result.insert(make_pair(it->first, UserCacheEntryPtr::dynamicCast((*it).second)->GetUserCacheData3NoRebuild(rebuildIds)));
	}
	
	if(rebuildIds.size() > 0)
	{
		RebuildOnline::instance().rebuild(rebuildIds);
	}

	return result;
}

UserCacheData3Seq UserCacheManagerI::getUserCacheList3(
		const ::MyUtil::IntSeq& userIds, const Ice::Current& current) {
	ostringstream msg;
	msg << "UserCacheManagerI::getUserCacheList3 -> size: " << userIds.size() << " "
	    << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(msg.str());

	ObjectResultPtr objres = getUserCacheData(userIds);
	
	UserCacheData3Seq result;

	MyUtil::IntSeq rebuildIds;

	result.reserve(objres->data.size());

	for (ObjectMap::iterator it = objres->data.begin(); it != objres->data.end(); ++it) {
		result.push_back(UserCacheEntryPtr::dynamicCast((*it).second)->GetUserCacheData3NoRebuild(rebuildIds));
	}
	
	if(rebuildIds.size() > 0)
	{
		RebuildOnline::instance().rebuild(rebuildIds);
	}

	return result;
}

::MyUtil::Str2StrMapSeq UserCacheManagerI::getUserCacheList(
		const ::MyUtil::IntSeq& userIds, const Ice::Current& current) {
	MCE_WARN("UserCacheManagerI::getUserCacheList -> Oh, no!" << current.con->toString().substr(current.con->toString().find("remote",25)));
	ObjectResultPtr objres = getUserCacheData(userIds);
	MyUtil::Str2StrMapSeq result;
	for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it
			!= userIds.end(); ++it) {
		ObjectMap::iterator hasRes = objres->data.find(*it);
		if (hasRes != objres->data.end()) {
			result.push_back(UserCacheEntryPtr::dynamicCast(hasRes->second)->GetUserCacheEntryProp());
		} else {
			MCE_WARN("Not Exists: " << *it);
		}
	}
	return result;
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
	for (Int2IntMap::const_iterator it = data.begin(); it != data.end(); ++it) {
		 MCE_DEBUG("UserCacheManagerI::setOnline userID "<< it->first<< " onlineType : " << it->second);
		setOnline(it->first, it->second);
	}

}

void UserCacheManagerI::reloadSetOnline(const ::MyUtil::Int2IntMap& data,
		const Ice::Current&) {
	MCE_DEBUG("UserCacheManagerI::reloadSetOnline -> size: "<<data.size());
	for (Int2IntMap::const_iterator it = data.begin(); it != data.end(); ++it) {
		reloadSetOnline(it->first, it->second);
	}
}

void UserCacheManagerI::setOnline(int userId, int online) {
	int cluster = cluster_;
	if (userId % cluster != mod_) {
		return;
	}
	UserCacheEntryPtr obj;
	if ( isValid() )
	{
		obj = ServiceI::instance().locateObject<UserCacheEntryPtr>(USER_CACHE,userId);
	}
	else
	{
		obj = ServiceI::instance().findObject<UserCacheEntryPtr>(USER_CACHE,userId);
	}
	if (obj) {
		UserCacheEntryPtr result = new UserCacheEntry(obj);
		result->set_online(online);
		if (online) {
			result->set_lastonlinetime(time(0));
		} else {
			result->set_lastonlinetime(0);
		}
		ServiceI::instance().addObject(result, USER_CACHE, userId);
	}

}

void UserCacheManagerI::reloadSetOnline(int userId, int online) {
	int cluster = cluster_;
	if (userId % cluster != mod_) {
		return;
	}
	UserCacheEntryPtr obj;
	if ( isValid() )
	{
		obj = ServiceI::instance().locateObject<UserCacheEntryPtr>(USER_CACHE,userId);
	}
	else
	{
		obj = ServiceI::instance().findObject<UserCacheEntryPtr>(USER_CACHE,userId);
	}
	if (obj) {
		UserCacheEntryPtr result = new UserCacheEntry(obj);
		result->reload_set_online(online);
		if (online) {
			result->set_lastonlinetime(time(0));
		} else {
			result->set_lastonlinetime(0);
		}
		ServiceI::instance().addObject(result, USER_CACHE, userId);
	}
}

void UserCacheManagerI::update(const UserCacheUpdateNewDataPtr& data, const Ice::Current& current) {
	MCE_INFO("UserCacheManagerI::update -> userid: "<<data->userid << " " <<current.con->toString().substr(current.con->toString().find("remote",25)));
	TaskManager::instance().execute(new UpdateTask(data, current));
}

void UserCacheManagerI::setData(const ::MyUtil::ObjectResultPtr& userData, const Ice::Current& current) {

        MCE_INFO("UserCacheManagerI::setData -> size: " << userData->data.size()<< " "<< current.con->toString().substr(current.con->toString().find("remote",25)));
	if (userData->data.size() == 0){
		return;
	}
	int cluster = cluster_;
	MyUtil::ObjectResultPtr results = new MyUtil::ObjectResult;
	
	for (map<long, Ice::ObjectPtr>::const_iterator i = userData->data.begin(); i != userData->data.end(); ++i) {
		if ((i->first % cluster) == mod_) {
			results->data.insert(make_pair(i->first, new UserCacheEntry(UserCacheData3Ptr::dynamicCast(i->second))));
		}
	}
	ServiceI::instance().getObjectCache()->addObjects(USER_CACHE, results->data);
}

ObjectResultPtr UserCacheManagerI::getUserCacheData(
		const MyUtil::IntSeq& userIds) {
	int cluster = cluster_;
	MyUtil::LongSeq uids;
	for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it
			!= userIds.end(); ++it) {
		if ((*it) % cluster == mod_) {
			uids.push_back(*it);
		}
	}
	return ServiceI::instance().getObjectCache()->locateObjects(USER_CACHE, uids);
}

Ice::ObjectPtr UserCacheFactoryI::create(Ice::Long id) {
	MCE_INFO("UserCacheFactoryI::create, id=" << id);
	MyUtil::IntSeq ids;
	ids.push_back(id);
	UserCacheReloaderAdapter::instance().invoke(ids);
	return NULL;
}

UserCacheEntry::UserCacheEntry(const UserCacheEntryPtr& entry) :
	id_(entry->id_),
	status_(entry->status_),
	univ_(entry->univ_),
	state_(entry->state_),
	level_(entry->level_),
	name_(entry->name_),
	tinyUrl_(entry->tinyUrl_),
	gender_online_(entry->gender_online_),
	lastonlinetime_(entry->lastonlinetime_) {
}

UserCacheEntry::UserCacheEntry(const UserCacheData3Ptr& data) :
	id_(data->id),
	status_(data->status),
	univ_(data->univ),
	state_(data->state),
	level_(data->level),
	name_(data->name),
	tinyUrl_(data->tinyUrl) {
	gender_online_ = 0;
	gender_online_ |= (int)(GENDER_MASK&(int)data->gender) << SHIFT_BIT;
	gender_online_ |= (int)(ONLINE_MASK&(int)data->online);
	if (data->online) {
		lastonlinetime_ = time(0);
	} else {
		lastonlinetime_ = 0;
	}
}

MyUtil::Str2StrMap UserCacheEntry::GetUserCacheEntryProp() const {
	MyUtil::Str2StrMap props;

	props[CUserCacheID] = boost::lexical_cast<string>(id_);
	props[CUserCacheSTATUS] = boost::lexical_cast<string>(status_);
	props[CUserCacheNAME] = name_;
	props[CUserCacheUNIV] = boost::lexical_cast<string>(univ_);
	props[CUserCacheTINYURL] = tinyUrl_;
	props[CUserCacheSTATE] = boost::lexical_cast<string>(state_);
	props[CUserCacheLEVEL] = boost::lexical_cast<string>(level_);

	props[CUserCacheONLINE] = boost::lexical_cast<string>(GENDER_MASK&gender_online_);
	int gender = ((GENDER_MASK_RESULT&gender_online_) >> SHIFT_BIT);
	if (gender == 2) {
		props[CUserCacheGENDER] = "女生";
	} else if (gender == 1) {
		props[CUserCacheGENDER] = "男生";
	} else {
		props[CUserCacheGENDER] = "";
	}

	if (state_&1) {
		props[CUserCacheSTAR] = boost::lexical_cast<string>(1);
	} else {
		props[CUserCacheSTAR] = boost::lexical_cast<string>(0);
	}

	props[CUserCacheHEADURL] = "";
	props[CUserCacheDOING] = "";
	
	if ((ONLINE_MASK&gender_online_) && ((time(0) - lastonlinetime_) > 360)) {
		MyUtil::IntSeq rebuildIds;
		rebuildIds.push_back(id_);
		RebuildOnline::instance().rebuild(rebuildIds);
	}

	return props;
}

UserCacheData3Ptr UserCacheEntry::GetUserCacheData3NoRebuild(MyUtil::IntSeq& rebuildIds) const {

	UserCacheData3Ptr obj = new UserCacheData3;
	obj->id = id_;
	obj->status = status_;
	obj->univ = univ_;
	obj->state = state_;
	obj->level = level_;
	obj->name = name_;
	obj->tinyUrl = tinyUrl_;
	int gender_online(gender_online_);
	obj->online = (Ice::Byte)(ONLINE_MASK&gender_online);
	obj->gender = (Ice::Byte)((GENDER_MASK_RESULT&gender_online) >> SHIFT_BIT);
	if (obj->online && ((time(0) - lastonlinetime_) > 360)) {
		rebuildIds.push_back(id_);
	}

	return obj;
}

MyUtil::ObjectResultPtr UserCacheFactoryI::create(const MyUtil::LongSeq& ids) {
	MCE_DEBUG("Loading " << ids.size() << " objects");
	MyUtil::IntSeq userids;
	for (MyUtil::LongSeq::const_iterator i = ids.begin(); i != ids.end(); ++i) {
		MCE_INFO("UserCacheFactoryI::create(ids), id=" << *i);
		userids.push_back(*i);
	}
	UserCacheReloaderAdapter::instance().invoke(userids);
	return NULL;
}

// ========= Tasks ======================================================
void UpdateTask::handle(){
	UserCacheEntryPtr olddata = ServiceI::instance().locateObject<UserCacheEntryPtr>(USER_CACHE , data_->userid);
	if (olddata == 0) {
		MCE_WARN("UpdateTask::handle -> Need Reload ID: "<<data_->userid);
	} else {
		UserCacheEntryPtr newdata = new UserCacheEntry(olddata);
		UpdateData(newdata, data_->data);
		ServiceI::instance().addObject(newdata, xce::usercache::USER_CACHE, data_->userid);
	}
}

void UpdateTask::UpdateData(UserCacheEntryPtr& data, const Byte2StrMap& updateddata) {
	for (Byte2StrMap::const_iterator itr = updateddata.begin(); itr != updateddata.end(); ++itr) {
		Ice::Byte field = itr->first;
		switch (field) {
			case FIELDSTATUS:
				data->set_status(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDNAME:
				data->set_name(itr->second);
				break;
			case FIELDUNIV:
				data->set_univ(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDTINYURL:
				data->set_tinyUrl(itr->second);
				break;
			case FIELDSTATE:
				data->set_state(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDLEVEL:
				data->set_level(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDGENDER:
				int gender;
				if (itr->second == "男生") {
					gender = 1;
				} else if (itr->second == "女生") {
					gender = 2;
				} else {
					gender = 0;
				}
				
				data->set_gender(gender);
				break;
			case FIELDONLINE:
				//data->set_online(boost::lexical_cast<int>(itr->second));
				break;
			default:
				MCE_WARN("UpdateTask::CopyData -> Invalid field: "<<field);
		}
	}
}

