#include "HotFeedRecommendCacheI.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::socialgraph;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace xce::serverstate;
using namespace xce::dbhelper;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.registerObjectCache(HOT_FEED_DATA, "HFD", new HotFeedFactory);

	service.getAdapter()->add(&HotFeedRecommendCacheI::instance(),
			service.createIdentity("HFRC", ""));

	ConnectionPoolManager::instance().initialize();

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("HotFeedRecommendCache.Mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("HotFeedRecommendCache.Interval", 5);

	//向controller注册
	ServerStateSubscriber::instance().initialize("ControllerHotFeedRecommendCache", &HotFeedRecommendCacheI::instance(), mod, sInterval);
}

//*******************************************************************************

HotFeedSeq HotFeedData::Get(::Ice::Int ownerId, int limit) {
	MCE_INFO("[HotFeedData] Get ownerId(" << ownerId << ") limit(" << limit << ")");
	IceUtil::Mutex::Lock lock(*this);

	if (!CheckTimeout()) {
		return HotFeedSeq();
	}

	if (limit < 0 || (int)_hotFeedList.size() <= limit) {
		return _hotFeedList;	
	}

	HotFeedSeq result;
	for (HotFeedSeq::iterator iter = _hotFeedList.begin();
			iter != _hotFeedList.end(); ++iter) {
		if (limit > 0) {
			result.push_back(*iter);
			--limit;
		}
	}

	return result;
}

void HotFeedData::Remove(int ownerId, const HotFeedSeq& hotFeedList) {
	MCE_INFO("[HotFeedData] Remove ownerId(" << ownerId << ") hotFeedList size(" << hotFeedList.size() << ")");
	IceUtil::Mutex::Lock lock(*this);

	for (HotFeedSeq::const_iterator iter = hotFeedList.begin();
			iter != hotFeedList.end(); ++iter) {
		for (HotFeedSeq::iterator it = _hotFeedList.begin();
				it != _hotFeedList.end(); ++it) {
			if (iter->userId == it->userId && iter->feedId == it->feedId) {
				_hotFeedList.erase(it);
				break;
			}
		}
	}
}

bool HotFeedData::CheckTimeout() {
	MCE_INFO("INTERVAL = " << INTERVAL << " : _timestamp = " << _timestamp);
	if (time(NULL) - _timestamp > INTERVAL) {
        MCE_INFO("[HotFeedData] CheckTimeout ownerId=" << _ownerId << " timeout");
        return false;   
    }
    return true;
}

//*******************************************************************************

void HotFeedRecommendCacheI::Set(::Ice::Int hostId, const HotFeedSeq& hotFeedList, const Ice::Current& current) {
	MCE_INFO("[HotFeedRecommendCacheI] Set hostId(" << hostId << ") hotFeedList size(" << hotFeedList.size() << ")");
	HotFeedDataPtr obj = new HotFeedData(hostId, hotFeedList);
	
	ServiceI::instance().addObject(obj, HOT_FEED_DATA, hostId);
}

HotFeedSeq HotFeedRecommendCacheI::GetAndRemove(::Ice::Int hostId, ::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("[HotFeedRecommendCacheI] GetAndRemove hostId(" << hostId << ") limit(" << limit << ")");
	HotFeedSeq result = FindHotFeedData(hostId)->Get(hostId, limit);
	
	TaskManager::instance().execute(new RemoveTask(hostId, result));
	if (result.empty()) {
		ServiceI::instance().removeObject(HOT_FEED_DATA, hostId);
	}

	return result;
}

HotFeedSeq HotFeedRecommendCacheI::Get(::Ice::Int hostId, ::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("[HotFeedRecommendCacheI] Get hostId(" << hostId << ") limit(" << limit << ")");
	HotFeedSeq result = FindHotFeedData(hostId)->Get(hostId, limit);

	if (result.empty()) {
		ServiceI::instance().removeObject(HOT_FEED_DATA, hostId);
	}

	return result;
}

void HotFeedRecommendCacheI::Remove(::Ice::Int hostId, const HotFeedSeq& hotFeedList, const Ice::Current& current) {
	MCE_INFO("[HotFeedRecommendCacheI] Remove hostId(" << hostId << ") hotFeedList size(" << hotFeedList.size() << ")");
	TaskManager::instance().execute(new RemoveTask(hostId, hotFeedList));
}

HotFeedDataPtr HotFeedRecommendCacheI::FindHotFeedData(::Ice::Int id) {
	HotFeedDataPtr obj = ServiceI::instance().findObject<HotFeedDataPtr>(HOT_FEED_DATA, id);
	if (!obj) {
		return new HotFeedData(id, HotFeedSeq());
	}

	return obj;
}

//*******************************************************************************

void RemoveTask::handle() {
	MCE_INFO("[RemoveTask] _ownerId(" << _ownerId << ") _hotFeedList size(" << _hotFeedList.size() << ")");
	HotFeedDataPtr obj = ServiceI:: instance().findObject<HotFeedDataPtr>(HOT_FEED_DATA, _ownerId);

	if (obj) {
		obj->Remove(_ownerId, _hotFeedList);
	}

	HotFeedDBHelper::RemoveHotFeed(_ownerId, _hotFeedList);	
}
