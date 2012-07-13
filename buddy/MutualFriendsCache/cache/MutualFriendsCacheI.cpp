#include <algorithm>
#include <set>
#include "MutualFriendsCacheI.h"
#include "ServiceI.h"
#include <MutualFriendsAdapter.h>
#include "BuddyByIdReplicaAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "util/cpp/InvokeMonitor.h"

//GlobalId.h没定义，在Adapter中也需要用到
#ifndef _GLOBALID_H_
#define _GLOBALID_H_
#include "UtilCxx/src/GlobalId.h"
#endif

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::serverstate;

//---------------------------------Service Init---------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(FANS_CACHE_DATA, "F", new FansCacheFactory);
	//	service.registerObjectCache(MUTUAL_CACHE_DATA, "M", new MutualFriendsCacheFactory);
	service.getAdapter()->add(&MutualFriendsCacheManagerI::instance(), service.createIdentity("MFCID", ""));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("MutualFriendsCache.Mod");
	string endpoint = service.getCommunicator()->getProperties()->getProperty("MutualFriendsCache.Endpoint");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MutualFriendsCache.Interval", 5);
	MutualFriendsCacheManagerI::instance().bootstrap(endpoint, mod);

	// 监视器初始化
	MyUtil::ServiceMonitorManager::instance().start();

	// 向Controller注册
	ServerStateSubscriber::instance().initialize("ControllerMutualFriendsCache", &MutualFriendsCacheManagerI::instance(), mod, interval);

	TaskManager::instance().config(TASK_LEVEL_FANS_CACHE_FILL, ThreadPoolConfig(1, 100));
//	TaskManager::instance().config(TASK_LEVEL_FANS_CACHE_LOAD, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(TASK_LEVEL_FANS_CACHE_RELOAD, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(TASK_LEVEL_FANS_CACHE_REMOVE, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(TASK_LEVEL_FANS_CACHE_ADD, ThreadPoolConfig(1, 100));

	service.registerXceObserver(new ServiceSchemaI);
}

//--------------------------MutualFriendsCacheManagerI--------------------------
void MutualFriendsCacheManagerI::bootstrap(const string& endpoint, int mod) {
	_endpoint = endpoint;
	_mod = mod;
	MCE_INFO( "[bootstrap] endpoint = " << _endpoint << " mod = " << _mod );
}

//从BuddyByIdCacheAdapter获取用户好友id列表
MyUtil::IntSeq MutualFriendsCacheManagerI::getFriendsAsc(int uid) {
	std::ostringstream oss;
	oss << "MutualFriendsCacheManagerI::getFriendsAsc(), uid : " << uid;
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);

	MyUtil::IntSeq userIdSeq;

	//互斥块1::begin
	//发布使用：正常调用BuddyByIdCache服务获取好友列表
	try {
		userIdSeq = BuddyByIdCacheAdapter::instance().getFriendListAsc(uid, 2000);
	} catch (...) {
		MCE_WARN("BuddyByIdCacheAdapter::instance().getFriendListAsc(" << uid << ", 2000) exception");
	}
	//互斥块1::end

//	//互斥块2::begin
//	//调试使用：没BuddyByIdCache服务环境的时候调试用
//	for(int i = 6; i < 200; i += 6){
//		userIdSeq.push_back(i);
//	}
//	//互斥块2::end

	return userIdSeq;
}

//从缓存中读取应用的粉丝列表，如果不存在，则用ReloadTask(globalId)从数据库加载，并返回空对象；如果过时，则重加载且返回旧值
MyUtil::IntSeq MutualFriendsCacheManagerI::getAllFansAsc(const MyUtil::GlobalIdPtr& gid) {
	Ice::Long globalId = GlobalIdTransfer::get(gid);

	std::ostringstream oss;
	oss << "MutualFriendsCacheManagerI::getAllFansAsc(), globalId : " << globalId;
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);

	FansDataIPtr fansData;
	try {
		fansData = ServiceI::instance().locateObject<FansDataIPtr> (FANS_CACHE_DATA, globalId);
		time_t now = time(0);
		if(now - fansData->getTimestamp() > 7200){
			TaskManager::instance().execute(new ReloadTask(globalId));
		}
	} catch (...) {
		MCE_WARN("ServiceI::instance().locateObject<FansDataIPtr> (FANS_CACHE_DATA, " << globalId << ") exception");
	}
	return fansData->get(0, -1);
}

