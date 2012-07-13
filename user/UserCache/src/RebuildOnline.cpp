/*
 * RebuildOnline.cpp
 *
 *  Created on: 2008-9-10
 *      Author: bbc
 */

#include "RebuildOnline.h"
#include "UtilCxx/src/TaskManager.h"
#include <UserCacheReloaderAdapter.h>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <string>
#include "UtilCxx/src/Singleton.h"

using namespace ::xce::usercache;
using namespace ::MyUtil;

RebuildOnline::RebuildOnline(){
	_cache.reserve(CACHE_SIZE);
}

void RebuildOnline::rebuild(int id) 
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_cache.push_back(id);
	if (_cache.size() > REBUILD_SIZE) {
		_mutex.notify();
	}
}

void RebuildOnline::rebuild(const MyUtil::IntSeq& ids) 
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_cache.insert(_cache.end(), ids.begin(), ids.end());
	if (_cache.size() > REBUILD_SIZE) {
		_mutex.notify();
	}
}

void RebuildOnline::run() {
	while (true) {
		try{
			MyUtil::IntSeq reIds;
			reIds.reserve(CACHE_SIZE);
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				if (_cache.size() <= REBUILD_SIZE) {
					_mutex.timedWait(IceUtil::Time::seconds(WAIT_TIME));
				}
				if (!_cache.empty()) {
					reIds.swap(_cache);
				} else {
					continue;
				}
			}
			TaskManager::instance().execute(new RebuildOnlineTask(reIds));
		}catch(...){
			cout << "Something causes RebuildOnline loop crash" << endl;
		}
	}
}

RebuildOnlineTask::RebuildOnlineTask(MyUtil::IntSeq& ids) :
	Task(TASK_LEVEL_REBUILDONLINE),_ids(ids) {
}

void RebuildOnlineTask::handle() {
	UserCacheReloaderAdapter::instance().getProxy(0)->rebuildOnline(_ids);
}

