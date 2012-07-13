#include "PresenceI.h"
#include "IceLogger.h"
#include <IceUtil/Time.h>
//#include "TalkLogicAdapter.h"
#include "TaskManager.h"
#include "ConnectionQuery.h"
#include "Date.h"
using namespace com::xiaonei::talk;
//using namespace mop::hi::svc::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;

//---------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add( &PresenceManagerI::instance(), service.createIdentity("M",
			""));
}

//---------------------------------------------------------------------------
//User
bool User::add(const JidPtr& jid, const string& show, const string& ver,
		const string& ext) {

	if (jid->endpoint.empty()) {
		MCE_WARN("endpoint is empty");
		return 0;
	}

	if (has(jid))
		return false;

	Avatar1Ptr a = new Avatar1();
	a->endpoint = jid->endpoint;
	a->show = show;
	a->ver = ver;
	a->ext = ext;

	if ('W' == jid->endpoint.at(0)) {
		//MCE_DEBUG("[User::add] "<<endpoint<<" added in _avatars_pager");
		_avatars_pager[jid->index] = a;
	} else if ('T' == jid->endpoint.at(0)) {
		//MCE_DEBUG("[User::add] "<<endpoint<<" added in _avatars_IM");
		if (_avatars_IM.empty()) {
			_duration = time(NULL);
		}
		_avatars_IM[jid->index] = a;
	} else if ("web" == jid->endpoint) {
		//MCE_DEBUG("[User::add] "<<endpoint<<" added in _avatars_web");
		_avatars_web[1] = a;
	} else {
		MCE_WARN("endpoint "<<jid->endpoint<<" is incorrect");
		return false;
	}

	return true;

}

void User::set(const JidPtr& jid, const string& show, const string& ver,
		const string& ext) {

	Iterator it;
	if ((it = _avatars_web.find(jid->index)) != _avatars_web.end()) {
		//MCE_DEBUG("[User::set] index "<<index<<" found in _avatars_web");
	} else if ((it = _avatars_pager.find(jid->index)) != _avatars_pager.end()) {
		//MCE_DEBUG("[User::set] index "<<index<<" found in _avatars_pager");
	} else if ((it = _avatars_IM.find(jid->index)) != _avatars_IM.end()) {
		//MCE_DEBUG("[User::set] index "<<index<<" found in _avatars_IM");
	} else {
		MCE_WARN("[User::set] Can not find the avatar whose index is "
				<<jid->index);
		return;
	}

	//if (!show.empty())
	it->second->show = show;
	if (!ver.empty())
		it->second->ver = ver;
	if (!ext.empty())
		it->second->ext = ext;
}

bool User::has(const JidPtr& jid) {
	if (_avatars_web.find(jid->index) != _avatars_web.end()) {
		//MCE_DEBUG("[User::has] index "<<index<<" found in _avatars_web");
		return true;
	} else if (_avatars_pager.find(jid->index) != _avatars_pager.end()) {
		//MCE_DEBUG("[User::has] index "<<index<<" found in _avatars_pager");
		return true;
	} else if (_avatars_IM.find(jid->index) != _avatars_IM.end()) {
		//MCE_DEBUG("[User::has] index "<<index<<" found in _avatars_IM");
		return true;
	}

	return false;
}

bool User::remove(const JidPtr& jid) { // if _map is empty return true; else return false
	Iterator it;
	if ((it = _avatars_web.find(jid->index)) != _avatars_web.end()) {
		//MCE_DEBUG("[User::remove] index "<<index<<" found in _avatars_web");
		_avatars_web.erase(it);
	} else if ((it = _avatars_pager.find(jid->index)) != _avatars_pager.end()) {
		//MCE_DEBUG("[User::remove] index "<<index<<" found in _avatars_pager");
		_avatars_pager.erase(it);
	} else if ((it = _avatars_IM.find(jid->index)) != _avatars_IM.end()) {
		//MCE_DEBUG("[User::remove] index "<<index<<" found in _avatars_IM");
		_avatars_IM.erase(it);
	} else {
		MCE_WARN("[User::remove] Can not find the avatar whose jid is "
				<<jid->userId<<"@talk.xiaonei.com/"<<jid->endpoint<<"_"
				<<jid->index);
	}

	return (_avatars_web.empty() && _avatars_pager.empty()
			&& _avatars_IM.empty());
}

