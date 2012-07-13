/*
 * OnlineTimeCache.h
 *
 *  Created on: 2008-12-29
 *      Author: bbc
 */

#ifndef ONLINETIMECACHE_H_
#define ONLINETIMECACHE_H_

#include "QueryRunner.h"

#include <ext/hash_map>

namespace xce {

namespace recentlyonlinebuddy {

const static int ONLINE_TIME_TRUNKS = 10111;

class GreaterOnlineTime: public binary_function<int, int, bool> {
public:
	GreaterOnlineTime(std::map<int, int> order_data) :
		_order_data(order_data) {
	}
	bool operator()(int x, int y) {
		std::map<int, int>::const_iterator it_x=_order_data.find(x);
		std::map<int, int>::const_iterator it_y=_order_data.find(y);
		if (it_x == _order_data.end() || it_y == _order_data.end())
			return false;
		return it_x->second > it_y->second;
	}
private:
	std::map<int, int> _order_data;
};

class OnlineTimeCache: public MyUtil::Singleton<OnlineTimeCache> {
private:
	class OTHandler: public com::xiaonei::xce::ResultHandler {
	public:
		OTHandler(int& large_id, map<int, int>& result);
	protected:
		virtual bool handle(mysqlpp::Row& res) const;
	private:
		int& _large_id;
		map<int, int>& _result;
	};
	friend class MyUtil::Singleton<OnlineTimeCache>;
	OnlineTimeCache(): _inited_flag(false){
		for ( int x=0;x<ONLINE_TIME_TRUNKS;x++ ) _online_time.push_back(new _OnlineTimeDataTrunk);
	}
public:
	void initialize();
	void set(int userid, int time);
	vector<int> sort(int userid, const std::vector<int>& buddies);
private:
	bool _inited_flag;
	IceUtil::RWRecMutex _inited_flag_mutex;

	vector<long> _pendding_sort_userids;
	IceUtil::Mutex _pendding_sort_mutex;

	struct _OnlineTimeDataTrunk {
		__gnu_cxx::hash_map<int, int> data;
		IceUtil::RWRecMutex mutex;
	};
	vector<_OnlineTimeDataTrunk*> _online_time;
};

class OnlineTimeCacheInitThread: public IceUtil::Thread {
public:
	virtual void run(){
		OnlineTimeCache::instance().initialize();
	};
};
typedef IceUtil::Handle<OnlineTimeCacheInitThread> OnlineTimeCacheInitThreadPtr;

}
}

#endif /* ONLINETIMECACHE_H_ */
