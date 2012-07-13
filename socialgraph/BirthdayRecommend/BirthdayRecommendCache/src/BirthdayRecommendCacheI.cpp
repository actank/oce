#include "BirthdayRecommendCacheI.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::socialgraph;
using namespace xce::serverstate;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace xce::helper;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.registerObjectCache(BIRTH_RECOMMEND_DATA, "BRD", new BirthdayRecommendFactory);

	service.getAdapter()->add(&BirthdayRecommendCacheI::instance(),
		 service.createIdentity("BRC", ""));

	ConnectionPoolManager::instance().initialize();

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BirthdayRecommendCache.Mod");	
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BirthdayRecommendCache.Interval", 5);
	
	//向controller注册
	ServerStateSubscriber::instance().initialize("ControllerBirthdayRecommendCache", &BirthdayRecommendCacheI::instance(), mod, sInterval);
}

//****************************************************************************************

MyUtil::IntSeq BirthdayRecommendData::GetBirthdayRecommendList(int limit) {
	IceUtil::Mutex::Lock lock(*this);
	MCE_INFO("[BirthdayRecommendData] GetBirthdayRecommendList hostId=" << _hostId <<" limit = " << limit);

	if (!CheckTimeout()) {
		return IntSeq();
	}

	if (limit <= 0 || (int)_birthdayRecommendList.size() <= limit) {
		return _birthdayRecommendList;
	}

	MyUtil::IntSeq result;
	for (IntSeq::iterator iter = _birthdayRecommendList.begin();
			iter != _birthdayRecommendList.end(); ++iter) {
		if (limit <= 0) {
			break;
		}
		result.push_back(*iter);
		--limit;
	}

	return result;
}

int BirthdayRecommendData::GetBirthdayRecommend() {
	IceUtil::Mutex::Lock lock(*this);
	MCE_INFO("[BirthdayRecommendData] GetBirthdayRecommend hostId=" << _hostId);

	if (!CheckTimeout()) {
		return 0;
	}

	if (!_birthdayRecommendList.empty()) {
		return _birthdayRecommendList.at(0);
	}
	return 0;
}

void BirthdayRecommendData::RemoveBirthdayRecommend(int friendId) {
	IceUtil::Mutex::Lock lock(*this);
	MCE_INFO("[BirthdayRecommendData] RemoveBirthdayRecommend hostId=" << _hostId << " friendId = " << friendId);

	for (IntSeq::iterator iter = _birthdayRecommendList.begin();
			iter != _birthdayRecommendList.end(); ++iter) {
		if (*iter == friendId) {
			_birthdayRecommendList.erase(iter);
			break;
		}	
	}
}

bool BirthdayRecommendData::CheckTimeout() {
	if (time(NULL) - _timestamp > INTERVAL) {
		MCE_INFO("[BirthdayRecommendData] CheckTimeout hostId=" << _hostId << " timeout");
		return false;	
	}
	return true;
}

//****************************************************************************************

void BirthdayRecommendCacheI::Set(::Ice::Int hostId, const MyUtil::IntSeq& birthdayRecommendList, const Ice::Current& current) {
	MCE_INFO("[BirthdayRecommendCacheI] Set hostId(" << hostId << ") birthdayRecommendList(" << birthdayRecommendList.size() << ")");
	BirthdayRecommendDataPtr obj = new BirthdayRecommendData( hostId, birthdayRecommendList );
	ServiceI::instance().addObject( obj, BIRTH_RECOMMEND_DATA, hostId );
}

::Ice::Int BirthdayRecommendCacheI::Get(::Ice::Int hostId, const Ice::Current& current) {
	MCE_INFO("[BirthdayRecommendCacheI] Get hostId(" << hostId << ")");
	int friendId = FindBirthdayRecommendData(hostId)->GetBirthdayRecommend();
	if (friendId == 0) {
		ServiceI::instance().removeObject( BIRTH_RECOMMEND_DATA, hostId );	
	}
	return friendId;
}

::Ice::Int BirthdayRecommendCacheI::GetAndRemove(::Ice::Int hostId, const Ice::Current& current) {
	int friendId = FindBirthdayRecommendData(hostId)->GetBirthdayRecommend();

	MCE_INFO("[BirthdayRecommendCacheI] GetAndRemove hostId(" << hostId << ") friendId(" << friendId << ")");
	TaskManager::instance().execute( new RemoveTask(hostId, friendId) );
	if (friendId == 0) {
		ServiceI::instance().removeObject( BIRTH_RECOMMEND_DATA, hostId );	
	}

	return friendId;
}

MyUtil::IntSeq BirthdayRecommendCacheI::GetList(::Ice::Int hostId, ::Ice::Int limit, const Ice::Current& current) {
	MCE_INFO("[BirthdayRecommendCacheI] GetList hostId(" << hostId << ")");
	IntSeq result = FindBirthdayRecommendData(hostId)->GetBirthdayRecommendList(limit);
	if (result.empty()) {
		ServiceI::instance().removeObject( BIRTH_RECOMMEND_DATA, hostId );	
	}
	return result;
}

void BirthdayRecommendCacheI::Remove(::Ice::Int hostId, ::Ice::Int friendId, const Ice::Current& current) {
	MCE_INFO("[BirthdayRecommendCacheI] Remove hostId(" << hostId << ") friendId(" << friendId << ")");
	TaskManager::instance().execute( new RemoveTask(hostId, friendId) );
}

BirthdayRecommendDataPtr BirthdayRecommendCacheI::FindBirthdayRecommendData(::Ice::Int id) {
	BirthdayRecommendDataPtr obj = ServiceI::instance().findObject<BirthdayRecommendDataPtr>(BIRTH_RECOMMEND_DATA, id);
	if (!obj) {
		return new BirthdayRecommendData( id, IntSeq() );
	}
	return obj;
}

//****************************************************************************************

Ice::ObjectPtr BirthdayRecommendFactory::create(::Ice::Int id) {
	MCE_INFO("[BirthdayRecommendFactory] create id(" << id << ")");
	BirthdayRecommendDataPtr obj = new BirthdayRecommendData( id, IntSeq() );
	return obj;
}

//****************************************************************************************

void RemoveTask::handle() {
	MCE_INFO("[RemoveTask] _ownerId(" << _ownerId << ") _friendId(" << _friendId << ")");
	BirthdayRecommendDataPtr obj = ServiceI::instance().findObject<BirthdayRecommendDataPtr>( BIRTH_RECOMMEND_DATA, _ownerId );
	if (obj) {
		obj->RemoveBirthdayRecommend(_friendId);
	}
	
	RecommendDBHelper::RemoveRecommend(_ownerId, _friendId);
}
