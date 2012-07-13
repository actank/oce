#include <vector>
#include <map>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include "DistUserCacheReloaderI.h"
#include "DistUserCacheAdapter.h"
#include "OnlineBitmapAdapter.h"
#include "WapOnlineAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include <QueryRunner.h>
#include "OnlineEventMediator.h"
#include "StatusCacheAdapter.h"

using namespace xce::statuscache;
using xce::mediator::onlineevent::OFFLINE;
using xce::mediator::onlineevent::ONLINEWEBPAGGER;
using xce::mediator::onlineevent::ONLINEXNT;
using xce::mediator::onlineevent::ONLINEWAP;
using xce::mediator::onlineevent::OFFLINEWAP;
using namespace com::xiaonei::wap::adapter;
using namespace com::xiaonei::xce;
using xce::distcache::usercacheadapter::UserCacheMemDataPtr;
using xce::adapter::userbase::UserBaseFullViewPtr;
using xce::adapter::userbase::UserBaseAdapter;
using talk::onlinebitmap::OnlineBitmapAdapter;
using talk::online::UserOnlineTypeSeq;
using xce::serverstate::ServerStateSubscriber;
using namespace xce::distcache::usercacheadapter;
using namespace MyUtil;
using namespace xce::usercache;
using namespace xce::distcache::usercache;
using namespace com::xiaonei::xce;
using namespace mysqlpp;
using namespace std;

void MyUtil::initialize() {
	DistUserCacheAdapter::instance().initialize();
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&xce::distcache::usercache::DistUserCacheReloaderI::instance(), service.createIdentity(xce::distcache::usercache::DIST_USER_CACHE_RELOADER, ""));
	TaskManager::instance().config(xce::distcache::usercache::TASK_LEVEL_RELOAD, MyUtil::ThreadPoolConfig(1, 10));
	TaskManager::instance().config(xce::distcache::usercache::TASK_LEVEL_UPDATE, MyUtil::ThreadPoolConfig(1, 10));
	TaskManager::instance().config(xce::distcache::usercache::TASK_LEVEL_INSERT, MyUtil::ThreadPoolConfig(1, 5));
	TaskManager::instance().config(TASK_LEVEL_UPDATEONLINEUSERSTATE, MyUtil::ThreadPoolConfig(1, 5));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("DistUserCacheReloader.Mod", 0);
    int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("DistUserCacheReloader.Interval", 5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerDistUserCacheReloader", &xce::distcache::usercache::DistUserCacheReloaderI::instance(), mod, interval);
	DistUserCacheReloaderI::instance().startTask();
	DistUserCacheReloaderI::instance().setValid(true);
}

UpdateOnlineTask::UpdateOnlineTask(OnlineTypeSeq onlineSeq) : MyUtil::Task(TASK_LEVEL_UPDATEONLINEUSERSTATE), onlineSeq_(onlineSeq) {
}

void UpdateOnlineTask::handle() {
	vector<int> reloadIds;
	ostringstream os;
	os << "UpdateOnlineTask::handle size = " << onlineSeq_.size() ;

	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_DEBUG);
	vector<long> ids;
	multimap<int, int> userId2Online;
	for(OnlineTypeSeq::const_iterator it = onlineSeq_.begin(); it != onlineSeq_.end(); ++it) {
		ids.push_back((*it)->userId);
		userId2Online.insert(make_pair((*it)->userId, (*it)->online));
	}
	map<long, UserCacheMemDataPtr> res = DistUserCacheAdapter::instance().getDataWithCAS(ids);
	for(map<long, UserCacheMemDataPtr>::const_iterator it = res.begin(); it != res.end(); ++it) {
		UserCacheMemDataPtr data = it->second;
		if(data) {
			int online_cache = data->online();
			for(multimap<int, int>::const_iterator itr = userId2Online.lower_bound(data->id()); itr != userId2Online.upper_bound(data->id()); ++itr) {
				int online = itr->second;
				if(OFFLINE == online) {
					online_cache &= (~(1 << (ONLINEWEBPAGGER - 1)));
					online_cache &= (~(1 << (ONLINEXNT - 1)));
				} else if(online > 0) {
					online_cache |= (1 << (online - 1));
				} else if(online < 0) {
					online_cache &= (~(1 << (std::abs(online) - 1)));
				} else {
					MCE_WARN("UpdateOnlineTask::handle userid = " << data->id() << " online is wrong, online=" << online);
					continue;
				}
			}
			data->set_online(online_cache);
		}
	}
	vector<long> wrongIds = DistUserCacheAdapter::instance().setData(res);
	if(wrongIds.size() > 0) {
		reloadIds.insert(reloadIds.end(), wrongIds.begin(), wrongIds.end());
	}
	if(reloadIds.size() > 0) {
		ostringstream os1;
		os1 << "UpdateOnlineTask::handle setData fail id ";
		for(vector<int>::iterator it = reloadIds.begin(); it != reloadIds.end(); ++it) {
			os1 << *it << " , ";
		}
		MCE_WARN(os.str());
		MyUtil::TaskManager::instance().execute(new ReloadTask(reloadIds, Ice::Current()));
	}
}