bool User::remove(const string& endpoint, JidSeq &seq) {

	//	bool empty = true;
	bool empty1 = _remove(_avatars_web, endpoint, seq);
	bool empty2 = _remove(_avatars_pager, endpoint, seq);
	bool empty3 = _remove(_avatars_IM, endpoint, seq);
	return empty1&&empty2&&empty3;
}

void User::getJidSeq(JidSeq &seq) {
	_getJidSeq(_avatars_web, seq);
	_getJidSeq(_avatars_pager, seq);
	_getJidSeq(_avatars_IM, seq);
}

void User::getAsOnlineInfoSeq(OnlineInfoSeq &seq) {
	_getAsOnlineInfoSeq(_avatars_web, seq);
	_getAsOnlineInfoSeq(_avatars_pager, seq);
	_getAsOnlineInfoSeq(_avatars_IM, seq);
}
OnlineInfoPtr User::getAsOnlineInfo(const JidPtr & jid) {
	OnlineInfoPtr info;
	if ('W' == jid->endpoint.at(0)) {
		map<Ice::Long, Avatar1Ptr>::iterator it = _avatars_pager.find(jid->index);
		if (it != _avatars_pager.end()) {
			info = new OnlineInfo();
			info->userId = _id;
			info->endpoint = it->second->endpoint;
			info->index = it->first;
		}
	} else if ('T' == jid->endpoint.at(0)) {
		map<Ice::Long, Avatar1Ptr>::iterator it = _avatars_IM.find(jid->index);
		if (it != _avatars_IM.end()) {
			info = new OnlineInfo();
			info->userId = _id;
			info->endpoint = it->second->endpoint;
			info->index = it->first;
			info->show = it->second->show;
			info->ver = it->second->ver;
			info->ext = it->second->ext;
		}
	} else if ("web" == jid->endpoint) {
		map<Ice::Long, Avatar1Ptr>::iterator it = _avatars_web.begin();
		if (it != _avatars_web.end()) {
			info = new OnlineInfo();
			info->userId = _id;
			info->endpoint = it->second->endpoint;
			info->index = it->first;
		}
	}
	return info;
}
/*
 int User::next() {
 return _index++;
 }
 */
void User::_getJidSeq(map<Ice::Long, Avatar1Ptr> &map, JidSeq &seq) {
	for (Iterator it = map.begin(); it != map.end(); ++it) {
		JidPtr j = new Jid();
		j->userId = _id;
		j->endpoint = it->second->endpoint;
		j->index = it->first;
		seq.push_back(j);
	}
}

bool User::_remove(map<Ice::Long, Avatar1Ptr> &map, const string& endpoint,
		JidSeq &seq) {
	//MCE_DEBUG("[User::_remove] "<<_id<<" "<<endpoint);
	for (Iterator it = map.begin(); it != map.end();) {

		if (it->second->endpoint != endpoint) {
			++it;
			continue;
		}
		JidPtr j = new Jid();
		j->userId = _id;
		j->index = it->first;
		j->endpoint = it->second->endpoint;
		seq.push_back(j);
		map.erase(it++);
	}
	//MCE_DEBUG("[User::_remove end] "<<_id<<" "<<endpoint);
	return map.empty();
}

void User::_getAsOnlineInfoSeq(map<Ice::Long, Avatar1Ptr> &map,
		OnlineInfoSeq &seq) {
	for (Iterator it = map.begin(); it != map.end(); ++it) {
		OnlineInfoPtr info = new OnlineInfo();
		info->userId = _id;
		info->endpoint = it->second->endpoint;
		info->index = it->first;
		info->show = it->second->show;
		info->ver = it->second->ver;
		info->ext = it->second->ext;
		//info->stamp = it->second->timestamp;
		seq.push_back(info);
	}
}

