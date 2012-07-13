#include "OnlineTimeCache.h"
using xce::buddy::OnlineTimeCache;
using xce::buddy::ONLINE_TIME_TRUNKS;
using __gnu_cxx::hash_map;

#include "QueryRunner.h"
using com::xiaonei::xce::QueryRunner;
using com::xiaonei::xce::Statement;

#include "BuddyByOnlineTimeLoaderI.h"
using xce::buddy::BuddyByOnlineTimeLoaderI;

#include "TaskManager.h"
using MyUtil::TaskManager;

OnlineTimeCache::OTHandler::OTHandler(int& large_id, map<int, int>& result) :
	_large_id(large_id), _result(result) {
}

bool OnlineTimeCache::OTHandler::handle(mysqlpp::Row& res) const {
	int uid = res["id"];
	_large_id = uid > _large_id ? uid : _large_id;
	int time = res["llt"];
	_result.insert(make_pair(uid, time));
	return true;
}

void OnlineTimeCache::initialize() {
        MCE_INFO("[OnlineTimeCache::initialize] start ...");
	try {
		int max_id = 0;
		int large_id = 0;
		do {
			Statement sql;
			sql << "SELECT id, UNIX_TIMESTAMP(lastlogintime) AS llt"
				" FROM user_time WHERE id > " << max_id << " LIMIT 10000";
			map<int, int> db_data;
			com::xiaonei::xce::QueryRunner( "user_time",com::xiaonei::xce::CDbRServer ).query(
					sql, OTHandler( large_id, db_data)
			);
			for (map<int, int>::const_iterator it = db_data.begin(); it
					!= db_data.end(); ++it) {
				int pos=it->first % ONLINE_TIME_TRUNKS;
				IceUtil::RWRecMutex::RLock lock(_online_time.at(pos)->mutex);
				hash_map<int, int>::const_iterator cache_it = _online_time.at(pos)->data.find(
						it->first);
				if (cache_it == _online_time.at(pos)->data.end()) {
					lock.upgrade();
					_online_time.at(pos)->data[it->first] = it->second;
				}
			}
			if (large_id > max_id) {
				max_id = large_id;
			} else {
				break;
			}
                        MCE_INFO("[OnlineTimeCache::initialize] initializing to " << large_id);
		} while (true);
	} catch (...) {
                MCE_WARN("[OnlineTimeCache::initialize] loading exit with unknown exception.");
	}
        MCE_INFO("[OnlineTimeCache::initialize] thread loading done.");
	{
		IceUtil::RWRecMutex::WLock lock(_inited_flag_mutex);
		_inited_flag = true;
	}
        MCE_INFO("[OnlineTimeCache::initialize] thread set flag done.");
	try {
                vector<int> pendding;
                {
                        IceUtil::Mutex::Lock lock(_pendding_sort_mutex);
                        pendding.swap(_pendding_sort_userids);
                }
                MyUtil::IntSeq ids;
                for( vector<int>::iterator it = pendding.begin(); it
                                != pendding.end(); ++it) {
			TaskManager::instance().execute( new ReloadTask(*it) );
			/*
                        ids.push_back( *it );
                        if( ids.size() >= 1000 ){
                                CacheLoader<BuddyByOnlineTimeCacheManagerPrx>::instance().reload( ids );
                                ids.clear();
                        }
			*/
                }
	} catch (...) {
		MCE_WARN("OnlineTimeCache reload pendding exit with unknown exception.");
	}
	MCE_INFO("OnlineTimeCache thread all done.");
}
bool OnlineTimeCache::isValid(){
        IceUtil::RWRecMutex::RLock lock(_inited_flag_mutex);
        return _inited_flag;
}
void OnlineTimeCache::set(int userId, int time) {
	IceUtil::RWRecMutex::WLock lock(_online_time.at(userId % ONLINE_TIME_TRUNKS)->mutex);
	_online_time.at(userId % ONLINE_TIME_TRUNKS)->data[userId] = time;
}
MyUtil::IntSeq OnlineTimeCache::sort(int userId, const std::vector<int>& buddies) {
        MyUtil::IntSeq result(buddies);
	bool inited = false;
	{
		IceUtil::RWRecMutex::RLock lock(_inited_flag_mutex);
		inited = _inited_flag;
	}
	if (inited) {
		std::map<int, int> times;
		for (std::vector<int>::const_iterator it = buddies.begin(); it
				!= buddies.end(); ++it) {
			if( *it < 0 ){
				continue;
			}
			int pos = *it % ONLINE_TIME_TRUNKS;
			IceUtil::RWRecMutex::RLock lock(_online_time.at(pos)->mutex);
			hash_map<int, int>::const_iterator time_it = _online_time.at(pos)->data.find(*it);
			if (time_it != _online_time.at(pos)->data.end()) {
				times.insert(make_pair(time_it->first, time_it->second));
			}
		}
		std::sort(result.begin(), result.end(), GreaterOnlineTime(times));
		MCE_DEBUG("[OnlineTimeCache::sort] userId=" << userId << " size=" << result.size() );
	} else {
		random_shuffle(result.begin(), result.end());
		IceUtil::Mutex::Lock lock(_pendding_sort_mutex);
		_pendding_sort_userids.push_back(userId);
	}
	return result;
}

xce::buddy::OnlineTimeSeq OnlineTimeCache::getOnlineTime(int userId, const std::vector<int>& buddies) {
        OnlineTimeSeq times;
	bool inited = false;
	{
		IceUtil::RWRecMutex::RLock lock(_inited_flag_mutex);
		inited = _inited_flag;
	}
	if (inited) {
		for (std::vector<int>::const_iterator it = buddies.begin(); it
				!= buddies.end(); ++it) {
			OnlineTime one;
			one.userId = *it;
			if( *it < 0 ){
				one.time = time(NULL);
				times.push_back( one );
				continue;
			}
			int pos = *it % ONLINE_TIME_TRUNKS;
			IceUtil::RWRecMutex::RLock lock(_online_time.at(pos)->mutex);
			hash_map<int, int>::const_iterator time_it = _online_time.at(pos)->data.find(*it);
			if (time_it != _online_time.at(pos)->data.end()) {
				one.time = time_it->second;
			} else{
				one.time = time(NULL);
			}
			times.push_back( one );
		}
		MCE_DEBUG("[OnlineTimeCache::getOnlineTime] userId=" << userId << " size=" << times.size() );
	} else {
		time_t now = time(NULL);
		for (std::vector<int>::const_iterator it = buddies.begin(); it
				!= buddies.end(); ++it) {
			OnlineTime one;
			one.userId = *it;
			one.time = now;
			times.push_back( one );
		}
		IceUtil::Mutex::Lock lock(_pendding_sort_mutex);
		_pendding_sort_userids.push_back(userId);
	}
	return times;
}