void UpdateOnlineThread::add(OnlineTypePtr data) {
	ostringstream os;
	os << "UpdateOnlineThread::add userid = " << data->userId << " online = " << data->online;
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_DEBUG);
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	bool needsNotify = onlineSeq_.empty();
	onlineSeq_.push_back(data);
	if(needsNotify) {
		mutex_.notify();
	}
}
void UpdateOnlineThread::add(OnlineTypeSeq data) {
	ostringstream os;
	os << "UpdateOnlineThread::add size = " << data.size();
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_DEBUG);
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	bool needsNotify = onlineSeq_.empty();
	onlineSeq_.insert(onlineSeq_.end(), data.begin(), data.end());
	if (needsNotify) {
		mutex_.notify();
	}
}

void UpdateOnlineThread::run() {
	while (true) {
		ostringstream os;
		os << "UpdateOnlineThread::run";
		TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_DEBUG);
		try {
				OnlineTypeSeq batch;
				{
					IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
					if(onlineSeq_.empty()) {
						mutex_.wait();
					}
					batch.swap(onlineSeq_);
				}
				MyUtil::TaskManager::instance().execute(new UpdateOnlineTask(batch));
				sleep(1);
		} catch (...) {
			MCE_WARN("UpdateOnlineThread::run  main loop exception");
		}
	}
}

void MaintainOnlineThread::add(int userId, int online) {
	ostringstream os;
	os << "MaintainOnlineThread::add userid = " << userId << " online = " << online;
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_DEBUG);

	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	bool needsNotify = onlineMap_.empty();
	onlineMap_.insert(make_pair(userId, online));
	if (needsNotify) {
		mutex_.notify();
	}
}

void MaintainOnlineThread::add(const MyUtil::Int2IntMap& onlineMap) {
	ostringstream os;
	os << "UpdateOnlineThread::add size = " << onlineMap.size();
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_DEBUG);

	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	bool needsNotify = onlineMap_.empty();
	onlineMap_.insert(onlineMap.begin(), onlineMap.end());
	if (needsNotify) {
		mutex_.notify();
	}
}

void MaintainOnlineThread::getOnlineInfo(const MyUtil::IntSeq& userIds, MyUtil::Int2IntMap& onlineUserMap,
														map<long, UserCacheMemDataPtr>& onlineUserData,
														map<long, UserCacheMemDataPtr>& offlineUserData) {
	ostringstream os;
	os << "MaintainOnlineThread::getOnlineInfo userIds size = " << userIds.size();
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_DEBUG);

	try {
	
		UserOnlineTypeSeq temData = OnlineBitmapAdapter::instance().getOnlineUsers( userIds );
		map<int, int> res;
		for( UserOnlineTypeSeq::const_iterator it = temData.begin(); it != temData.end(); ++it )
		{
			res.insert( make_pair( (*it)->userId, (*it)->onlineType ) );
		}
		
		for(map<long, UserCacheMemDataPtr>::iterator it = onlineUserData.begin(); it != onlineUserData.end(); ++it) {
			map<int,int>::const_iterator pIt = res.find(it->first);
			if(pIt == res.end()) {
				it->second->set_online(0);
				MCE_DEBUG("MaintainOnlineThread::getOnlineInfo userid = " << it->first << " not in OnlineBitmapAdapter");
			} else {
				int online_cache = 0;
				int online = 0;
				online = (pIt->second & WEBPAGER_ONLINE) ? ONLINEWEBPAGGER : OFFLINE;
				if (online > 0) {
					online_cache |= (1 << (online - 1));
				}
				online = (pIt->second & TALK_ONLINE) ? ONLINEXNT: OFFLINE;
				if(online > 0) {
					online_cache |= (1 << (online - 1));
				}
				(it->second)->set_online(online_cache);
			}
		}

		map<int, int> wap_res = WapOnlineAdapter::instance().getOnlineStatus(userIds);
		for(map<long, UserCacheMemDataPtr>::iterator itr = onlineUserData.begin(); itr != onlineUserData.end(); ++itr) {
			map<int, int>::iterator wap_it = wap_res.find(itr->first);
			if(wap_it == wap_res.end()) {
				MCE_DEBUG("MaintainOnlineThread::getOnlineInfo userid = " << itr->first << " not in WapOnlineAdapter");
			} else {
				int online = 0;
				int online_cache = itr->second->online();
				online = (wap_it->second & WAP_ONLINE) ? ONLINEWAP : OFFLINEWAP;
				if (online > 0) {
					online_cache |= (1 << (online - 1));
				}
				(itr->second)->set_online(online_cache);
			}
		}

		for(std::map<long, UserCacheMemDataPtr>::iterator it = onlineUserData.begin(); it != onlineUserData.end(); ++it) {
			int online_cache = (it->second)->online();
			if(online_cache > 0) {
				onlineUserMap.insert(make_pair(it->first , online_cache));
			} else {
				offlineUserData.insert(*it);
			}
		}
		MCE_INFO("MaintainOnlineThread::getOnlineInfo onlineUserMap size = "<< onlineUserMap.size() <<" offlineUserData size = " << offlineUserData.size());
	} catch(const Ice::Exception& e) {
		MCE_WARN("MaintainOnlineThread::getOnlineInfo Exception: " << e);
	} catch(const exception& e) {
		MCE_WARN("MaintainOnlineThread::getOnlineInfo Exception: " << e.what());
	} catch(...) {
		MCE_WARN("MaintainOnlineThread::getOnlineInfo unknown Exception");
	}
}