int User::getOnlineDuration() {
	if (_avatars_IM.empty()) {
		_duration = 0;
		return 0;
	}
	if (!_duration) {
		return 0;
	}

	int duration = time(NULL) - _duration;
	_duration = time(NULL);
	return duration;

}

bool User::IMAvatarEmpty() {
	return _avatars_IM.empty();
}
//---------------------------------------------------------------------------
//UserPool
bool UserPool::add(const JidPtr& jid, const string& show, const string& ver,
		const string& ext) {

	IceUtil::RWRecMutex::WLock lock(_mutex);

	Iterator it = _users.find(jid->userId);
	if (it == _users.end()) {
		UserPtr u = new User(jid->userId);
		bool success = u->add(jid, show, ver, ext);
		if (!success) {
			return false;
		}
		_users[jid->userId] = u;
		return true;
	}

	return it->second->add(jid, show, ver, ext);
}

void UserPool::set(const JidPtr& jid, const string& show, const string& ver,
		const string& ext) {
	IceUtil::RWRecMutex::WLock lock(_mutex);

	Iterator it = _users.find(jid->userId);
	if (it == _users.end())
		return;

	it->second->set(jid, show, ver, ext);
}
bool UserPool::has(const JidPtr& jid) {
	IceUtil::RWRecMutex::RLock lock(_mutex);

	Iterator it = _users.find(jid->userId);
	if (it == _users.end())
		return false;

	return it->second->has(jid);
}
JidSeq UserPool::get(int userId) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	Iterator it = _users.find(userId);
	if (it == _users.end())
		return JidSeq();
	JidSeq seq;
	it->second->getJidSeq(seq);
	return seq;
}

JidSeq UserPool::get(MyUtil::IntSeq ids) {
	IceUtil::RWRecMutex::RLock lock(_mutex);

	JidSeq seq;
	for (size_t i = 0; i < ids.size(); ++i) {
		Iterator it = _users.find(ids.at(i));
		if (it != _users.end()) {
			it->second->getJidSeq(seq);
		}
	}
	return seq;

}

OnlineInfoSeq UserPool::getAsOnlineInfoSeq(int userId) {
	IceUtil::RWRecMutex::RLock lock(_mutex);

	OnlineInfoSeq seq;

	Iterator it = _users.find(userId);
	if (it != _users.end()) {
		it->second->getAsOnlineInfoSeq(seq);
	}
	return seq;
}

OnlineInfoPtr UserPool::getAsOnlineInfo(const JidPtr& jid) {
	IceUtil::RWRecMutex::RLock lock(_mutex);

	Iterator it = _users.find(jid->userId);
	if (it != _users.end()) {
		return it->second->getAsOnlineInfo(jid);
	}
	return 0;
}

void UserPool::remove(const JidPtr& jid) {
	IceUtil::RWRecMutex::WLock lock(_mutex);

	Iterator it = _users.find(jid->userId);
	if (it == _users.end()) {
		return;
	}

	bool empty = false;

	int dur = it->second->getOnlineDuration();
	if (dur !=0) {
		map<int, int>::iterator tmpit = _offlineUsersMap.find(jid->userId);
		if (tmpit == _offlineUsersMap.end())
			_offlineUsersMap[jid->userId] = dur;
		else {
			tmpit->second += dur;
		}
	}
	empty = it->second->remove(jid);
	if (empty) {
		_users.erase(it);
	}
}

JidSeq UserPool::remove(const string& endpoint) {
	IceUtil::RWRecMutex::WLock lock(_mutex);

	JidSeq seq;
	for (Iterator it = _users.begin(); it != _users.end();) {

		int dur = it->second->getOnlineDuration();
		bool empty = false;

		if (dur !=0) {
			map<int, int>::iterator tmpit = _offlineUsersMap.find(it->first);
			if (tmpit == _offlineUsersMap.end())
				_offlineUsersMap[it->first] = dur;
			else {
				tmpit->second += dur;
			}
		}
		empty = it->second->remove(endpoint, seq);

		if (empty) {
			MCE_DEBUG("[UserPool::remove] erase user "<<it->first);
			_users.erase(it++);
		} else {
			++it;
		}
	}

	return seq;
}

