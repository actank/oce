#include "User.h"
#include "IceLogger.h"
#include "OnlineManagerI.h"
#include "BuddyList.h"
#include "XmppTools.h"
#include "OnlineCenterAdapter.h"
#include "ScoreLoginRecordAdapter.h"

using namespace talk::online::adapter;
using namespace com::xiaonei::talk::util;
using namespace talk::online;
using namespace xce::userscore;
bool User::add(const list<AvatarPtr>::iterator& pos) {
	//MCE_WARN("User::add --> " << jidToString(avatar->jid));
	OnlineType onlineType = UNKNOWTYPE;
	if ((*pos)->jid->endpoint.empty() || "web" == (*pos)->jid->endpoint) {
		return false;
	}
	if ('W' == (*pos)->jid->endpoint.at(0)) {
		onlineType = WP_ONLINE;
	} else if ('T' == (*pos)->jid->endpoint.at(0)) {
		onlineType = IM_ONLINE;
	} else if ('P' == (*pos)->jid->endpoint.at(0)) {
		onlineType = PHONE_ONLINE;
	}else {
		MCE_WARN("BuddyInfo::join --> endpoint "<<(*pos)->jid->endpoint
				<<" is incorrect");
		return false;
	}
	map<Ice::Long, list<AvatarPtr>::iterator >::iterator it = _avatars.find((*pos)->jid->index);
	if (it != _avatars.end()) {
		//MCE_WARN("BuddyInfo::join --> the same index had existed");
		/*it->second->stat = avatar->stat;
		it->second->jid = avatar->jid;
		if (!avatar->ver.empty()) {
			it->second->ver = avatar->ver;
		}
		if (!avatar->ext.empty()) {
			it->second->ext = avatar->ext;
		}*/
		if ((*pos)->ver.empty()) {
			(*pos)->ver = (*(it->second))->ver;
		}
		if ((*pos)->ext.empty()) {
			(*pos)->ext = (*(it->second))->ext;
		}
		(*pos)->stamp = (*(it->second))->stamp;
		OnlineManagerI::instance().getPool((*pos)->jid->userId)->erase(it->second);
		it->second = pos;
		updateOnlineStatAndOnlineType((*pos)->jid->userId);
		return true;
	}
	_avatars[(*pos)->jid->index] = pos;
	_onlineType |= onlineType;
	/*
	 if (avatar->stat < _onlineStat) {
	 _onlineStat = avatar->stat;
	 }*/
	if (_timestamp == 0 && onlineType==IM_ONLINE) {
		_timestamp = time(NULL);
	}
	updateOnlineStatAndOnlineType((*pos)->jid->userId);
	return true;
}

void User::updateOnlineStatAndOnlineType(int userId) {
	//MCE_WARN("User::_updateOnlineStatAndOnlineType --> " << userId);
	int onlineType = UNKNOWTYPE;
	_onlineStat = ONLINE;
	bool hasIM = false;
	map<Ice::Long, list<AvatarPtr>::iterator >::iterator it = _avatars.begin();
	for (; it!=_avatars.end(); ++it) {
		char tmp = (*(it->second))->jid->endpoint.at(0);
		switch (tmp) {
		case 'W':
			onlineType |= WP_ONLINE;
			break;
		case 'T':
			onlineType |= IM_ONLINE;
			hasIM = true;
			break;
		case 'P':
			onlineType |= PHONE_ONLINE;
			break;
		default:
			break;
		}
		if ((*(it->second))->stat < _onlineStat) {
			_onlineStat = (*(it->second))->stat;
		}
	}
	_onlineType = onlineType;
	if (!hasIM && _timestamp!=0) {
		OnlineDurationStatTask::instance().push(userId, time(NULL) - _timestamp);
		_timestamp = 0;
	}
}