void MaintainOnlineThread::fresh(const MyUtil::Int2IntMap& onlineMap) {
	ostringstream os;
	os << "MaintainOnlineThread::fresh size = " << onlineMap.size();
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_INFO);
	vector<int> ids;
	for(Int2IntMap::const_iterator it = onlineMap.begin(); it != onlineMap.end(); ++it) {
		if(it->first > 0) {
			ids.push_back(it->first);
		}
	}
	int length = ids.size();
	int i = 0;
	vector<int>::iterator it = ids.begin();
	int batch = 100;
	vector<long> allWrongIds;

	while(it != ids.end()) {
		int j = i + batch <= length ? batch : length - i;
		vector<int>  ids_tmp(it, it + j);
		vector<long> ids_long(ids_tmp.begin(), ids_tmp.end());
		vector<long> wrongIds;
		MyUtil::Int2IntMap onlineMap;		
		map<long, UserCacheMemDataPtr> userCacheData = DistUserCacheAdapter::instance().getDataWithCAS(ids_long);
		map<long, UserCacheMemDataPtr> offlineUserCacheData;
		getOnlineInfo(ids_tmp, onlineMap, userCacheData, offlineUserCacheData);
		wrongIds = DistUserCacheAdapter::instance().setData(offlineUserCacheData);
		if(wrongIds.size() > 0) {
			copy(wrongIds.begin(), wrongIds.end(), allWrongIds.end());
		}
		add(onlineMap);
		i += j;
		it += j;
	}				
	if(allWrongIds.size() > 0) {
		vector<int> intIds(allWrongIds.begin(), allWrongIds.end());
		ostringstream os;
		os << "MaintainOnlineThread::fresh Exception id ";
		for(vector<long>::const_iterator wIt = allWrongIds.begin(); wIt != allWrongIds.end(); ++wIt) {
			os << *wIt << " , ";
		}
		MCE_WARN(os.str());
		MyUtil::TaskManager::instance().execute(new ReloadTask(intIds, Ice::Current()));
	}
}

void MaintainOnlineThread::run() {
	init();
	while(true) {
		struct timeval tv;
		memset(&tv, 0, sizeof(tv));
		gettimeofday(&tv, NULL);
		long begin = tv.tv_sec * 1000 + tv.tv_usec / 1000;
		
		try {
			MyUtil::Int2IntMap batch;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
				if(onlineMap_.empty()) {
					mutex_.wait();
				}
				batch.swap(onlineMap_);
			}
			{
				ostringstream os;
				os << "MaintainOnlineThread::run size = " << batch.size();
				TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_INFO);
				fresh(batch);
			}

			memset(&tv, 0, sizeof(tv));
			gettimeofday(&tv, NULL);
			long end = tv.tv_sec * 1000 + tv.tv_usec / 1000;

			if(end - begin > 360000) {
				MCE_WARN("MaintainOnlineThread::run more than 6 minutes");
			} else {
				sleep((360000 - (end - begin)) / 1000);
			}
		} catch (const Ice::Exception& e) {
			MCE_WARN("MaintainOnlineThread::run Exception: " << e);
		} catch (const exception& e) {
			MCE_WARN("MaintainOnlineThread::run Exception: " << e.what());
		} catch (...) {
			MCE_WARN("MaintainOnlineThread::run unknown Exception");
		}
	}
}

void MaintainOnlineThread::init() {
	ostringstream os;
	os << "MaintainOnlineThread::init ";
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_INFO);
	try {
		static bitset<STATUS_CACHE_SIZE> idsStatus;
		StatusCacheAdapter::instance().getAllStatus(idsStatus);
		vector<int> userIds(3000);
		int count = 0;
		for(int i = 0; i < STATUS_CACHE_SIZE; ++i) {
			if(idsStatus[i] == 0) {
				userIds[count++] = i;
				if(count % 3000 == 0) {
					add(userIds);
					count = 0;
				}
			}
		}
		if(count != 0) {
			vector<int> tempIds(userIds.begin(), userIds.begin() + count);
			add(tempIds);
		}
	} catch(const Ice::Exception& e) {
		MCE_WARN("MaintainOnlineThread::init Exception: " << e);
	} catch(const exception& e) {
		MCE_WARN("MaintainOnlineThread::init Exception: " << e.what());
	} catch (...) {
		MCE_WARN("MaintainOnlineThread::init unknown Exception");
	}
}