int UserPool::size(bool dedup) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	if (dedup) {
		return _users.size();
	} else {
		int size = 0;
		for (Iterator it = _users.begin(); it != _users.end(); ++it) {
			size += it->second->size();
		}
		return size;
	}
}

void UserPool::getOnlineDurationInfo(map<int, int> &onlineDurationMap) {
	IceUtil::RWRecMutex::RLock lock(_mutex);

	//	OnlineDurationMap.insert(_offlineUsersMap.begin(), _offlineUsersMap.end());
	//	_offlineUsersMap.clear();

	_offlineUsersMap.swap(onlineDurationMap);
	Iterator uit = _users.begin();
	for (; uit!=_users.end(); ++uit) {
		int dur = uit->second->getOnlineDuration();
		if (dur != 0) {
			map<int, int>::iterator it = onlineDurationMap.find(uit->first);
			if (it == onlineDurationMap.end())
				onlineDurationMap[uit->first] = dur;
			else
				it->second += dur;
		}
	}
}

//--------------------------------------------------------------------------------
WUserEvictor::WUserEvictor() {
	duration(60*1); // web online
	start(131072).detach();
}

WUserEvictor::~WUserEvictor() {

}

void WUserEvictor::push(int userId) {
	//add(userId, userId);
	IceUtil::Mutex::Lock lock(_mutex);
	//cout << "add obj: " << id.name << endl;
	Entry entry;
	entry.timestamp = time(NULL);
	entry.obj = userId;
	while (true) {
		pair<EvictorMap::iterator, bool> result = _map.insert(make_pair(userId,
				entry));
		if (result.second) {
			hook(result.first->second, result.first);
			//              entry->pos = _queue.insert(_queue.begin(), result.first);
			break;
		}
		EvictorMap::iterator pos = _map.find(userId);
		if (pos != _map.end()) {
			_queue.erase(pos->second.pos);
			_map.erase(pos);
		}
	};
}

void WUserEvictor::pop(MyUtil::IntSeq& userIds) {
	IceUtil::Mutex::Lock lock(_mutex);
	// If the evictor queue has grown larger than the limit,
	// look at the excess elements to see whether any of them can be evicted.
	//MCE_INFO("_queue.size:"<<_queue.size());
	if (_duration > 0) {
		time_t lastTime = time(NULL) - _duration;
		while (!_queue.empty()) {
			EvictorQueue::reverse_iterator p = _queue.rbegin();
			//evict((*p)->second->prx);
			EvictorMap::iterator pos = *p;
			if (!pos->second.expired(lastTime)) {
				break;
			}

			userIds.push_back(pos->second.obj);
			_queue.erase(pos->second.pos);
			_map.erase(pos);
		}
	}
	//cout << "evict done" << endl;
}

void WUserEvictor::run() {
	while (true) {
		MyUtil::IntSeq userIds;
		pop(userIds);
		//	MCE_DEBUG("call WUserEvictor::run() pop size:"<< userIds.size());
		if (!userIds.empty()) {
			JidSeq jids;
			for (MyUtil::IntSeq::iterator it = userIds.begin(); it
					!= userIds.end(); ++it) {
				JidPtr j = new Jid;
				j->userId = *it;
				j->index = 1;
				j->endpoint = "web";
				if (PresenceManagerI::instance().has(j)) {
					PresenceManagerI::instance().unbind(j);
					jids.push_back(j);
				}
			}
			try {
				MCE_DEBUG("WUserEvictor::run --> do offlineNotify size:"<<jids.size());
				//PresenceMsgHandler::instance().offlineNotify(jids);
			} catch (Ice::Exception& e) {
				MCE_WARN("WUserEvictor::run -> offlineNotify -> " << e);
			} catch (...) {
				MCE_WARN("WUserEvictor::run -> offlineNotify -> ...");
			}
		}
		sleep(5);
	}
}