bool User::remove(Ice::Long index) {
	//MCE_WARN("User::remove --> " << index);
	map<Ice::Long, list<AvatarPtr>::iterator >::iterator it = _avatars.find(index);
	if (it == _avatars.end()) {
		//MCE_WARN("BuddyInfo::leave --> the index does not exist");
		return _avatars.empty();
	}
	int userId = (*(it->second))->jid->userId;
	OnlineManagerI::instance().getPool((*(it->second))->jid->userId)->erase(it->second);
	_avatars.erase(it);
	if (!_avatars.empty()) {
		updateOnlineStatAndOnlineType(userId);
	}
	return _avatars.empty();
}

bool User::remove(const string& endpoint, JidSeq &seq, NotifyType& type) {
	//MCE_WARN("User::remove --> " << endpoint);
	map<Ice::Long, list<AvatarPtr>::iterator >::iterator it = _avatars.begin();
	for (; it != _avatars.end();) {
		if ((*(it->second))->jid->endpoint != endpoint) {
			++it;
			continue;
		}
		seq.push_back((*(it->second))->jid);
		OnlineManagerI::instance().getPool((*(it->second))->jid->userId)->erase(it->second);
		_avatars.erase(it++);
	}
	OnlineStat oldStat = _onlineStat;
	if (!_avatars.empty()) {
		updateOnlineStatAndOnlineType((*(_avatars.begin()->second))->jid->userId);
	}
	if (!seq.empty()) {
		OnlineStat newStat = _onlineStat;
		if (oldStat==INVISIABLE && newStat==INVISIABLE) {
			type = AvatarToItsOtherAvatars;
		} else if (oldStat!=INVISIABLE && newStat!=INVISIABLE) {
			type = AvatarToAll;
		} else {
			type = AllAvatarsToBuddies;
		}
	}
	return _avatars.empty();
}

JidSeq User::getJidSeq(int onlineType) {
	//MCE_WARN("User::getJidSeq --> " << onlineType);
	com::xiaonei::talk::common::JidSeq seq;
	map<Ice::Long, list<AvatarPtr>::iterator >::iterator it = _avatars.begin();
	for(; it!=_avatars.end(); ++it){
		//pager在线和手机在线的index分别为0,2 类型都是WP_ONLINE
		if(it->first == 0){
			if(onlineType&WP_ONLINE){
				seq.push_back((*(it->second))->jid);
			}
		}else if(it->first == 2){
			if(onlineType&PHONE_ONLINE){
				seq.push_back((*(it->second))->jid);
			}
		}else{
			if(onlineType&IM_ONLINE){
				seq.push_back((*(it->second))->jid);
			}else{
				break;
			}
		}
	}
	return seq;
/*	com::xiaonei::talk::common::JidSeq seq;
	map<Ice::Long, list<AvatarPtr>::iterator >::iterator it = _avatars.begin();
	if (onlineType & WP_ONLINE && it != _avatars.end()) {
		if (it->first == 0) {
			seq.push_back((*(it->second))->jid);
			++it;
		}
	}

	if (onlineType & IM_ONLINE && it != _avatars.end()) {
		if(it->first == 0){
			++it;
		}
		for (; it != _avatars.end(); ++it) {
			seq.push_back((*(it->second))->jid);
		}
	}

	MCE_DEBUG("User::getJidSeq --> size " << seq.size());
	return seq;*/
}
//AvatarSeq       getAvatars();
AvatarSeq User::getAvatars(int onlineType) {
	//MCE_WARN("User::getAvatars --> " << onlineType);

	AvatarSeq seq;
	map<Ice::Long, list<AvatarPtr>::iterator >::iterator it = _avatars.begin();
	for(; it!=_avatars.end(); ++it){
		//pager在线和手机在线的index分别为0,2 类型都是WP_ONLINE
		if(it->first == 0){
			if(onlineType&WP_ONLINE){
				seq.push_back((*(it->second)));
			}
		}else if(it->first == 2){
			if(onlineType&PHONE_ONLINE){
				seq.push_back((*(it->second)));
			}
		}else{
			if(onlineType&IM_ONLINE){
				seq.push_back((*(it->second)));
			}else{
				break;
			}
		}
	}
	return seq;

/*	AvatarSeq seq;

	map<Ice::Long, list<AvatarPtr>::iterator >::iterator it = _avatars.begin();
	if (onlineType & WP_ONLINE && it != _avatars.end()) {
		if (it->first == 0) {
			seq.push_back((*(it->second)));
			++it;
		}
	}
	if (onlineType & IM_ONLINE && it != _avatars.end()) {
		if(it->first == 0){
			++it;
		}
		for (; it != _avatars.end(); ++it) {
			seq.push_back((*(it->second)));
		}
	}
	MCE_DEBUG("User::getAvatars --> size " << seq.size());
	return seq;*/
}
//AvatarPtr       getAvatar(Ice::Long index);
AvatarPtr User::getAvatar(Ice::Long index) {
	//MCE_WARN("User::getAvatar --> " << index);
	map<Ice::Long, list<AvatarPtr>::iterator >::iterator it = _avatars.find(index);
	if (it!=_avatars.end()) {
		return (*(it->second));
	}
	return 0;
}


