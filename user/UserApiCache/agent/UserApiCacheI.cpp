#include "UserApiCacheI.h"
#include <UserApiCacheReloaderAdapter.h>
#include <UserApiCacheAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <boost/lexical_cast.hpp>

using namespace ::MyUtil;
using namespace ::xce::userapicache;
using namespace ::xce::serverstate;
using namespace boost;

void MyUtil::initialize() {
        ServiceI& service = ServiceI::instance();
        service.registerObjectCache(USER_API_CACHE, "UC", new UserApiCacheFactoryI, false);
        service.getAdapter()->add(&UserApiCacheManagerI::instance(),service.createIdentity("M", ""));

        TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserApiCache.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserApiCache.Interval", 5);

	UserApiCacheAdapter::instance().initialize();
        ServerStateSubscriber::instance().initialize("ControllerUserApiCache", &UserApiCacheManagerI::instance(), mod, interval);
	UserApiCacheManagerI::instance().setValid(true);
	UserApiCacheManagerI::instance().set_mod(mod);
}

UserApiCacheManagerI::UserApiCacheManagerI() :_valid(true) {
}

UserApiCacheDataPtr UserApiCacheManagerI::getUserApiCache(::Ice::Int userId, const Ice::Current& current) {
	MCE_WARN("UserApiCacheManagerI::getUserApiCache, userIdid=" << userId << " Oh, no! "
		<< current.con->toString().substr(current.con->toString().find("remote",25)));
	int cluster = 4;
	return new UserApiCacheData;
}

Int2UserApiCacheData1Map UserApiCacheManagerI::getUserApiCache1List(const IntSeq& userIds, const Ice::Current& current) {
	MCE_WARN("UserApiCacheManagerI::getUserApiCache1List Oh, no! "
			<< current.con->toString().substr(current.con->toString().find("remote",25)));
    return UserApiCacheAdapter::instance().GetUserApiCacheList(userIds);
}

void UserApiCacheManagerI::update(const UserApiCacheUpdateNewDataPtr& data, const ::Ice::Current&) {
}

Int2UserApiCacheDataMap UserApiCacheManagerI::getUserApiCacheList(const IntSeq& userIds, const Ice::Current& current) {
	MCE_WARN("UserApiCacheManagerI::getUserApiCacheList Oh, no! "
			<< current.con->toString().substr(current.con->toString().find("remote",25)));
    return Int2UserApiCacheDataMap();
}

bool UserApiCacheManagerI::isValid(const Ice::Current&) {
        IceUtil::RWRecMutex::RLock lock(_validMutex);
	MCE_INFO("UserApiCacheManagerI::isValid -> "<<_valid);
        return _valid;
}

void UserApiCacheManagerI::setValid(bool valid, const Ice::Current&) {
	MCE_INFO("UserApiCacheManagerI::setValid -> "<<valid);
        IceUtil::RWRecMutex::WLock lock(_validMutex);
        _valid = valid;
	if (_valid) {
		ServerStateSubscriber::instance().setStatus(1);
	} else {
		ServerStateSubscriber::instance().setStatus(0);
	}
}

void UserApiCacheManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	if (userData->data.size() <= 0) {
		return;
	}
	MCE_INFO("UserApiCacheManagerI::setData -> size : " << userData->data.size());
}

void FillTask::handle() {
	int cluster = 4;
	int mod = UserApiCacheManagerI::instance().mod();
	MyUtil::ObjectResultPtr results = new MyUtil::ObjectResult;
	for (map<long, Ice::ObjectPtr>::const_iterator i = _userData->data.begin(); i != _userData->data.end(); ++i) {
		if ((i->first % cluster) == mod) {
			results->data.insert(make_pair(i->first, new UserApiCacheEntry(UserApiCacheData1Ptr::dynamicCast(i->second))));
		}
	}
	UserApiCacheManagerI::instance().retention()->add(results->data);
}

Ice::ObjectPtr UserApiCacheFactoryI::create(Ice::Long id) {
	MCE_WARN("UserApiCacheFactoryI::create -> No Hit For Id: "<<id);
	MyUtil::IntSeq ids;
	ids.push_back(id);
	UserApiCacheReloaderAdapter::instance().getProxy(0)->reload(ids);
    return NULL;
}

MyUtil::ObjectResultPtr UserApiCacheFactoryI::create(const LongSeq& ids) {
        MCE_WARN("UserApiCacheFactoryI::create -> No Hit Size: " << ids.size());
	vector<int> userids;
        for(LongSeq::const_iterator it=ids.begin();it !=ids.end(); ++it) {
                userids.push_back((int)(*it));
        }
        UserApiCacheReloaderAdapter::instance().getProxy(0)->reload(userids);
        return NULL;
}

void UpdateTask::handle() {
        for(UserApiCacheUpdateNewDataSeq::iterator i = data_.begin(); i != data_.end(); ++i) {
                UpdateData((*i)->userid,(*i)->data);
        }
}

void UpdateTask::UpdateData(int id, const Byte2StrMap& data) {
        UserApiCacheEntryPtr olddata = UserApiCacheEntryPtr::dynamicCast(UserApiCacheManagerI::instance().retention()->find(id));
        if (olddata == 0) {
                MCE_WARN("UpdateTask::UpdateData -> Need Reload ID: "<<id);
        } else {
                UserApiCacheEntryPtr newdata = new UserApiCacheEntry(olddata);
                UpdateData(newdata, data);
                UserApiCacheManagerI::instance().retention()->add(id, newdata);
        }
}