//--------------------------------------------------------------------------------
PresenceManagerI::PresenceManagerI() {
	_pool.resize(USER_POOL_SIZ);
	for (size_t i = 0; i < USER_POOL_SIZ; ++i) {
		_pool[i] = new UserPool();
	}
	TaskManager::instance().scheduleRepeated(new StatOnlineSizeTask());
	TaskManager::instance().scheduleRepeated(new OnlineDurationStatTask());
}

PresenceManagerI::~PresenceManagerI() {

}

UserPoolPtr PresenceManagerI::getPool(int uid) {

	if (uid>=0) {
		return PresenceManagerI::instance()._pool[uid % USER_POOL_SIZ];
	} else {
		MCE_WARN("call getPool and userid:"<<uid<<" is < 0");
		return 0;
	}
}

void PresenceManagerI::bind(const JidPtr& jid, const Ice::Current&) {
	//MCE_INFO("call bind --> userid:"<<jid->userId<<"  endpoint:"<<jid->endpoint);

	UserPoolPtr pool = PresenceManagerI::instance().getPool(jid->userId);
	if (pool) {
		if (jid->endpoint == "web") {
			WUserEvictor::instance().push(jid->userId);
			if (pool->has(jid)) {
				return ;
			}
		}
		pool->add(jid, "", "", "");
	}
	//return false;

}

void PresenceManagerI::unbind(const JidPtr& jid, const Ice::Current&) {
	MCE_DEBUG("PresenceManagerI::unbind --> jid index:"<< jid->index);
	UserPoolPtr pool = PresenceManagerI::instance().getPool(jid->userId);
	if (pool) {
		pool->remove(jid);
	}

}

void PresenceManagerI::presence(const JidPtr& jid, const string& show,
		const string& ver, const string& ext, const Ice::Current&) {

	MCE_DEBUG("call PresenceManagerI::presence   jid="<<jid->index
			<< " and show="<<show);

	UserPoolPtr pool = PresenceManagerI::instance().getPool(jid->userId);
	if (pool) {
		pool->set(jid, show, ver, ext);
	}

}

JidSeqMap PresenceManagerI::getIMOnlineUserJidSeqMap(const MyUtil::IntSeq& ids,
		const Ice::Current&) {

	JidSeqMap res;
	for (size_t i = 0; i < ids.size(); ++i) {
		if (ids.at(i) < 0)
			continue;

		JidSeq seq = PresenceManagerI::instance().getPool(ids.at(i))->get(ids.at(i));
		if (!seq.empty()) {
			res[ids.at(i)] = seq;
		}
	}

	return res;

}
JidSeq PresenceManagerI::getOnlineUserJidSeq(Ice::Int id, const Ice::Current&) {

	UserPoolPtr pool = PresenceManagerI::instance().getPool(id);
	if (pool) {
		return pool->get(id);
	}
	return JidSeq();
}

OnlineInfoMap PresenceManagerI::filterOnlineUsers(const MyUtil::IntSeq& ids,
		const Ice::Current&) {

	OnlineInfoMap res;

	for (size_t i = 0; i < ids.size(); ++i) {
		if (ids.at(i) < 0)
			continue;

		OnlineInfoSeq seq = PresenceManagerI::instance().getPool(ids.at(i))->getAsOnlineInfoSeq(ids.at(i));
		if (!seq.empty()) {
			res[ids.at(i)] = seq;
		}
	}

	return res;
}

OnlineInfoPtr PresenceManagerI::filterOnlineUser(const JidPtr& jid,
		const Ice::Current&) {
	return PresenceManagerI::instance().getPool(jid->userId)->getAsOnlineInfo(jid);
}

OnlineInfoSeq PresenceManagerI::getOnlineUsersInfo(const JidSeq& jids,
		const Ice::Current&) {
	OnlineInfoSeq seq;
	for (size_t i = 0; i < jids.size(); ++i) {
		seq.push_back(filterOnlineUser(jids.at(i)));
	}
	return seq;
}

bool PresenceManagerI::has(const JidPtr& jid, const Ice::Current&) {
	UserPoolPtr pool = PresenceManagerI::instance().getPool(jid->userId);
	if (pool) {
		return pool->has(jid);
	} else
		return false;

}