void MaintainOnlineThread::add(const vector<int>& userIds) {
	ostringstream os;
	os << "MaintainOnlineThread::add size = " << userIds.size();
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_DEBUG);
	vector<long> userIdsLong(userIds.begin(), userIds.end());
	map<long, UserCacheMemDataPtr> userCacheData = DistUserCacheAdapter::instance().getDataWithCAS(userIdsLong);
	map<int, int> tempMap;
	for(map<long, UserCacheMemDataPtr>::iterator it = userCacheData.begin(); it != userCacheData.end(); ++it) {
		MCE_DEBUG("[MaintainOnlineThread::add] id = " << it->first << " added");
		int online_cache = (it->second)->online();
		if(online_cache > 0) {
			int id =(it->second)->id();
			tempMap.insert(make_pair(id, online_cache));
		}
	}
	if(tempMap.size() > 0) {
		add(tempMap);
	}
}

DistUserCacheReloaderI::DistUserCacheReloaderI() {
	maintainOnlineThread_ = new MaintainOnlineThread;
	updateOnlineThread_ = new UpdateOnlineThread;
}

void DistUserCacheReloaderI::startTask() {
	maintainOnlineThread_->start(1024 * 1024).detach();
	updateOnlineThread_->start(1024 * 1024).detach();
}

bool DistUserCacheReloaderI::isValid(const Ice::Current&) {
	IceUtil::RWRecMutex::RLock lock(validmutex_);
	return valid_;
}

void DistUserCacheReloaderI::setValid(bool valid, const Ice::Current&) {
	ostringstream oss;
	oss << "DistUserCacheReloaderI::setValid " << valid;
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	IceUtil::RWRecMutex::WLock lock(validmutex_);
	valid_ = valid;
	if(valid_) {
		xce::serverstate::ServerStateSubscriber::instance().setStatus(1);
	} else {
		xce::serverstate::ServerStateSubscriber::instance().setStatus(0);
	}
}