void UpdateTask::UpdateData(UserApiCacheEntryPtr& data, const Byte2StrMap& updateddata) {
        for (Byte2StrMap::const_iterator itr = updateddata.begin(); itr != updateddata.end(); ++itr) {
                Ice::Byte field = itr->first;
                switch (field) {
			case FIELDGENDER:
                                data->set_gender(itr->second);
				break;
			case FIELDBIRTHYEAR:
				data->set_birthyear(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDBIRTHMONTH:
				data->set_birthmonth(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDBIRTHDAY:
				data->set_birthday(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDHOMEPROVINCE:
				data->set_homeprovince(itr->second);
				break;
			case FIELDHOMECITY:
				data->set_homecity(itr->second);
				break;
			case FIELDUNIV:
                                data->set_univ(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDSTAGE:
				data->set_stage(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDLEVEL:
                                data->set_level(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDSTATE:
                                data->set_state(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDNAME:
                                data->set_name(itr->second);
				break;
			case FIELDAUTH:
				data->set_auth(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDSTATUS:
                                data->set_status(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDTINYURL:
                                data->set_tinyurl(itr->second);
				break;
			case FIELDHEADURL:
				data->set_headurl(itr->second);
				break;
			case FIELDMAINURL:
				data->set_mainurl(itr->second);
				break;
			case FIELDPROFILEPRIVACY:
				data->set_profileprivacy(itr->second);
				break;
			case FIELDBASICCONFIG:
				data->set_basicconfig(boost::lexical_cast<int>(itr->second));
				break;
			case FIELDSTATUSCONFIG:
				data->set_statusconfig(boost::lexical_cast<int>(itr->second));
				break;
                        default:
                                MCE_WARN("UpdateTask::CopyData -> Invalid field: "<<field);
                }
        }
}

UserApiCacheEntry::UserApiCacheEntry(const UserApiCacheEntryPtr& entry) :
	id_(entry->id_),
	status_(entry->status_),
	name_(entry->name_),
	state_(entry->state_),
	level_(entry->level_),
	auth_(entry->auth_),
	statusconfig_(entry->statusconfig_),
	basicconfig_(entry->basicconfig_),
	profileprivacy_(entry->profileprivacy_),
	stage_(entry->stage_),
	gender_(entry->gender_),
	univ_(entry->univ_),
	birthyear_(entry->birthyear_),
	birthmonth_(entry->birthmonth_),
	birthday_(entry->birthday_),
	homeprovince_(entry->homeprovince_),
	homecity_(entry->homecity_),
	mainurl_(entry->mainurl_),
	headurl_(entry->headurl_),
	tinyurl_(entry->tinyurl_) {
}

UserApiCacheEntry::UserApiCacheEntry(const UserApiCacheDataPtr& entry) :
	id_(entry->id),
	status_(entry->status),
	name_(entry->name),
	state_(entry->state),
	auth_(entry->auth),
	statusconfig_(entry->statusconfig),
	basicconfig_(entry->basicconfig),
	profileprivacy_(entry->profileprivacy),
	stage_(entry->stage),
	gender_(entry->gender),
	univ_(entry->univ),
	birthyear_(entry->birthyear),
	birthmonth_(entry->birthmonth),
	birthday_(entry->birthday),
	homeprovince_(entry->homeprovince),
	homecity_(entry->homecity),
	mainurl_(entry->mainurl),
	headurl_(entry->headurl),
	tinyurl_(entry->tinyurl) {
}

UserApiCacheEntry::UserApiCacheEntry(const UserApiCacheData1Ptr& entry) :
	id_(entry->id),
	status_(entry->status),
	name_(entry->name),
	state_(entry->state),
	level_(entry->level),
	auth_(entry->auth),
	statusconfig_(entry->statusconfig),
	basicconfig_(entry->basicconfig),
	profileprivacy_(entry->profileprivacy),
	stage_(entry->stage),
	gender_(entry->gender),
	univ_(entry->univ),
	birthyear_(entry->birthyear),
	birthmonth_(entry->birthmonth),
	birthday_(entry->birthday),
	homeprovince_(entry->homeprovince),
	homecity_(entry->homecity),
	mainurl_(entry->mainurl),
	headurl_(entry->headurl),
	tinyurl_(entry->tinyurl) {
}

UserApiCacheData1Ptr UserApiCacheEntry::GetUserApiCacheData1() const {
	UserApiCacheData1Ptr result = new UserApiCacheData1;
	result->id = id_;
	result->status = status_;
	result->name = name_;
	result->state = state_;
	result->level = level_;
	result->auth = auth_;
	result->statusconfig = statusconfig_;
	result->basicconfig = basicconfig_;
	result->profileprivacy = profileprivacy_;
	result->stage = stage_;
	result->gender = gender_;
	result->univ = univ_;
	result->birthyear = birthyear_;
	result->birthday = birthday_;
	result->birthmonth = birthmonth_;
	result->homeprovince = homeprovince_;
	result->homecity = homecity_;
	result->mainurl = mainurl_;
	result->headurl = headurl_;
	result->tinyurl = tinyurl_;
	return result;
}

UserApiCacheDataPtr UserApiCacheEntry::GetUserApiCacheData() const {
	UserApiCacheDataPtr result = new UserApiCacheData;
	result->id = id_;
	result->status = status_;
	result->name = name_;
	result->state = state_;
	result->auth = auth_;
	result->statusconfig = statusconfig_;
	result->basicconfig = basicconfig_;
	result->profileprivacy = profileprivacy_;
	result->stage = stage_;
	result->gender = gender_;
	result->univ = univ_;
	result->birthyear = birthyear_;
	result->birthday = birthday_;
	result->birthmonth = birthmonth_;
	result->homeprovince = homeprovince_;
	result->homecity = homecity_;
	result->mainurl = mainurl_;
	result->headurl = headurl_;
	result->tinyurl = tinyurl_;
	return result;
}
