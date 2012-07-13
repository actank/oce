/*
 * OnlineTimeCache.cpp
 *
 *  Created on: 2008-12-29
 *      Author: bbc
 */

#include "OnlineTimeCache.h"
using xce::recentlyonlinebuddy::OnlineTimeCache;
using xce::recentlyonlinebuddy::ONLINE_TIME_TRUNKS;
using __gnu_cxx::hash_map;

#include "QueryRunner.h"
using com::xiaonei::xce::QueryRunner;
using com::xiaonei::xce::Statement;

#include "ServiceI.h"
using MyUtil::ServiceI;

#include "RecentlyOnlineBuddyI.h"
using xce::recentlyonlinebuddy::CATEGORY_DATA;

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
	try {
		int max_id = 0;
		int large_id = 0;
		do {
			Statement sql;
			sql << "SELECT id, UNIX_TIMESTAMP(lastlogintime) AS llt"
				" FROM user_desc_ab WHERE id > " << max_id << " LIMIT 10000";
			map<int, int> db_data;
			com::xiaonei::xce::QueryRunner("recentlyonlinebuddy_user_desc_ab").query(
					sql, OTHandler(
							large_id, db_data));
			for (map<int, int>::const_iterator it = db_data.begin(); it
					!= db_data.end(); ++it) {
				int pos=it->first % ONLINE_TIME_TRUNKS;
				IceUtil::RWRecMutex::RLock lock(_online_time.at(pos)->mutex);
				hash_map<int, int>::const_iterator cache_it = _online_time.at(pos)->data.find(
						it->first);
				if (cache_it == _online_time.at(pos)->data.end()) {
					lock.upgrade();
					/** not override online time when the Cache is initializeing. **/
					_online_time.at(pos)->data[it->first] = it->second;
				}
			}
			if (large_id > max_id) {
				max_id = large_id;
			} else {
				break;
			}
		} while (true);
	} catch (...) {
		MCE_WARN("OnlineTimeCache loading exit with unknown exception.");
	}
	MCE_INFO("OnlineTimeCache thread loading done.");
	{
		IceUtil::RWRecMutex::WLock lock(_inited_flag_mutex);
		_inited_flag = true;
	}
	MCE_INFO("OnlineTimeCache thread set flag done.");
	try {
		vector<long> pendding;
		{
			IceUtil::Mutex::Lock lock(_pendding_sort_mutex);
			pendding.swap(_pendding_sort_userids);
		}
		MyUtil::ServiceI::instance().getObjectCache()->reloadObjects(
				CATEGORY_DATA, pendding);
	} catch (...) {
		MCE_WARN("OnlineTimeCache reload pendding exit with unknown exception.");
	}
	MCE_INFO("OnlineTimeCache thread all done.");
}
void OnlineTimeCache::set(int userid, int time) {
	IceUtil::RWRecMutex::WLock lock(_online_time.at(userid % ONLINE_TIME_TRUNKS)->mutex);
	_online_time.at(userid % ONLINE_TIME_TRUNKS)->data[userid] = time;
}
vector<int> OnlineTimeCache::sort(int userid, const std::vector<int>& buddies) {
	vector<int> result(buddies);
	bool inited = false;
	{
		IceUtil::RWRecMutex::RLock lock(_inited_flag_mutex);
		inited = _inited_flag;
	}
	if (inited) {
		std::map<int, int> times;
		for (std::vector<int>::const_iterator it = buddies.begin(); it
				!= buddies.end(); ++it) {
			int pos = *it % ONLINE_TIME_TRUNKS;
			IceUtil::RWRecMutex::RLock lock(_online_time.at(pos)->mutex);
			hash_map<int, int>::const_iterator time_it = _online_time.at(pos)->data.find(*it);
			if (time_it != _online_time.at(pos)->data.end()) {
				times.insert(make_pair(time_it->first, time_it->second));
			}
		}
		std::sort(result.begin(), result.end(), GreaterOnlineTime(times));
	} else {
		random_shuffle(result.begin(), result.end());
		IceUtil::Mutex::Lock lock(_pendding_sort_mutex);
		_pendding_sort_userids.push_back(userid);
	}
	return result;
}