void DistUserCacheReloaderI::reload(const MyUtil::IntSeq& ids, const Ice::Current& current) {
	ostringstream oss;
	oss << "DistUserCacheReloaderI::reload size = " << ids.size() << " " << current.con->toString().substr(current.con->toString().find("remote", 25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	MyUtil::IntSeq tempids;
	for(MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if(*it > 0) {
			tempids.push_back(*it);
		}
	}
	MyUtil::TaskManager::instance().execute(new ReloadTask(tempids, current));
}

void DistUserCacheReloaderI::insert(int id, const Ice::Current& current) {
	ostringstream oss;
	oss << "DistUserCacheReloaderI::insert userid = " << id << " " << current.con->toString().substr(current.con->toString().find("remote", 25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	MyUtil::TaskManager::instance().execute(new InsertTask(id));
}

void DistUserCacheReloaderI::update(const UserCacheUpdateOldDataPtr& data, const Ice::Current& current) {
	ostringstream oss;
	oss << "DistUserCacheReloaderI::update userid = " << data->userid << " " << " table = " << data->table<<" ";
	for( MyUtil::Str2StrMap::const_iterator it = data->data.begin(); it != data->data.end(); ++ it ) {
		oss<<it->first<<" = "<<it->second<<" , ";
	}
	oss << current.con->toString().substr(current.con->toString().find("remote", 25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	MyUtil::TaskManager::instance().execute(new UpdateTask(data, current));
}

void DistUserCacheReloaderI::setOnline(int id, int online, const Ice::Current& current) {
	ostringstream oss;
	oss << "DistUserCacheReloaderI::setOnline add userid = " << id << " online = " << online << " "
		<< current.con->toString().substr(current.con->toString().find("remote", 25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_DEBUG);
	if(online > 0) {
		maintainOnlineThread_->add(id, online);
	}
	OnlineTypePtr tmp = new OnlineType();
	tmp->userId = id;
	tmp->online = online;
	updateOnlineThread_->add(tmp);
}

void DistUserCacheReloaderI::setOnlineBatch(const OnlineTypeSeq& data, const Ice::Current& current) {
	ostringstream oss;
	oss << "DistUserCacheReloaderI::setOnlineBatch size = " << data.size() << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_DEBUG);
	ostringstream osd;
	osd<<"DistUserCacheReloaderI::setOnlineBatch ";
	for(OnlineTypeSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		int id = (*it)->userId;
		int online = (*it)->online;
		if(online > 0) {
			maintainOnlineThread_->add(id, online);
		}
		osd<<" id = "<<id<<" online = "<<online<<" , ";
	}
	MCE_DEBUG(osd.str());
	updateOnlineThread_->add(data);
}

MyUtil::ObjectResultPtr DistUserCacheReloaderI::getObjectResult(const MyUtil::IntSeq& ids) {
	TimeCost tc = TimeCost::create("DistUserCacheReloaderI::getObjectResult", TimeCost::LOG_LEVEL_DEBUG);
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	getUserPassport(ids, result);
	getUserState(ids, result);
	getUserStage(ids, result);
	getUserBorn(ids, result);
	getUserNames(ids, result);
	getUserUrl(ids, result);

	getOnlineInfo(ids, result);
	MyUtil::ObjectResultPtr final_result = new MyUtil::ObjectResult;
	for(map<long, Ice::ObjectPtr>::const_iterator it = result->data.begin(); it != result->data.end(); ++it) {
		if(xce::usercache::TABLE_NUMBER == UserCacheMemDataPtr::dynamicCast(it->second)->in_tables_) {
			final_result->data.insert(make_pair(it->first, it->second));
		} else {
			MCE_WARN("DistUserCacheReloader::getObjectResult Exception userid = " << it->first << " has uncomplete information!");
		}
	}
	return final_result;
}

Statement DistUserCacheReloaderI::generateSql(const MyUtil::IntSeq& ids, const string& tableName, const string& resultFields) {
	Statement sql;
	sql << "select " << resultFields << " from " << tableName << " where id in (";
	if(ids.size() > 0) {
		sql << *ids.begin();
		for(MyUtil::IntSeq::const_iterator it = ids.begin() + 1; it != ids.end(); ++it) {
			sql << ", " << *it;
		}
	}
	sql << ")";
	return sql;
}

void DistUserCacheReloaderI::getUserPassport(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr& resultPtr) {
	Statement sql = generateSql(ids, "user_passport", "id, status");
	QueryRunner("usercache_passport", CDbRServer).query(sql, BatchUserPassportResultHandler(resultPtr));
}

void DistUserCacheReloaderI::getUserState(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr& resultPtr) {
	Statement sql = generateSql(ids, "user_state", "id, state, star, level");
	QueryRunner("usercache_state", CDbRServer).query(sql, BatchUserStateResultHandler(resultPtr));
}

void DistUserCacheReloaderI::getUserStage(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr& resultPtr) {
	Statement sql = generateSql(ids, "user_stage", "id, univ");
	QueryRunner("usercache_stage", CDbRServer).query(sql, BatchUserStageResultHandler(resultPtr));
}

void DistUserCacheReloaderI::getUserBorn(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr& resultPtr) {
	Statement sql = generateSql(ids, "user_born", "id, gender");
	QueryRunner("usercache_born", CDbRServer).query(sql, BatchUserBornResultHandler(resultPtr));
}

void DistUserCacheReloaderI::getUserNames(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr& resultPtr) {
	Statement sql = generateSql(ids, "user_names", "id, name");
	QueryRunner("usercache_names", CDbRServer).query(sql, BatchUserNamesResultHandler(resultPtr));
}

void DistUserCacheReloaderI::getUserUrl(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr& resultPtr) {
	Statement sql = generateSql(ids, "user_url", "id, tinyurl");
	QueryRunner("usercache_url", CDbRServer).query(sql, BatchUserUrlResultHandler(resultPtr));
}

void DistUserCacheReloaderI::getOnlineInfo(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr& resultPtr) {
	TimeCost tc = TimeCost::create("DistUserCacheReloaderI::getOnlineInfo", TimeCost::LOG_LEVEL_DEBUG);
	try {
		UserOnlineTypeSeq temData = OnlineBitmapAdapter::instance().getOnlineUsers( ids );
		map<int, int> res;
		for( UserOnlineTypeSeq::const_iterator it = temData.begin(); it != temData.end(); ++it )
		{
			res.insert( make_pair( (*it)->userId, (*it)->onlineType ) );
		}
		
		for(map<int,int>::iterator itr = res.begin(); itr != res.end(); ++itr) {
			MyUtil::ObjectMap::iterator it = resultPtr->data.find(itr->first);
			if(it == resultPtr->data.end()) {
				MCE_DEBUG("DistUserCacheReloaderI::getOnlineInfo userid = " << itr->first);
			} else  {
				int online = 0;
				int online_cache = 0;
				online = (itr->second & WEBPAGER_ONLINE) ? ONLINEWEBPAGGER : OFFLINE;
				UserCacheMemDataPtr ptr = UserCacheMemDataPtr::dynamicCast(it->second);
				if(online > 0) {
					online_cache |= (1 << (online - 1));
				}
				online = (itr->second & TALK_ONLINE) ? ONLINEXNT : OFFLINE;
				if(online > 0) {
					online_cache |= (1 << (online - 1));
				}
				ptr->set_online(online_cache);
			}
		}

		map<int, int> wap_res = WapOnlineAdapter::instance().getOnlineStatus(ids);
		for(map<int, int>::iterator itr = wap_res.begin(); itr != wap_res.end(); ++itr) {
			MyUtil::ObjectMap::iterator wap_it = resultPtr->data.find(itr->first);
			if(wap_it == resultPtr->data.end()) {
				MCE_DEBUG("DistUserCacheReloaderI::getOnlineInfo userid = " << itr->first);
			} else {
				int online = 0;
				online = (itr->second & WAP_ONLINE) ? ONLINEWAP : OFFLINEWAP;
				if (online > 0) {
					UserCacheMemDataPtr ptr = UserCacheMemDataPtr::dynamicCast(wap_it->second);
					ptr->set_online(ptr->online() | (1 << (online - 1)));
				}
			}
		}

		map<int,int> idsMap;
		for(MyUtil::ObjectMap::const_iterator it = resultPtr->data.begin(); it != resultPtr->data.end(); ++it) {
			UserCacheMemDataPtr resData = UserCacheMemDataPtr::dynamicCast(it->second);
			if(resData->online() > 0) {
				idsMap.insert(make_pair(it->first, resData->online()));
			}
		}
		if(idsMap.size() > 0) {
			maintainOnlineThread_->add(idsMap);
		}
	} catch(const Ice::Exception& e) {
		MCE_WARN("DistUserCacheReloaderI::getOnlineInfo Exception: " << e);
	} catch(const exception& e) {
		MCE_WARN("DistUserCacheReloaderI::getOnlineInfo Exception: " << e.what());
	} catch(...) {
		MCE_WARN("DistUserCacheReloaderI::getOnlineInfo unknown Exception");
	}
}

BatchUserPassportResultHandler::BatchUserPassportResultHandler(const  MyUtil::ObjectResultPtr& result) : result_(result) {}

bool BatchUserPassportResultHandler::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];
	MyUtil::ObjectMap::iterator it = result_->data.find(id);
	if(it != result_->data.end()) {
		MCE_WARN("BatchUserPassportResultHandler::handle Exception userid = " << id);
	} else {
		UserCacheMemDataPtr data = new UserCacheMemData;
		data->set_id(id);
		data->setUserPassportProperties(row, "status");
		result_->data.insert(make_pair(id, data));
	}
	return true;
}

BatchUserStageResultHandler::BatchUserStageResultHandler(const MyUtil::ObjectResultPtr& result) : result_(result) {}

bool BatchUserStateResultHandler::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];
	MyUtil::ObjectMap::iterator it = result_->data.find(id);
	if(it != result_->data.end()) {
		UserCacheMemDataPtr::dynamicCast(it->second)->setStateProperties(row, "state", "level");
	} else {
			MCE_WARN("BatchUserStateResultHandler::handle Exception user_state has userid = " << id << " but user_passport not.");
	}
	return true;
}

BatchUserBornResultHandler::BatchUserBornResultHandler(const MyUtil::ObjectResultPtr& result) : result_(result) {}

bool BatchUserNamesResultHandler::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];
	MyUtil::ObjectMap::iterator it = result_->data.find(id);
	if(it != result_->data.end()) {
		UserCacheMemDataPtr::dynamicCast(it->second)->setNamesProperties(row, "name");
	} else {
		MCE_WARN("BatchUserNamesResultHandler::handle Exception user_names has userid = " << id << " but user_passport not.");
	}
	return true;
}

