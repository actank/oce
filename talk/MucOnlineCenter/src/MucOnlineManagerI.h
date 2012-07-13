#ifndef __MUCONLINEMANAGER_H__
#define __MUCONLINEMANAGER_H__
#include <map>
#include <string>
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>
#include "MucOnlineCenter.h"
#include "Singleton.h"
#include "RoomPool.h"
namespace com{
namespace xiaonei{
namespace talk{
namespace muc{
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;
const int POOLSIZE = 23;
//接口注释 详见 MucOnlieCenter.ice
class MucOnlineManagerI : public MucOnlineCenterManager, public Singleton<MucOnlineManagerI>{
public:
	MucOnlineManagerI();
	void Init();
	RoomPoolPtr GetPool(const MucRoomIdPtr& roomid);
	virtual void ChatSetingChange(int userid, const MucRoomIdPtr& roomid, int set, const Ice::Current& = Ice::Current());
	virtual void EnterRoom(const MucActiveUserPtr& activeuser, const Ice::Current& = Ice::Current());
  virtual void UserOnline(const MucActiveUserPtr& activeuser, const Ice::Current& = Ice::Current());
  virtual void UserOnlineBySeq(const MucActiveUserSeq& onlineuserseq, const Ice::Current& = Ice::Current());
  virtual void AwayRoom(const JidPtr& opuser, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  virtual void SendTalkMessage(const JidPtr& opuser, const MucRoomIdPtr& roomid, const string& msg, const Ice::Current& = Ice::Current());
  virtual int GetUserCount(const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  virtual void SetPermision(int opuser, Permisions permision, int targetuser, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  //FOR PAGER ONLY
  virtual MucUserIdentitySeq GetActiveUsers(const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
	virtual JidSeq GetActiveJids(const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  //FOR IM ONLY
  virtual bool SetPermisionForIM(int opuser, Permisions permision, int targetuser, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  virtual MucActiveUserSeq GetUserByPermision(const MucRoomIdPtr& roomid, Permisions permision, const Ice::Current& = Ice::Current());
	virtual void SendPrivateMsg(const JidPtr& sender, const MucUserIdentityPtr& recidentity, const string& msg, const Ice::Current& = Ice::Current());
private:
	vector<RoomPoolPtr> _pool;//按照房间分桶
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
