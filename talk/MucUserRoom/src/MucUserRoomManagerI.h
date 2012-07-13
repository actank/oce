#ifndef __MUCUSERROOM_H__
#define __MUCUSERROOM_H__
#include <map>
#include <string>
#include <vector>
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>
#include "MucUserRoom.h"
#include "Singleton.h"
#include "UserPool.h"

namespace com{
namespace xiaonei{
namespace talk{
namespace muc{

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;

const unsigned int POOLSIZE = 1009;
const string TEMPORARY = "conference.talk.xiaonei.com";

class MucUserRoomManagerI : public MucUserRoomManager, public Singleton<MucUserRoomManagerI>{
public:
  void Init();
	virtual MucRoomIdSeq GetUserRooms(int userid, const string& domain, const Ice::Current& = Ice::Current());
	virtual bool AddRoom(int userid, const MucRoomIdPtr& roomid, const MucMemberPtr& member, const Ice::Current& = Ice::Current());
	virtual bool RemoveRoom(int userid, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());

	virtual MucMemberPtr GetMemberPermision(int userid, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
	virtual MucMemberMap GetMemberPermisionBySeq(const MyUtil::IntSeq& userids, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
	
	
	virtual MucRoomMemberSeq GetUserRoomAndMember(int userid, const Ice::Current& = Ice::Current());
	virtual void SetUserPermision(int targetuser, const MucRoomIdPtr& roomid, Permisions permision, const Ice::Current& = Ice::Current());
	//-----------------------------------------------------------PUSH-----------------------------------------
	virtual void UserOnline(const JidPtr& onlinejid, const Ice::Current& = Ice::Current());
  virtual void UserOffline(const JidPtr& offlinejid, const Ice::Current& = Ice::Current());
  virtual void EnterRoomForTemporary(const MucActiveUserPtr& enteruser, const Ice::Current& = Ice::Current());
  virtual void AwayRoomForTemporary(const JidPtr& awayjid, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  //virtual void UserOnlineBySeq(const JidSeq& onlinejids, const Ice::Current& = Ice::Current());
protected:
  void ProcessOnline(const JidPtr& jid);
  UserPoolPtr GetPool(int userid);
  MucMemberPtr GetUserRoomPermision(int userid, const MucRoomIdPtr& roomid);
  MucMemberMap GetUserRoomPermision(const MyUtil::IntSeq& userids, const MucRoomIdPtr& roomid);
private:
	vector<UserPoolPtr> _pool;
};


class TimeStat{
	timeval _begin, _end;
	public:
	TimeStat(){
		reset();
	}
	void reset(){
		gettimeofday(&_begin, NULL);
	}
	float getTime(){
		gettimeofday(&_end, NULL);
		float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
			-_begin.tv_usec;
		timeuse/=1000;
		return timeuse;
	}
};


};
};
};
};

#endif