BatchUserUrlResultHandler::BatchUserUrlResultHandler(const MyUtil::ObjectResultPtr& result) : result_(result) {}

bool BatchUserUrlResultHandler::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];
	MyUtil::ObjectMap::iterator it = result_->data.find(id);
	if(it != result_->data.end()) {
		UserCacheMemDataPtr::dynamicCast(it->second)->setUrlProperties(row, "tinyurl");
	} else {
		MCE_WARN("BatchUserUrlResultHandler::handle Exception user_url has userid = " << id << " but user_passport not.");
	}
	return true;
}

BatchUserNamesResultHandler::BatchUserNamesResultHandler(const MyUtil::ObjectResultPtr& result) : result_(result) {}

bool BatchUserStageResultHandler::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];
	MyUtil::ObjectMap::iterator it = result_->data.find(id);
	if(it != result_->data.end()) {
		UserCacheMemDataPtr::dynamicCast(it->second)->setStageProperties(row, "univ");
	} else {
		MCE_WARN("BatchUserStageResultHandler::handle Exception user_stage has userid = " << id << " but user_passport not.");
	}
	return true;
}

BatchUserStateResultHandler::BatchUserStateResultHandler(const MyUtil::ObjectResultPtr& result) : result_(result) {}

bool BatchUserBornResultHandler::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];
	MyUtil::ObjectMap::iterator it = result_->data.find(id);
	if(it != result_->data.end()) {
		UserCacheMemDataPtr::dynamicCast(it->second)->setBornProperties(row, "gender");
	} else {
		MCE_WARN("BatchUserBornResultHandler::handle Exception user_born has userid = " << id << " but user_passport not.");
	}
	return true;
}

InsertTask::InsertTask(int id) : MyUtil::Task(TASK_LEVEL_INSERT), id_(id) {}

void InsertTask::handle() {
	TimeCost tc = TimeCost::create("InsertTask::handle", TimeCost::LOG_LEVEL_INFO);
	vector<pair<int, Ice::ObjectPtr> > data;
	data.push_back(pair<int, Ice::ObjectPtr>(id_, 0));
	InsertCallGuarantee::instance().DoInTime(data);
}

InsertCallGuarantee::InsertCallGuarantee() : CallGuarantee(1, 1000, 2, 10000) {
}

bool InsertCallGuarantee::Redo(const vector<int>& ids) {
	TimeCost tc = TimeCost::create("InsertCallGuarantee::Redo", TimeCost::LOG_LEVEL_INFO);
	if(ids.size() == 0) {
		return true;
	}
	try {
		MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
		for(vector<int>::const_iterator it = ids.begin(); it != ids.end(); ++it) {
			UserBaseFullViewPtr full_view_ptr = UserBaseAdapter::instance().getUserBaseFullView(*it);
			result->data.insert(pair<long, Ice::ObjectPtr>(*it, DistUserCacheAdapter::instance().Base2Cache(full_view_ptr)));
		}
		if(DistUserCacheAdapter::instance().setData(result).size() > 0) {
			return false;
		}
		return true;
	} catch (const Ice::Exception& e) {
		MCE_WARN("InsertCallGuarantee::Redo Exception: " << e);
	} catch (const exception& e) {
		MCE_WARN("InsertCallGuarantee::Redo Exception: " << e.what());
	} catch (...) {
		MCE_WARN("InsertCallGuarantee::Redo unknown Exception");
	}
	return false;
}