void User::scoreCheck(int index){
        int today = Date(time(NULL)).day();
	map<Ice::Long, list<AvatarPtr>::iterator >::iterator avatarIt;
        for (avatarIt = _avatars.begin(); avatarIt != _avatars.end(); ++avatarIt){
		if('T' == (*(avatarIt->second))->jid->endpoint.at(0)){
			MCE_INFO("User::scoreCheck-->userid:" << (*(avatarIt->second))->jid->userId);
			time_t login_time = (*(avatarIt->second))->stamp;
			int login_day = Date(login_time).day();
			if(login_day != today){
				try{
					ScoreLoginRecordAdapter::instance().incLoginCount((*(avatarIt->second))->jid->userId,16);
					ScoreLoginRecordAdapter::instance().incLoginCount((*(avatarIt->second))->jid->userId,4);
				}
				catch(Ice::Exception& e){
					MCE_WARN("User::scoreCheck --> ice error" << e);
				}
				return;
			}
                }
        }
}


//------------------------------------------------------------------------


bool UserPool::add(const AvatarPtr& avatar) {
	IceUtil::RWRecMutex::WLock lock(_mutex);
	_wLockCount++;
	//MCE_WARN("UserPool::add --> " << jidToString(avatar->jid));
	OnlineType onlineType = onlineTypeFromJid(avatar->jid);
	_avatars[onlineType].push_back(avatar);

	Iterator it = _users.find(avatar->jid->userId);
	if (it == _users.end()) {
		UserPtr u = new User();
		bool success = u->add(--(_avatars[onlineType].end()));
		if (!success) {
			return false;
		}
		_users[avatar->jid->userId] = u;
		return true;
	}
	return it->second->add(--(_avatars[onlineType].end()));
}

bool UserPool::has(const JidPtr& jid) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	_rLockCount++;
	//MCE_WARN("UserPool::has --> " << jidToString(jid));
	Iterator it = _users.find(jid->userId);
	if (it == _users.end())
		return false;

	return it->second->has(jid->index);
}

bool UserPool::has(int userId) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	_rLockCount++;
	//MCE_WARN("UserPool::has --> " << userId);
	Iterator it = _users.find(userId);
	return it != _users.end();
}
/*
 JidSeq UserPool::getJidSeq(int userId) {
 IceUtil::RWRecMutex::RLock lock(_mutex);
 Iterator it = _users.find(userId);
 if (it == _users.end())
 return JidSeq();
 return it->second->getJidSeq();
 }*/

UserPtr UserPool::getUser(int userId) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	_rLockCount++;
	//MCE_WARN("UserPool::getUser --> " << userId);
	Iterator it = _users.find(userId);
	if (it == _users.end())
		return 0;
	return it->second;
}

