/*
 * UserCacheAdapter.cpp
 *
 *  Created on: 2008-8-22
 *      Author: bbc
 */

#include "UserCacheAdapter.h"
#include "DistUserCacheReloaderAdapter.h"
#include "util/cpp/IpAddr.hpp"
#include <boost/lexical_cast.hpp>

using namespace xce::usercache;
using namespace xce::clusterstate;
using namespace MyUtil;

// ========= UserCacheAdapter =================================
void UserCacheAdapter::setOnline(int userId, int online) {

	_invokeSetOnline.invoke(userId, online);

}

void UserCacheAdapter::setOnline(const MyUtil::Int2IntMap& data) {

	_invokeSetOnline.invoke(data);

}

void UserCacheAdapter::reloadSetOnline(const MyUtil::Int2IntMap& data) {
	doReloadSetOnline(data);
}

UserCacheAdapter::UserCacheAdapter() : MyUtil::ReplicatedClusterAdapterI <UserCacheManagerPrx>("ControllerUserCacheN", 120, 300) {
	while( ( _distCacheAdapterPtr = xce::distcache::DistCacheAdapter::connect("UserCache") )== NULL) {
		MCE_WARN("UserCacheAdapter::UserCacheAdapter can not init DistCacheAdapter!");
	}
	_invokeSetOnline.start(128 * 1024).detach();

}

void UserCacheAdapter::doSetOnline(const MyUtil::Int2IntMap& data) {
	   MCE_DEBUG("UserCacheAdapter::doSetOnline userId = " << data.size());

	   for (Int2IntMap::const_iterator it = data.begin(); it != data.end(); ++it) {
			MCE_DEBUG("UserCacheAdapter::doSetOnline userID "<< it->first<< " onlineType : " << it->second);
		}

		map<int, vector<UserCacheManagerPrx> > mapAll(getAvailableProxiesOneway());
		for ( map<int, vector<UserCacheManagerPrx> >::iterator it = mapAll.begin();
				   it != mapAll.end(); ++it ) {
			for ( vector<UserCacheManagerPrx>::iterator it2 = (*it).second.begin();
					it2 != (*it).second.end(); ++it2 ) {
				(*it2)->setOnline(data);
			}
		}

}

void UserCacheAdapter::doReloadSetOnline(const MyUtil::Int2IntMap& data) {

	map<int, vector<UserCacheManagerPrx> > mapAll(getAvailableProxiesOneway());
	for ( map<int, vector<UserCacheManagerPrx> >::iterator it = mapAll.begin();
		       it != mapAll.end(); ++it ) {
		for ( vector<UserCacheManagerPrx>::iterator it2 = (*it).second.begin(); 
				it2 != (*it).second.end(); ++it2 ) {
			(*it2)->reloadSetOnline(data);
		}
	}
}

std::map<int, UserCachePtr> UserCacheAdapter::GetUserCache(const MyUtil::IntSeq& userIds) {

	map<int, UserCachePtr> allResults;
	if (userIds.size() == 0) {
		return allResults;
	}

	vector<long> lIds(userIds.begin(), userIds.end());

	std::vector<int> missIds;
		std::map<long, xce::distcache::DataPtr> propMap = _distCacheAdapterPtr->get(lIds);
					
		for(std::map<long, xce::distcache::DataPtr>::const_iterator it = propMap.begin(); it != propMap.end(); ++it) {
			if(it->second == NULL) {
				MCE_WARN("UserCacheAdapter::GetUserCache not in memcached userid = " << it->first);
				continue;
			}
			std::string dataStr;
			dataStr.assign(it->second->begin(), it->second->end());
			xce::distcache::usercache::UserCacheData data;
			data.ParseFromString(dataStr);
			allResults.insert(make_pair(it->first,new UserCache(data)));
		}
	for( MyUtil::IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it ) {
		if( allResults.find( *it ) == allResults.end() ) {
			missIds.push_back( *it );
		}
	}
	if( missIds.size() > 0 ) {
		try {
			xce::distcache::usercache::DistUserCacheReloaderAdapter::instance().reload( missIds );
		}catch(...) {
		}
	}
	return allResults;
}

// ======== InvokdeSetOnline ===========================================
// 
void InvokeSetOnline::invoke(int userId, int online) {

		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
		bool needsNotify = _cache.empty();
		_cache.insert(make_pair(userId, online));
		if (needsNotify) {
			_mutex.notify();
		}

}

void InvokeSetOnline::invoke(const MyUtil::Int2IntMap& data) {

		if (data.empty()) {
			return;
		}
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
		bool needsNotify = _cache.empty();
		_cache.insert(data.begin(), data.end());
		if (needsNotify) {
			_mutex.notify();
		}

}

void InvokeSetOnline::run() {
	while (true) {
	  try {
			MyUtil::Int2IntMap batch;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				if (_cache.empty()) {
					_mutex.wait();
					continue;
				}
				batch.swap(_cache);
			}
			UserCacheAdapter::instance().doSetOnline(batch);
			sleep(1);
		} catch (...) {
			//MCE_WARN("UserCacheAdapter InvokeSetOnline::run() Exception");
		}
	}
}
UserCache::UserCache(const xce::distcache::usercache::UserCacheData& data) {
	id_ = data.id();
	status_ = data.status();
	name_ = data.name();
	univ_ = data.univ();
	tinyurl_ = data.tinyurl();
	if (data.gender()==1) {
		gender_ = "男生";
	} else if (data.gender()==2) {
		gender_ = "女生";
	} else {
		gender_ = "";
	}
	state_ = data.state();
	level_ = data.level();
	online_ = data.online();
}

