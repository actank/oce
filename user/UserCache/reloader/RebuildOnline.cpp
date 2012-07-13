/*
 * RebuildOnline.cpp
 *
 *  Created on: 2008-9-10
 *      Author: bbc
 */

#include "RebuildOnline.h"
#include "UtilCxx/src/TaskManager.h"
#include "OceCxxAdapter/src/PresenceAdapter.h"
#include "OceCxxAdapter/src/WapOnlineAdapter.h"
#include "OceCxxAdapter/src/UserCacheAdapter.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <string>
#include "UtilCxx/src/Singleton.h"

using namespace ::xce::usercache;
using namespace ::MyUtil;
using namespace com::xiaonei::wap::adapter;

using xce::usercache::TASK_LEVEL_SETONLINE;
using mop::hi::svc::adapter::PresenceAdapter;
using mop::hi::svc::adapter::WEBPAGER_ONLINE;
using mop::hi::svc::adapter::TALK_ONLINE;

#include "OnlineEventMediator.h"
using xce::mediator::onlineevent::OFFLINE;
using xce::mediator::onlineevent::ONLINEWEBPAGGER;
using xce::mediator::onlineevent::ONLINEXNT;
using xce::mediator::onlineevent::ONLINEWAP;
using xce::mediator::onlineevent::OFFLINEWAP;

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
			TaskManager::instance().execute(new SetOnlineTask(reIds));
		}catch(...){
			cout << "Something causes RebuildOnline loop crash" << endl;
		}
	}
}

SetOnlineTask::SetOnlineTask(MyUtil::IntSeq& ids) :
	Task(TASK_LEVEL_SETONLINE),_ids(ids) {
}

void SetOnlineTask::handle() {
	map<int, int> result;
	try {
		map<int, int> res = PresenceAdapter::instance().getOnlineStat(_ids);
		for (MyUtil::IntSeq::iterator it = _ids.begin(); it != _ids.end(); ++it) {
			map<int, int>::iterator resIt = res.find(*it);
			if (resIt == res.end()) {
				result.insert(make_pair(*it, OFFLINE));
			} else {
				int online = 0;
				int value = 0;
				online = (resIt->second & WEBPAGER_ONLINE) ? ONLINEWEBPAGGER
								: OFFLINE;
				if (online > 0) {
					value |= (1 << (online - 1));
				}

				online = (resIt->second & TALK_ONLINE) ? ONLINEXNT
						: OFFLINE;
				if (online > 0) {
					value |= (1 << (online - 1));
				}

				result.insert(make_pair(*it,value));
			}
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("PresenceAdapter Ice::Exception " << e.what());
		for (MyUtil::IntSeq::iterator it = _ids.begin(); it != _ids.end(); ++it) {
				result.insert(make_pair(*it, OFFLINE));
		}
	}
	//TODO WapOnlineAdapter
	try {
		map<int, int> wap_res = WapOnlineAdapter::instance().getOnlineStatus(_ids);
		for (map<int, int>::iterator it = wap_res.begin(); it != wap_res.end(); ++it) {
			MCE_DEBUG("id " << it->first << " wap_status " << it->second);
			map<int, int>::iterator resIt = result.find(it->first);
			if (resIt == result.end()) {
				MCE_WARN("wap return a wrong id=" << it->first);
			} else {
				int online = 0;
				online = (it->second & WAP_ONLINE) ? ONLINEWAP
						: OFFLINEWAP;
				if (online > 0) {
					resIt->second |= (1 << (online - 1));
				}
			}
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("WapOnlineAdapter Ice::Exception " << e.what());
	}

	UserCacheAdapter::instance().reloadSetOnline(result);
}