bool InsertCallGuarantee::Do(const vector<std::pair<int, Ice::ObjectPtr> >& params) {
	TimeCost tc = TimeCost::create("InsertCallGuarantee::Do", TimeCost::LOG_LEVEL_INFO);

	if(params.size() == 0) {
		return true;
	}
	try {
		MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;

		for (vector<pair<int, Ice::ObjectPtr> >::const_iterator it = params.begin(); it != params.end(); ++it) {
			UserBaseFullViewPtr full_view_ptr = UserBaseAdapter::instance().getUserBaseFullView(it->first);
			result->data.insert(pair<long, Ice::ObjectPtr>(it->first, DistUserCacheAdapter::instance().Base2Cache(full_view_ptr)));
		}
		if(result->data.size() == 0) {
			return true;
		}
		if(DistUserCacheAdapter::instance().setData(result).size() > 0) {
			return false;
		}
		return true;
	} catch (const Ice::Exception& e) {
		MCE_WARN("InsertCallGuarantee::Do Exception: " << e);
	} catch (const exception& e) {
		MCE_WARN("InsertCallGuarantee::Do Exception: " << e.what());
	} catch (...) {
		MCE_WARN("InsertCallGuarantee::Do unknown Exception");
	}
	return false;
}
ReloadCallGuarantee::ReloadCallGuarantee() : CallGuarantee(1, 1000, 2, 10000) {
}

bool ReloadCallGuarantee::Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params) {
	TimeCost tc = TimeCost::create("ReloadCallGuarantee::Do", TimeCost::LOG_LEVEL_INFO);
	if (params.size() == 0) {
		return true;
	}

	MyUtil::IntSeq ids;
	for(std::vector<std::pair<int, Ice::ObjectPtr> >::const_iterator it = params.begin(); it != params.end(); ++it)
	{
		ids.push_back(it->first);
	}
	try {
		MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
		DistUserCacheReloaderI::instance().getUserPassport(ids, result);
		if(result->data.empty())
		{
			return true;
		}
		result->data.clear();
		for(vector<std::pair<int, Ice::ObjectPtr> >::const_iterator it = params.begin(); it != params.end(); ++it) {
			try{
				UserBaseFullViewPtr full_view_ptr = UserBaseAdapter::instance().getUserBaseFullView(it->first);
				if(full_view_ptr) {
					result->data.insert(pair<long, Ice::ObjectPtr>(it->first, DistUserCacheAdapter::instance().Base2Cache(full_view_ptr)));
			}
			}catch(Ice::UnknownException& e){
				MCE_WARN("ReloadCallGuarantee::Do id = " << it->first << " Ice::UnknownException: " << e);
				continue;
			}
		}
		if (result->data.empty()) {
			return true;
		}
		if(DistUserCacheAdapter::instance().setData(result).size() > 0) {
			return false;
		}
		return true;
	} catch (const Ice::Exception& e) {
		MCE_WARN("ReloadCallGuarantee::Do Exception: " << e);
	} catch (const exception& e) {
		MCE_WARN("ReloadCallGuarantee::Do Exception: " << e.what());
	} catch (...) {
		MCE_WARN("ReloadCallGuarantee::Do unknown Exception");
	}
	return false;
}

bool ReloadCallGuarantee::Redo(const std::vector<int>& ids) {
	TimeCost tc = TimeCost::create("ReloadCallGuarantee::Redo", TimeCost::LOG_LEVEL_INFO);
	if(ids.size() == 0) {
		return true;
	}
	try {
		MyUtil::ObjectResultPtr result = DistUserCacheReloaderI::instance().getObjectResult(ids);
		if(result->data.size() == 0) {
			return true;
		}
		if(DistUserCacheAdapter::instance().setData(result).size() > 0) {
			return false;
		}
		return true;
	} catch (const Ice::Exception& e) {
		MCE_WARN("ReloadCallGuarantee::Redo Exception: " << e);
	} catch (const exception& e) {
		MCE_WARN("ReloadCallGuarantee::Redo Exception: " << e.what());
	} catch (...) {
		MCE_WARN("ReloadCallGuarantee::Redo unknown Exception");
	}
	return false;
}

ReloadTask::ReloadTask(const MyUtil::IntSeq& ids, const Ice::Current& current) : MyUtil::Task(TASK_LEVEL_RELOAD),ids_(ids), current_(current) {
}

void ReloadTask::handle() {
	TimeCost tc = TimeCost::create("ReloadTask::handle", TimeCost::LOG_LEVEL_INFO);
	vector<pair<int, Ice::ObjectPtr> > data;
	for(vector<int>::const_iterator i = ids_.begin(); i != ids_.end(); ++i) {
		data.push_back(pair<int, Ice::ObjectPtr>(*i, 0));
	}
	ReloadCallGuarantee::instance().DoInTime(data);
}