bool UserPool::remove(const JidPtr& jid) {
	IceUtil::RWRecMutex::WLock lock(_mutex);
	_wLockCount++;
	//MCE_WARN("UserPool::remove --> " << jidToString(jid));
	Iterator it = _users.find(jid->userId);
	if (it == _users.end()) {
		//MCE_WARN("UserPool::remove --> the userId does not exist, "
		//		<< jid->userId);
		return true;
	}

	bool empty = it->second->remove(jid->index);
	if (empty) {
		MCE_DEBUG("UserPool::remove --> the user has no avatars, "
				<< jid->userId);
		_users.erase(it);
		return true;
	}
	return false;
}

JidSeq UserPool::remove(const string& endpoint, map<int, JidSeq>& userJids,
		map<int, NotifyType>& notifyTypes) {
	IceUtil::RWRecMutex::WLock lock(_mutex);
	_wLockCount++;
	//MCE_WARN("UserPool::remove --> " << endpoint);
	JidSeq seq;
	for (Iterator it = _users.begin(); it != _users.end();) {
		bool empty = it->second->remove(endpoint, userJids[it->first],
				notifyTypes[it->first]);
		if (empty) {
			MCE_DEBUG("[UserPool::remove] erase user "<<it->first);
			BuddyListPtr ptr = ServiceI::instance().findObject<BuddyListPtr>(OBS, it->first);
			if (ptr) {
				ptr->leave(it->first);
			}
			_users.erase(it++);
		} else {
			++it;
		}
	}
	return seq;
}

int UserPool::size(bool dedup) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	//_reqCount++;
	//MCE_WARN("UserPool::size --> " << dedup);
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
AvatarSeq UserPool::getUserAvatars(int userId, int onlineType) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	_rLockCount++;
	//MCE_WARN("UserPool::getUserAvatars --> " << userId);
	Iterator it = _users.find(userId);
	if (it == _users.end())
		return AvatarSeq();
	return it->second->getAvatars(onlineType);
}

AvatarPtr UserPool::getAvatar(const JidPtr& jid) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	_rLockCount++;
	//MCE_WARN("UserPool::getAvatar --> " << jidToString(jid));
	Iterator it = _users.find(jid->userId);
	if (it == _users.end())
		return 0;
	return it->second->getAvatar(jid->index);
}
/*
 AvatarSeq UserPool::getAvatars(const JidSeq& jids){
 IceUtil::RWRecMutex::RLock lock(_mutex);
 Iterator it = _users.find(jid->userId);
 if (it == _users.end())
 return 0;
 return it->second->getAvatar();
 }
 */
JidSeq UserPool::getUserJids(int userId, int onlineType) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	_rLockCount++;
	//MCE_WARN("UserPool::getUserJids --> " << userId);
	Iterator it = _users.find(userId);
	if (it == _users.end())
		return JidSeq();
	return it->second->getJidSeq(onlineType);
}

int UserPool::getUserOnlineType(int userId) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	_rLockCount++;
	//MCE_WARN("UserPool::getUserOnlineType --> " << userId);
	Iterator it = _users.find(userId);
	if (it == _users.end()) {
		return UNKNOWTYPE;
	}
	//if(userId % 10 == 2 || userId % 10 == 3){
	//	MCE_INFO("UserPool::getUserOnlineType --> " << userId << " " << it->second->onlineType());
	//}
	return it->second->onlineType();
}
OnlineStat UserPool::getUserOnlineStat(int userId) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	_rLockCount++;
	//MCE_WARN("UserPool::getUserOnlineStat --> " << userId);
	Iterator it = _users.find(userId);
	if (it == _users.end()) {
		return OFFLINE;
	}
	return it->second->onlineStat();
}

