#include "UserAdsCacheI.h"
#include <UserAdsCacheLoaderAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"

using namespace ::MyUtil;
using namespace ::xce::useradscache;
using namespace ::xce::serverstate;
using namespace ::com::xiaonei::platform::storage;
void MyUtil::initialize() {
        ServiceI& service = ServiceI::instance();
        service.registerObjectCache(USER_ADS_CACHE_URL, "UAC", new UserAdsCacheFactoryI, false);
        service.getAdapter()->add(&UserAdsCacheManagerI::instance(),service.createIdentity(USER_ADS_CACHE_URL_MANAGER, ""));

        TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserAdsCache.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserAdsCache.Interval", 5);

	UserAdsCacheManagerI::instance().set_mod(mod);

        ServerStateSubscriber::instance().initialize("ControllerUserAdsCache", &UserAdsCacheManagerI::instance(), mod, interval);
}

UserAdsCacheManagerI::UserAdsCacheManagerI() :_valid(false) {
}

UserAdsCacheDataPtr UserAdsCacheManagerI::getUserAdsCache(::Ice::Int userId, const Ice::Current& current) {
	ostringstream msg;
	msg << "UserAdsCacheManagerI::getUserAdsCache id=" << userId << " "
	    << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(msg.str());

        return getUserAdsCacheData(userId);
}

Int2UserAdsCacheDataMap UserAdsCacheManagerI::getUserAdsCacheList(const IntSeq& userIds, const Ice::Current& current) {
	ostringstream msg;
	msg << "UserAdsCacheManagerI::getUserAdsCacheList -> size: " << userIds.size() << " "
	    << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(msg.str());

        ObjectResultPtr objres = getUserAdsCacheData(userIds);
        Int2UserAdsCacheDataMap result;
        for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
                ObjectMap::iterator hasRes = objres->data.find(*it);
                if (hasRes != objres->data.end()) {
                        result[*it]=toOld(UserAdsCacheUrlDataPtr::dynamicCast(hasRes->second));
                } else {
                        MCE_WARN("Not Exists: " << *it);
                }
        }
        return result;
}

UserAdsCacheUrlDataPtr UserAdsCacheManagerI::getUserAdsCacheUrl(::Ice::Int userId, const Ice::Current& current) {
	ostringstream msg;
	msg << "UserAdsCacheManagerI::getUserAdsCacheUrl id=" << userId << " "
	    << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(msg.str());

        return getUserAdsCacheUrlData(userId);
}

Int2UserAdsCacheUrlDataMap UserAdsCacheManagerI::getUserAdsCacheUrlList(const IntSeq& userIds, const Ice::Current& current) {
	ostringstream msg;
	msg << "UserAdsCacheManagerI::getUserAdsCacheUrlList -> size: " << userIds.size() << " "
	    << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(msg.str());

        ObjectResultPtr objres = getUserAdsCacheUrlData(userIds);
        Int2UserAdsCacheUrlDataMap result;
        for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
                ObjectMap::iterator hasRes = objres->data.find(*it);
                if (hasRes != objres->data.end()) {
                        result[*it]=UserAdsCacheUrlDataPtr::dynamicCast(hasRes->second);
			result[*it]->tinyurl = HeadUrlUtil::instance().getHeadFullUrlFromOuterSite(result[*it]->tinyurl);
                } else {
                        MCE_WARN("Not Exists: " << *it);
                }
        }
        return result;
}

bool UserAdsCacheManagerI::isValid(const Ice::Current& current) {
        IceUtil::RWRecMutex::RLock lock(_validMutex);
	map<string, string>::const_iterator isNew = current.ctx.find("isNewService");
	if (isNew != current.ctx.end()){
		return !_valid;
	}
        return _valid;
}

void UserAdsCacheManagerI::setValid(bool valid, const Ice::Current&) {
        IceUtil::RWRecMutex::WLock lock(_validMutex);
	MCE_INFO("UserAdsCacheManagerI::setValid -> "<<valid);
        _valid = valid;
	if (_valid){
		ServerStateSubscriber::instance().setStatus(1);
	}else{
		ServerStateSubscriber::instance().setStatus(0);
	}
}

void UserAdsCacheManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
        TaskManager::instance().execute(new FillTask(userData));
}

UserAdsCacheDataPtr UserAdsCacheManagerI::getUserAdsCacheData(int userId) {
	
	//MCE_DEBUG("[UserAdsCacheManagerI::getUserAdsCacheData : " << userId);
        UserAdsCacheUrlDataPtr obj = ServiceI::instance().locateObject<UserAdsCacheUrlDataPtr> (USER_ADS_CACHE_URL,userId);

        if ( obj == 0 ) {
                MCE_WARN("[UserAdsCacheUrlDataPtr::ptr] null " << "[" << userId << "]");
        }
	UserAdsCacheDataPtr objOld = toOld(obj);
        return objOld;
}