//获取uid的好友用户中是gid的粉丝的那些用户
MyUtil::IntSeq MutualFriendsCacheManagerI::getMutualFriends(int uid, const MyUtil::GlobalIdPtr& gid, int begin, int limit, const Ice::Current& current) {
	ostringstream msg;
	msg << "uid = " << uid << ", gid = " << GlobalIdTransfer::get(gid) << ", begin = " << begin << ", limit = " << limit;
	InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	MyUtil::IntSeq fids = getAllFansAsc(gid);
	MyUtil::IntSeq uids = getFriendsAsc(uid);
	MyUtil::IntSeq result;

	if (limit < -1 || begin < 0 || uid < 0) {
		return result;
	} else {
		int count = 0;
		int countLimit = (limit == -1) ? uids.size() : limit;
		MyUtil::IntSeq::iterator fit = fids.begin();
		MyUtil::IntSeq::iterator uit = uids.begin();

		while (fit != fids.end() && uit != uids.end() && count < countLimit + begin) {
			if (*fit < *uit) {
				fit++;
			} else if (*uit < *fit) {
				uit++;
			} else {
				if(count >= begin){
					result.push_back(*fit);
				}
				count++;
				fit++;
				uit++;
			}
		}
		return result;
	}
}

//获取uid的好友用户中不是gid的粉丝的那些用户
MyUtil::IntSeq MutualFriendsCacheManagerI::getUnMutualFriends(int uid, const MyUtil::GlobalIdPtr& gid, int begin, int limit, const Ice::Current& current) {
	ostringstream msg;
	msg << "uid = " << uid << ", gid = " << GlobalIdTransfer::get(gid) << ", begin = " << begin << ", limit = " << limit;
	InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	MyUtil::IntSeq fids = getAllFansAsc(gid);
	MyUtil::IntSeq uids = getFriendsAsc(uid);
	MyUtil::IntSeq result;

	if (limit < -1 || begin < 0 || uid < 0) {
		return result;
	} else {
		int count = 0;
		int countLimit = (limit == -1) ? uids.size() : limit;
		MyUtil::IntSeq::iterator fit = fids.begin();
		MyUtil::IntSeq::iterator uit = uids.begin();

		while (fit != fids.end() && uit != uids.end() && count < countLimit + begin) {
			if (*fit < *uit) {
				fit++;
			} else if (*uit < *fit) {
				if(count >= begin){
					result.push_back(*uit);
				}
				count++;
				uit++;
			} else {
				fit++;
				uit++;
			}
		}
		return result;
	}
}

int MutualFriendsCacheManagerI::getMutualFriendsCount(int uid, const MyUtil::GlobalIdPtr& gid, const Ice::Current& current) {
	ostringstream msg;
	msg << "uid = " << uid << ", gid = " << GlobalIdTransfer::get(gid);
	InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return getMutualFriends(uid, gid, 0, -1, current).size();
}

int MutualFriendsCacheManagerI::getUnMutualFriendsCount(int uid, const MyUtil::GlobalIdPtr& gid, const Ice::Current& current) {
	ostringstream msg;
	msg << "uid = " << uid << ", gid = " << GlobalIdTransfer::get(gid);
	InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return getUnMutualFriends(uid, gid, 0, -1, current).size();
}

int MutualFriendsCacheManagerI::getFansCount(const MyUtil::GlobalIdPtr& gid, const Ice::Current& current) {
	ostringstream msg;
	msg << "gid = " << GlobalIdTransfer::get(gid);
	InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return getAllFansAsc(gid).size();
}

bool MutualFriendsCacheManagerI::isFans(int uid, const MyUtil::GlobalIdPtr& gid, const Ice::Current& current) {
	ostringstream msg;
	msg << "uid = " << uid << ", gid = " << GlobalIdTransfer::get(gid);
	InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	MyUtil::IntSeq fans = getAllFansAsc(gid);
	MyUtil::IntSeq::iterator it = lower_bound(fans.begin(), fans.end(), gid);
	return it != fans.end();
}

MyUtil::IntSeq MutualFriendsCacheManagerI::getFans(const MyUtil::GlobalIdPtr& gid, int begin, int limit, const Ice::Current& current) {
	ostringstream msg;
	msg << "gid = " << GlobalIdTransfer::get(gid) << ", begin = " << begin << ", limit = " << limit;
	InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	MyUtil::IntSeq fans = getAllFansAsc(gid);
	MyUtil::IntSeq result;
	if (limit < -1 || begin < 0) {
		return result;
	} else {
		MyUtil::IntSeq::iterator it = lower_bound(fans.begin(), fans.end(), gid);
		int countLimit = (limit == -1) ? fans.size() : limit;
		for (int count = 0; it != fans.end() && count < countLimit; it++) {
			result.push_back(*it);
			count++;
		}
		return result;
	}
}