//add by guoqing.liu
//获取onlineBitmap的值 = 高四位 ： 在线状态(隐身，忙碌，离开等)    低四位 ： 在线类型(手机在线，pager在线，客户端在线)
int UserPool::getUserBitmapValue(int userId){
	IceUtil::RWRecMutex::RLock lock(_mutex);
	_rLockCount++;
	int online_stat = 0;
	int online_type = 0;
	Iterator it = _users.find(userId);
	if (it == _users.end()) {//用户不在，返回0
		return 0;
	}
	online_stat = it->second->onlineStat();
	online_type = it->second->onlineType();
	return (online_type | (online_stat << 4) );
}

UserOnlineTypeSeq UserPool::getOnlineUsersByServerIndex(int serverIndex){
	int cluster = OnlineCenterAdapter::instance().getCluster();
        IceUtil::RWRecMutex::RLock lock(_mutex);
        UserOnlineTypeSeq seq;
        Iterator it = _users.begin();
        for(; it!=_users.end(); ++it){
		if(it->first % cluster == serverIndex){
                	UserOnlineTypePtr  tmp = new UserOnlineType();
               		tmp->userId = it->first;
                	tmp->onlineType = it->second->onlineType();
                	seq.push_back(tmp);
		}
        }
        return seq;
}

void UserPool::scoreCheck(int index){
	int cluster = OnlineCenterAdapter::instance().getCluster();
        IceUtil::RWRecMutex::RLock lock(_mutex);
	map<int, UserPtr>::iterator userIt;
	for(userIt = _users.begin(); userIt != _users.end(); ++userIt){
		if(userIt->first % cluster == index){
			userIt->second->scoreCheck(index);
		}
	}
}


UserOnlineTypeSeq UserPool::getOnlineUsers(){
	IceUtil::RWRecMutex::RLock lock(_mutex);
	UserOnlineTypeSeq seq;
	Iterator it = _users.begin();
	for(; it!=_users.end(); ++it){
		UserOnlineTypePtr  tmp = new UserOnlineType();
		tmp->userId = it->first;
		tmp->onlineType = it->second->onlineStat();
		seq.push_back(tmp);
	}
	return seq;
}
UserOnlineTypeSeq UserPool::getUsersOnlineTypes(){
	IceUtil::RWRecMutex::RLock lock(_mutex);
	UserOnlineTypeSeq seq;
	Iterator it = _users.begin();
	for(; it!=_users.end(); ++it){
		UserOnlineTypePtr  tmp = new UserOnlineType();
		tmp->userId = it->first;
		tmp->onlineType = it->second->onlineType();
		//if(tmp->userId % 10 == 2 || tmp->userId % 10 == 3)
		//	MCE_INFO("UserPool::getUsersOnlineTypes --> " << tmp->userId << " " << tmp->onlineType);
		seq.push_back(tmp);
	}
	return seq;
}
UserOnlineTypeSeq UserPool::getUsersBitmapValues(){
	IceUtil::RWRecMutex::RLock lock(_mutex);
	UserOnlineTypeSeq seq;
	Iterator it = _users.begin();
	for(; it!=_users.end(); ++it){
		UserOnlineTypePtr  tmp = new UserOnlineType();
		tmp->userId = it->first;
		tmp->onlineType = (it->second->onlineType() | (it->second->onlineStat() << 4));
		seq.push_back(tmp);
	}
	return seq;
}

AvatarSeq UserPool::getOldestAvatars(int beg, int limit, int type){
	IceUtil::RWRecMutex::RLock lock(_mutex);

	AvatarSeq seq;
	int count = 0;
	map<int, list<AvatarPtr> >::iterator it = _avatars.find(type);
	if(it == _avatars.end()){
		return seq;
	}
	list<AvatarPtr>::iterator lIt = it->second.begin();
	for(; lIt!=it->second.end(); ++lIt){
		if(count >= beg && (count-beg)<limit){
			seq.push_back(*lIt);
		}else{
			break;
		}
		count++;
	}
	return seq;
}
