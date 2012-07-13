#include "NegtiveListCacheI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::socialgraph;
using namespace MyUtil;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	
	service.registerObjectCache(NEGTIVELIST_DATA, "ND", new NegtiveListDataFactory);

	service.getAdapter()->add(&NegtiveListCacheI::instance(),
			service.createIdentity("NLC", ""));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("NegtiveListCache.Mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("NegtiveListCache.Interval", 5);

	//向controller注册
	ServerStateSubscriber::instance().initialize("ControllerNegtiveListCache", &NegtiveListCacheI::instance(), mod, sInterval);
}

//********************************************************************************

MyUtil::IntSeq NegtiveListData::Get(int limit) {
	MCE_INFO("[NegtiveListData] Get _hostId(" << _hostId<< ") limit(" << limit << ")");

	if (limit < 0 || limit >= (int)_negtiveList.size()) {
		return _negtiveList;
	}	

	IntSeq result;
	for (IntSeq::iterator iter = _negtiveList.begin();
			iter != _negtiveList.end(); ++iter) {
		if (limit > 0) {
			result.push_back(*iter);
			--limit;
		}
	}
	return result;
}

void NegtiveListData::Remove(const MyUtil::IntSeq& negtiveList) {
	MCE_INFO("[NegtiveListData] Remove _hostId(" << _hostId << ") negtiveList size(" << negtiveList.size() << ")");
	for (IntSeq::const_iterator iter = negtiveList.begin();
			iter != negtiveList.end(); ++iter) {
		IntSeq::iterator fIt = find(_negtiveList.begin(), _negtiveList.end(), *iter);
		if (fIt != _negtiveList.end()) {
			_negtiveList.erase(fIt);
		}
	}
}

//********************************************************************************

void NegtiveListCacheI::Set(::Ice::Int hostId, const MyUtil::IntSeq& negtiveList, const Ice::Current& current) {
	MCE_INFO("[NegtiveListCacheI] Set hostId(" << hostId << ") negtiveList size(" << negtiveList.size() << ")");
	NegtiveListDataPtr obj = new NegtiveListData(hostId, negtiveList);
	ServiceI::instance().addObject(obj, NEGTIVELIST_DATA, hostId);
}

MyUtil::IntSeq NegtiveListCacheI::Get(::Ice::Int hostId, ::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("[NegtiveListCacheI] Get hostId(" << hostId << ") limit(" << limit << ")");
	return FindNegtiveListData(hostId)->Get(limit);
}

void NegtiveListCacheI::Remove(::Ice::Int hostId, const MyUtil::IntSeq& negtiveList, const Ice::Current& current) {
	MCE_INFO("[NegtiveListCacheI] Remove hostId(" << hostId << ") negtiveList size(" << negtiveList.size() << ")");
	TaskManager::instance().execute(new RemoveTask(hostId, negtiveList));
}

NegtiveListDataPtr NegtiveListCacheI::FindNegtiveListData(::Ice::Int id) {
	NegtiveListDataPtr obj = ServiceI::instance().findObject<NegtiveListDataPtr>(NEGTIVELIST_DATA, id);
	if (obj) {
		return obj;
	}
	return new NegtiveListData(id, IntSeq());
}

//********************************************************************************

void RemoveTask::handle() {
	MCE_INFO("[RemoveTask] _ownerId(" << _ownerId << ") _negtiveList size(" << _negtiveList.size() << ")");
	NegtiveListDataPtr obj = ServiceI::instance().findObject<NegtiveListDataPtr>(NEGTIVELIST_DATA, _ownerId);
	if (obj) {
		if (_negtiveList.empty()) {      		//列表为空时，清空ObjectCache
			ServiceI::instance().removeObject(NEGTIVELIST_DATA, _ownerId);
		} else {
			obj->Remove(_negtiveList);
		}
	}
}