UpdateCallGuarantee::UpdateCallGuarantee() : CallGuarantee(1, 1000, 2, 10000) {
}

bool UpdateCallGuarantee::Redo(const std::vector<int>& ids) {
	TimeCost tc = TimeCost::create("UpdateCallGuarantee::Redo", TimeCost::LOG_LEVEL_INFO);
	if(ids.size() == 0) {
		return true;
	}
	try {
		MyUtil::ObjectResultPtr result = DistUserCacheReloaderI::instance().getObjectResult(ids);
		if(result->data.size() == 0) {
			return true;
		}

		if(DistUserCacheAdapter::instance().setData(result).size() > 0) {
			return false;
		}
		return true;
	} catch (const Ice::Exception& e) {
		MCE_WARN("UpdateCallGuarantee::Redo Exception: " << e);
	} catch (const exception& e) {
		MCE_WARN("UpdateCallGuarantee::Redo Exception: " << e.what());
	} catch (...) {
		MCE_WARN("UpdateCallGuarantee::Redo unknown Exception");
	}
	return false;
}

bool UpdateCallGuarantee::Do(const vector<pair<int, Ice::ObjectPtr> >& params) {
	TimeCost tc = TimeCost::create("UpdateCallGuarantee::Do", TimeCost::LOG_LEVEL_INFO);
	if(params.size() != 1) {
		MCE_WARN("UpdateCallGuarantee::Do Exception params.size is not 1 but " << params.size());
		return true;
	}

	try {
		if(DistUserCacheAdapter::instance().setData(UserCacheMemDataPtr::dynamicCast(params.begin()->second)) == false) {
			return false;
		}
		return true;
	} catch (const Ice::Exception& e) {
		MCE_WARN("UpdateCallGuarantee::Do Exception: " << e);
	} catch (const exception& e) {
		MCE_WARN("UpdateCallGuarantee::Do Exception: " << e.what());
	} catch (...) {
		MCE_WARN("UpdateCallGuarantee::Do unknown Exception");
	}
	return false;
}

UpdateTask::UpdateTask(const UserCacheUpdateOldDataPtr& data, const Ice::Current& current): MyUtil::Task(TASK_LEVEL_UPDATE), data_(data), current_(current) {
}

void UpdateTask::handle() {
	TimeCost tc = TimeCost::create("UpdateTask::handle", TimeCost::LOG_LEVEL_INFO);
	UserCacheMemDataPtr data = getNewData();
	if(data == 0) {
		ostringstream os;
		os << "UpdateTask::handle reload userid = " << data_->userid;
		MyUtil::IntSeq tempids;
		tempids.push_back(data_->userid);
		MyUtil::TaskManager::instance().execute(new ReloadTask(tempids, Ice::Current()));
		MCE_WARN(os.str());
		return;
	}
	vector<pair<int, Ice::ObjectPtr> > d;
	d.push_back(pair<int, Ice::ObjectPtr>(data->id(), data));
	UpdateCallGuarantee::instance().DoInTime(d);
}

UserCacheMemDataPtr UpdateTask::getNewData() {
	TimeCost tc = TimeCost::create("UpdateTask::getNewData", TimeCost::LOG_LEVEL_DEBUG);
	UserCacheMemDataPtr data = DistUserCacheAdapter::instance().getDataWithCAS(data_->userid);
	if(data == 0) {
		return 0;
	}
	Str2StrMap::const_iterator i;
	switch (data_->table) {
		case TABLEPASSPORT:
			i = data_->data.find("STATUS");
			if(i != data_->data.end()) {
				data->set_status(boost::lexical_cast<Ice::Int>(i->second));
			}
			break;
		case TABLEBORN:
			i = data_->data.find("GENDER");
			if(i != data_->data.end()) {
				if(i->second == "男生") { 
					data->set_gender(0x0f & 1);
				} else if(i->second == "女生") {
					data->set_gender(0x0f & 2);
				} else {
					data->set_gender(0x0f & 0);
				}
			}
			break;
		case TABLESTATE:
			i = data_->data.find("STATE");
			if(i != data_->data.end()) {
				data->set_state(boost::lexical_cast<Ice::Int>(i->second));
			}
			i = data_->data.find("LEVEL");
			if(i != data_->data.end()) {
				data->set_level(boost::lexical_cast<Ice::Int>(i->second) );
			}
			break;
		case TABLENAMES:
			i = data_->data.find("NAME");
			if(i != data_->data.end()) {
				data->set_name(i->second);
			}
			break;
		case TABLEURL:
			i = data_->data.find("TINYURL");
			if(i != data_->data.end()) {
				data->set_tinyurl(i->second );
			}
			break;
		case TABLESTAGE:
			i = data_->data.find("UNIV");
			if(i != data_->data.end()) {
				data->set_univ( boost::lexical_cast<Ice::Int>(i->second) );
			}
			break;
		default:
			MCE_WARN("UpdateTask::getNewData Exception no table name");
	}
	return data;
}