void PresenceManagerI::closeProxyServer(const string& endpoint,
		const Ice::Current&) {
	//NotifyTaskManager::instance().addNotify(new Notify<string,CloseProxyNotifyHandler>(endpoint));
	MCE_INFO("call closeProxyServer Task  -->endpoint:"<<endpoint);
	MyUtil::TaskManager::instance().execute(new ResetTalkProxyTask(endpoint));
}

//added by yuanfei
void PresenceManagerI::getOnlineDurationInfo(map<int, int> &onlineDurationMap) {
	vector<UserPoolPtr>::iterator it = _pool.begin();
	for (; it != _pool.end(); ++it) {
		map<int,int> tmp;
		(*it)->getOnlineDurationInfo(tmp);
		onlineDurationMap.insert(tmp.begin(), tmp.end());
	}
}

//----------------------------------------------------
void PresenceManagerI::ResetTalkProxyTask::handle() {

	MCE_DEBUG("[ResetTalkProxyTask::handle] in handle");
	JidSeq seq;
	for (size_t i = 0; i < USER_POOL_SIZ; ++i) {
		JidSeq tmp = PresenceManagerI::instance().getPool(i)->remove(_endpoint);
		seq.insert(seq.end(), tmp.begin(), tmp.end());
	}
	//PresenceMsgHandlerAdapter::instance().offlineNotify(seq);
}

//----------------------------------------------------
void PresenceManagerI::StatOnlineSizeTask::handle() {

	//TaskManager::instance().schedule(this);
	int sizeTotal = 0;
	int sizeDedup = 0;
	for (size_t i = 0; i < USER_POOL_SIZ; ++i) {
		sizeTotal += PresenceManagerI::instance().getPool(i)->size(false);
		sizeDedup += PresenceManagerI::instance().getPool(i)->size();
	}

	MCE_INFO("STAT_ONLINE_SIZE -->  TOTAL_ONLINE_SIZE:"<<sizeTotal
			<<"    DEDUP_ONLINE_SIZE:"<<sizeDedup);

	ostringstream sql;
	sql
			<< "insert into xntalk_snap (name, time,value) values ('total_online_size',from_unixtime("
			<<::time(NULL)<<"),"<<sizeTotal
	<<"),('dedup_online_size',from_unixtime("<<::time(NULL)<<"),"
	<<sizeDedup <<")";

	ConnectionHolder conn("im_stat", CDbWServer);
	mysqlpp::Query query = conn.query();
	query.exec(sql.str());

}

//----------------------------------------------------
void PresenceManagerI::OnlineDurationStatTask::handle() {
	//TaskManager::instance().schedule(this);

	map<int, int> OnlineDurationMap;
	PresenceManagerI::instance().getOnlineDurationInfo(OnlineDurationMap);
	int size = OnlineDurationMap.size();
	MCE_INFO("write to database size="<<size);
	if (OnlineDurationMap.empty())
		return;
/*
	ostringstream sql1;

	int i = 0;
	for (map<int, int>::iterator it = OnlineDurationMap.begin(); it
			!= OnlineDurationMap.end(); ++it) {
		if (i == 0) {
			sql1.str("");
			sql1<< "insert into online_per_day(userid, date ,duration) values";
		} else {
			sql1<<",";
		}
		++i;
		sql1<<"("<<it->first<<",'"<<Date(::time(NULL)).str("%Y-%m-%d")<<"',"<<it->second<<") ";
		if (i % 1000 == 0) {
			sql1
					<<"on duplicate key update duration = duration + values(duration)";
			ConnectionHolder conn("im_stat", CDbWServer);
			mysqlpp::Query query = conn.query();
			query.exec(sql1.str());
			i = 0;
		}
	}
	if (i) {
		sql1 <<"on duplicate key update duration = duration + values(duration)";
		ConnectionHolder conn("im_stat", CDbWServer);
		mysqlpp::Query query = conn.query();
		query.exec(sql1.str());
		i = 0;
	}
*/
}
//----------------------------------------------------

