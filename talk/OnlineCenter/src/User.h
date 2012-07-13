#ifndef USER_H_
#define USER_H_

#include <map>
#include <set>
#include <list>
#include <IceUtil/Shared.h>
#include "TalkCommon.h"
#include <IceUtil/RWRecMutex.h>
#include "TalkLogic.h"
#include "OnlineCenter.h"
using namespace std;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk;
using namespace talk::online;

enum OnlineType {
	UNKNOWTYPE = 0,
	WEB_ONLINE = 1,
	WP_ONLINE = 2,
	IM_ONLINE = 4,
	PHONE_ONLINE = 8
};

inline OnlineType onlineTypeFromJid(const JidPtr& jid){
	if ('W' == jid->endpoint.at(0)) {
		return WP_ONLINE;
	} else if ('T' == jid->endpoint.at(0)) {
		return IM_ONLINE;
	} else if ('w' == jid->endpoint.at(0)){
		return WEB_ONLINE;
	} else if ('P' == jid->endpoint.at(0)) {
		return PHONE_ONLINE;
	} else{
		return UNKNOWTYPE;
	}
}

class User : public IceUtil::Shared {
public:
	User() :
		_onlineStat(ONLINE),_onlineType(0),_duration(0),_timestamp(0) {
		;
	}

	int onlineType() {
		if(_onlineStat==INVISIABLE){
			return UNKNOWTYPE;
		}
		return _onlineType;
	}
	OnlineStat onlineStat(){
		return _onlineStat;
	}
	bool empty() {
		return _avatars.empty();
	}
	bool add(const list<AvatarPtr>::iterator& pos);
	bool remove(Ice::Long index);
	bool remove(const string& endpoint, JidSeq &seq, NotifyType& type);
	bool has(Ice::Long index){
		return _avatars.find(index)!=_avatars.end();
	}
	int size() {
		return _avatars.size();
	}
	JidSeq getJidSeq(int onlineType);
	AvatarSeq getAvatars(int onlineType);
	AvatarPtr getAvatar(Ice::Long index);
	void scoreCheck(int index);
protected:
	void updateOnlineStatAndOnlineType(int userId);
private:
	map<Ice::Long, list<AvatarPtr>::iterator > _avatars;
	OnlineStat _onlineStat;
	int _onlineType;
	int _duration;
	int _timestamp;
};
typedef IceUtil::Handle<User> UserPtr;

class UserPool : public IceUtil::Shared {
public:
	UserPool() : _rLockCount(0),_wLockCount(0){}
	typedef map<int, UserPtr>::iterator Iterator;

	bool add(const AvatarPtr& avatar);
	bool remove(const JidPtr& jid);
	JidSeq remove(const string& endpoint, map<int, JidSeq>& userJids, map<int, NotifyType>& notifyTypes);

	bool has(const JidPtr& jid);
	bool has(int userId);

	
	UserOnlineTypeSeq getOnlineUsersByServerIndex(int serverIndex);
	void scoreCheck(int index);

	UserOnlineTypeSeq getOnlineUsers();
	UserOnlineTypeSeq getUsersOnlineTypes();
	//获取用户的OnlineBitmapValues，UserOnlineType这个数据结构里面的 -> onlineType 的值高四位赋值了OnlineStat
	UserOnlineTypeSeq getUsersBitmapValues();

	AvatarSeq getUserAvatars(int userId, int onlineType );
	AvatarPtr getAvatar(const JidPtr& jid);
	JidSeq getUserJids(int userId, int onlineType);
	
	int getUserOnlineType(int userId);
	OnlineStat getUserOnlineStat(int userId);
	//获取用户的OnlineBitmapValues，UserOnlineType这个数据结构里面的 -> onlineType 的值高四位赋值了OnlineStat
	int getUserBitmapValue(int userId);
	//JidSeqMap getUsersJids(const MyUtil::IntSeq& ids);
	//AvatarSeq getAvatars(const JidSeq& jids);
	int size(bool dedup = true);
	UserPtr getUser(int userId);

	void erase(const list<AvatarPtr>::iterator& pos){
		//no lock
		_avatars[onlineTypeFromJid((*pos)->jid)].erase(pos);
	}
	pair<int, int> getLockCount(){
		pair<int, int> p;
		p.first = _rLockCount;
		p.second = _wLockCount;
		_rLockCount = 0;
		_wLockCount = 0;
		return p;
	}
	AvatarSeq getOldestAvatars(int beg, int limit, int type);
private:
	IceUtil::RWRecMutex _mutex;
	map<int, UserPtr> _users;

	map<int, list<AvatarPtr> > _avatars;
	//the online-duration of the offline user
	map<int, int> _offlineUsersMap;
	int _rLockCount;
	int _wLockCount;
};
typedef IceUtil::Handle<UserPool> UserPoolPtr;

#endif