void MutualFriendsCacheManagerI::add(int uid, const MyUtil::GlobalIdPtr& gid, const Ice::Current& current) {
	Ice::Long globalId = GlobalIdTransfer::get(gid);

	ostringstream msg;
	msg << "uid = " << uid << ", gid = " << globalId;
	InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	TaskManager::instance().execute(new AddTask(uid, globalId));
}

void MutualFriendsCacheManagerI::remove(int uid, const MyUtil::GlobalIdPtr& gid, const Ice::Current& current) {
	Ice::Long globalId = GlobalIdTransfer::get(gid);

	ostringstream msg;
	msg << "uid = " << uid << ", gid = " << globalId;
	InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	TaskManager::instance().execute(new RemoveTask(uid, globalId));
}

bool MutualFriendsCacheManagerI::isValid(const Ice::Current& current) {
	ostringstream oss;
	oss << "valid = " << _isValid;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	IceUtil::RWRecMutex::RLock lock(_validMutex);
	return _isValid;
}

void MutualFriendsCacheManagerI::setValid(bool valid, const Ice::Current& current) {
	ostringstream oss;
	oss << "valid = " << _isValid;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	IceUtil::RWRecMutex::WLock lock(_validMutex);
	_isValid = valid;
	if (valid) {
		ServerStateSubscriber::instance().setStatus(1);
	} else {
		ServerStateSubscriber::instance().setStatus(0);
	}
}

void MutualFriendsCacheManagerI::setData(const ::MyUtil::ObjectResultPtr& fansData, const Ice::Current& current) {
	ostringstream oss;
	oss << "size = " << fansData->data.size();
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	TaskManager::instance().execute(new FillTask(fansData));
}

//------------------------------FansCacheFactory--------------------------------
Ice::ObjectPtr FansCacheFactory::create(Ice::Long id) {
	TaskManager::instance().execute(new ReloadTask(id));
	return new FansDataI;
}

//-----------------------------------Task---------------------------------------
//写入一组应用的粉丝数据
void FillTask::handle() {
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
	for (std::map<long, Ice::ObjectPtr>::const_iterator it = _fansMap->data.begin(); it != _fansMap->data.end(); ++it) {
		try {
			FansDataIPtr obj = new FansDataI(FansObjectPtr::dynamicCast(it->second));
			res->data.insert(make_pair<long, Ice::ObjectPtr> (it->first, obj));
		} catch (Ice::Exception e) {
			MCE_WARN( "FillTask " << it->first << " found Ice::Exception : " << e.what() );
			continue;
		} catch (...) {
			MCE_WARN( "FillTask " << it->first << " found unknown exception ." );
			continue;
		}
	}
	ServiceI::instance().getObjectCache()->addObjects(FANS_CACHE_DATA, res->data);
	MCE_INFO("FillTask done. Task size / ObjectCache size : " << res->data.size() << " / " << ServiceI::instance().getObjectCache()->locateCategory(FANS_CACHE_DATA)->size());
}

//重新加载所有备份的数据
void ReloadTask::handle() {
	MutualFriendsLoaderAdapter::instance().reload(_gid);
}

void AddTask::handle() {
	FansDataIPtr res = ServiceI::instance().findObject<FansDataIPtr> (FANS_CACHE_DATA, _gid);
	if (!res) {
		return;
	}
	res->add(_uid);
}

void RemoveTask::handle() {
	FansDataIPtr res = ServiceI::instance().findObject<FansDataIPtr> (FANS_CACHE_DATA, _gid);
	if (!res) {
		return;
	}
	res->remove(_uid);
}

//---------------------------------Configure------------------------------------
void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	TaskManager::instance().config(props, "Fill", TASK_LEVEL_FANS_CACHE_FILL, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(props, "Reload", TASK_LEVEL_FANS_CACHE_RELOAD, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(props, "Remove", TASK_LEVEL_FANS_CACHE_REMOVE, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(props, "Add", TASK_LEVEL_FANS_CACHE_ADD, ThreadPoolConfig(1, 100));
}

