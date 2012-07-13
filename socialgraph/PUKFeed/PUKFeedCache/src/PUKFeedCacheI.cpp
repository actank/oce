#include "PUKFeedCacheI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
using namespace xce::socialgraph;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace xce::serverstate;
using namespace dbhelper;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.registerObjectCache(PUK_DATA, "PD", new PUKFeedFactory);
	
	service.getAdapter()->add( &PUKFeedCacheI::instance(),
			service.createIdentity("PFC", ""));

	ConnectionPoolManager::instance().initialize();

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("PUKFeedCache.mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("PUKFeedCache.Interval", 5);

	//向controller注册
	ServerStateSubscriber::instance().initialize("ControllerPUKFeedCache", &PUKFeedCacheI::instance(), mod, sInterval);
}

//-----------------------------------------------------------------------------

PUKFeedSeq PUKFeedData::Get(int limit) {
	MCE_INFO("[PUKFeedData] Get _ownerId(" << _ownerId << ") limit(" << limit << ")");
	IceUtil::Mutex::Lock lock(*this);

    if( limit >= (int)_feedList.size() || limit <= 0 ) {
        return _feedList;
    }   
	
	PUKFeedSeq result;
	for (PUKFeedSeq::iterator iter = _feedList.begin();
			iter != _feedList.end(); ++iter) {
		if (limit > 0) {
			result.push_back(*iter);
			--limit;
		}
	}
	return result;
}

void PUKFeedData::Remove(const PUKFeedSeq& feedList) {
	MCE_INFO("[PUKFeedData] Remove feedList size(" << feedList.size() << ")");
	IceUtil::Mutex::Lock lock(*this);
	
	for (PUKFeedSeq::const_iterator iter = feedList.begin();
			iter != feedList.end(); ++iter) {
		for (PUKFeedSeq::iterator it = _feedList.begin();
				it != _feedList.end(); ++it) {
			if (iter->friendId == it->friendId && iter->commonId == it->commonId) {
				_feedList.erase(it);
				break;
			}	
		}
	}
}

//-----------------------------------------------------------------------------

void PUKFeedCacheI::Set(::Ice::Int hostId, const PUKFeedSeq& feedList, const Ice::Current& current) {
	MCE_INFO("[PUKFeedCacheI] Set hostId(" << hostId << ") feedList size(" << feedList.size() << ")");
	PUKFeedDataPtr obj = new PUKFeedData(hostId, feedList);
	ServiceI::instance().addObject(obj, PUK_DATA, hostId);
}

PUKFeedSeq PUKFeedCacheI::Get(::Ice::Int hostId, ::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("[PUKFeedCacheI] Get hostId(" << hostId << ") limit(" << limit << ")");
	return FindPUKFeedData(hostId)->Get(limit);
}

PUKFeedSeq PUKFeedCacheI::GetAndRemove(::Ice::Int hostId, ::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("[PUKFeedCacheI] GetAndRemove hostId(" << hostId << ") limit(" << limit << ")");
	PUKFeedSeq result = FindPUKFeedData(hostId)->Get(limit);

	TaskManager::instance().execute(new RemoveTask(hostId, result));

	return result;
}

void PUKFeedCacheI::Remove(::Ice::Int hostId, const PUKFeedSeq& feedList, const Ice::Current& current) {
	MCE_INFO("[PUKFeedCacheI] Remove hostId(" << hostId << ") feedList size(" << feedList.size() << ")");
	TaskManager::instance().execute(new RemoveTask(hostId, feedList));

}

PUKFeedDataPtr PUKFeedCacheI::FindPUKFeedData(::Ice::Int id) {
	PUKFeedDataPtr obj = ServiceI::instance().findObject<PUKFeedDataPtr>(PUK_DATA, id);
	if (!obj) {
		return new PUKFeedData(id, PUKFeedSeq());
	}
	return obj;
}

//-----------------------------------------------------------------------------

void RemoveTask::handle() {
	MCE_INFO("[RemoveTask] userId(" << _userId << ") _feedList size(" << _feedList.size() << ")");
	PUKFeedDataPtr obj = ServiceI::instance().findObject<PUKFeedDataPtr>(PUK_DATA, _userId);

	if (!obj) {
		return;
	}
	obj->Remove(_feedList);
	
	PUKFeedDBHelper::RemovePUKFeed(_userId, _feedList);
}
