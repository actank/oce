#ifndef __MUCTALKROOM_H__
#define __MUCTALKROOM_H__
#include <map>
#include <string>
#include <vector>
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>
#include "MucTalkRoom.h"
#include "Singleton.h"
#include "RoomPool.h"
namespace com{
namespace xiaonei{
namespace talk{
namespace muc{
using namespace MyUtil;
const unsigned int POOLSIZE = 23;
class MucTalkRoomManagerI : public MucTalkRoomManager, public Singleton<MucTalkRoomManagerI>{
public:
	void Init();
	RoomPoolPtr GetPool(const MucRoomIdPtr& roomid);
  
	virtual void CreateRoom(const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  virtual void RemoveRoom(const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  virtual void AddRoomMember(int userid, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  virtual void RemoveRoomMember(int userid, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());

  virtual IntSeq GetRoomMember(const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  virtual MucActiveUserSeq GetRoomActiveUserSeq(const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());

	//virtual void PageNotifyOnline(int onlineuser, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
	//virtual void PageNotifyOffline(int offlineuser, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());

private:
	vector <RoomPoolPtr> _pool;
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