ObjectResultPtr UserAdsCacheManagerI::getUserAdsCacheData(
                const IntSeq& userIds) {

        MyUtil::LongSeq uids;
        for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it
                        != userIds.end(); ++it) {
                uids.push_back(*it);
        }
	ObjectResultPtr result = new ObjectResult;
	ObjectResultPtr obj = ServiceI::instance().getObjectCache()->locateObjects(USER_ADS_CACHE_URL, uids);
	for(MyUtil::ObjectMap::const_iterator it = obj->data.begin(); it
			!= obj->data.end(); ++it){
		result->data.insert(make_pair((*it).first, toOld(UserAdsCacheUrlDataPtr::dynamicCast((*it).second))));
	}
 //       return ServiceI::instance().getObjectCache()->locateObjects(USER_ADS_CACHE_URL, uids);
          return result;
}

UserAdsCacheUrlDataPtr UserAdsCacheManagerI::getUserAdsCacheUrlData(int userId) {
	//MCE_DEBUG("[UserAdsCacheManagerI::getUserAdsCacheUrlData : " << userId);
        UserAdsCacheUrlDataPtr obj = ServiceI::instance().locateObject<UserAdsCacheUrlDataPtr> (USER_ADS_CACHE_URL,userId);
	if (obj->tinyurl.find("http://") == string::npos){
		obj->tinyurl = HeadUrlUtil::instance().getHeadFullUrlFromOuterSite(obj->tinyurl);
	}
        if ( obj == 0 ) {
                MCE_WARN("[UserAdsCacheUrlDataPtr::ptr] null " << "[" << userId << "]");
        }
        return obj;
}

ObjectResultPtr UserAdsCacheManagerI::getUserAdsCacheUrlData(
                const IntSeq& userIds) {
        MyUtil::LongSeq uids;
        for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it
                        != userIds.end(); ++it) {
                uids.push_back(*it);
        }
        return ServiceI::instance().getObjectCache()->locateObjects(USER_ADS_CACHE_URL, uids);
}

UserAdsCacheDataPtr UserAdsCacheManagerI::toOld(const UserAdsCacheUrlDataPtr& obj){

	UserAdsCacheDataPtr objOld = new UserAdsCacheData;
	objOld->id = obj->id;
	objOld->stage = obj->stage;
	objOld->sex = obj->sex;
	objOld->birthyear = obj->birthyear;
	objOld->birthmonth = obj->birthmonth;
	objOld->birthday = obj->birthday;
	objOld->homeprovince = obj->homeprovince;
	objOld->homecity = obj->homecity;
	objOld->seatprovince = obj->seatprovince;
	objOld->seatcity = obj->seatcity;
	
	objOld->schools = obj->schools;
	objOld->workplaces = obj->workplaces;
	objOld->networks = obj->networks;
	return objOld;
}

FillTask::FillTask(const ObjectResultPtr& userData):_userData(userData) {
}

void FillTask::handle() {
        MCE_DEBUG("[FillTask::handle] set Data size : " << _userData->data.size());
	int cluster = 4;
	int mod = UserAdsCacheManagerI::instance().mod();
	MyUtil::ObjectResultPtr result=new MyUtil::ObjectResult();
	for (map<long, Ice::ObjectPtr>::const_iterator i = _userData->data.begin(); i != _userData->data.end(); ++i) {
		if (i->first % cluster == mod) {
			result->data.insert(*i);
		}
	}
	if (result->data.size() > 0) {
	        ServiceI::instance().getObjectCache()->addObjects(USER_ADS_CACHE_URL, result->data);
	}
}

Ice::ObjectPtr UserAdsCacheFactoryI::create(Ice::Long id) {
        UserAdsCacheUrlDataPtr result = new UserAdsCacheUrlData;
	MCE_INFO("UserAdsCacheLoaderAdapter::instance().load(id): " << id);
	UserAdsCacheLoaderAdapter::instance().load(id);
        return NULL;
}

MyUtil::ObjectResultPtr UserAdsCacheFactoryI::create(const LongSeq& ids) {
        MCE_DEBUG("Loading " << ids.size() << " objects");
        ObjectResultPtr results = new ObjectResult;
	vector<int> userids;
	for(LongSeq::const_iterator it=ids.begin();it !=ids.end(); ++it) {
		userids.push_back((int)(*it));
	}
	UserAdsCacheLoaderAdapter::instance().load(userids);
        return NULL;
}

