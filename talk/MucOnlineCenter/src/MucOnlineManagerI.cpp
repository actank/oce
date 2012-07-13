#include "MucOnlineManagerI.h"
#include "ServiceI.h"
#include <boost/lexical_cast.hpp>

using namespace com::xiaonei::talk::muc;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace std;

void MyUtil::initialize(){
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(new MucOnlineManagerI, service.createIdentity("M", ""));
}

MucOnlineManagerI::MucOnlineManagerI(){
	Init();
}
void MucOnlineManagerI::Init(){
	for(int i = 0; i < POOLSIZE; ++i){
		RoomPoolPtr rp = new RoomPool();
		_pool.push_back(rp);
	}
}


RoomPoolPtr MucOnlineManagerI::GetPool(const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);//按房间ID的组成找到其对应的桶  临时房间取首字母作为数字ID(TalkUtil/XmppTools.cpp)
	int index = irid % POOLSIZE;
	MCE_DEBUG("MucOnlineManagerI::GetPool --> get index = " << index);
	if(index >= 0 && index < POOLSIZE){
		return _pool[index];
	}
	else{
		MCE_WARN("MucOnlineManagerI::GetPool --> roomid = " << mucRoomIdToString(roomid)<< " get pool error");
		return 0;
	}
}


void MucOnlineManagerI::ChatSetingChange(int userid, const MucRoomIdPtr& roomid, int set, const Ice::Current& current){
	TimeStat ts;
	RoomPoolPtr pool = GetPool(roomid);
	if(pool){
		pool->ChatSetingChange(userid, roomid, set);
	}
	MCE_INFO("interface ChatSetingChange usetime : " << ts.getTime());
}
void MucOnlineManagerI::EnterRoom(const MucActiveUserPtr& activeuser, const Ice::Current& current){
	TimeStat ts;
	RoomPoolPtr pool = GetPool(activeuser->identity->roomid);
	if(pool){
		pool->EnterRoom(activeuser);
	}
	MCE_INFO("interface EnterRoom usetime : " << ts.getTime());
}

void MucOnlineManagerI::UserOnline(const MucActiveUserPtr& activeuser, const Ice::Current& current){
	TimeStat ts;
	RoomPoolPtr pool = GetPool(activeuser->identity->roomid);
	if(pool){
		pool->UserOnline(activeuser);
	}
	MCE_INFO("interface UserOnline usetime : " << ts.getTime());
}

void MucOnlineManagerI::UserOnlineBySeq(const MucActiveUserSeq& onlineuserseq, const Ice::Current& current){
	TimeStat ts;
  MucActiveUserSeq::const_iterator it = onlineuserseq.begin();
  for(; it != onlineuserseq.end(); ++it){
    MucActiveUserPtr activeuser = (*it);
    RoomPoolPtr pool = GetPool(activeuser->identity->roomid);
    if(pool){
      pool->UserOnline(activeuser);
    }
  }
	MCE_INFO("interface UserOnlineBySeq usetime : " << ts.getTime());
}


void MucOnlineManagerI::AwayRoom(const JidPtr& opuser, const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
	RoomPoolPtr pool = GetPool(roomid);
	if(pool){
		pool->AwayRoom(opuser, roomid);
	}
	MCE_INFO("interface AwayRoom usetime : " << ts.getTime());
}

void MucOnlineManagerI::SendTalkMessage(const JidPtr& opuser, const MucRoomIdPtr& roomid, const string& msg, const Ice::Current& current){
	TimeStat ts;
	RoomPoolPtr pool = GetPool(roomid);
	if(pool){
		pool->SendTalkMessage(opuser, roomid, msg);
  }
	MCE_INFO("interface SendTalkMessage usetime : " << ts.getTime());
}

int MucOnlineManagerI::GetUserCount(const MucRoomIdPtr& roomid, const Ice::Current& current){
	RoomPoolPtr pool = GetPool(roomid);
	if(pool){
		return pool->GetUserCount(roomid);
	}
	return 0;
}

void MucOnlineManagerI::SetPermision(int opuser, Permisions permision, int targetuser, const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
  RoomPoolPtr pool = GetPool(roomid);
	if(pool){
    pool->SetPermision(opuser, permision, targetuser, roomid);
  }
	MCE_INFO("interface SetPermision usetime : " << ts.getTime());
}

bool MucOnlineManagerI::SetPermisionForIM(int opuser, Permisions permision, int targetuser, const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
  bool ans = false;
  RoomPoolPtr pool = GetPool(roomid);
  if(pool){
    ans = pool->SetPermisionForIM(opuser, permision, targetuser, roomid);
  }
	MCE_INFO("interface SetPermisionForIM usetime : " << ts.getTime());
  return ans;
}


MucUserIdentitySeq MucOnlineManagerI::GetActiveUsers(const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
  MucUserIdentitySeq ans;
  RoomPoolPtr pool = GetPool(roomid);
  if(pool){
    ans = pool->GetActiveUsers(roomid);
  }
	MCE_INFO("interface GetActiveUsers usetime : " << ts.getTime());
  return ans;
}

JidSeq MucOnlineManagerI::GetActiveJids(const MucRoomIdPtr& roomid, const Ice::Current&){
	TimeStat ts;
  JidSeq ans;
  RoomPoolPtr pool = GetPool(roomid);
  if(pool){
    ans = pool->GetActiveJids(roomid);
  }
	MCE_INFO("interface GetActiveJids usetime : " << ts.getTime());
  return ans;
}

MucActiveUserSeq MucOnlineManagerI::GetUserByPermision(const MucRoomIdPtr& roomid, Permisions permision, const Ice::Current& current){
	TimeStat ts;
  MucActiveUserSeq ans;
  if(None == permision){
    return ans;
  }
  RoomPoolPtr pool = GetPool(roomid);
  if(pool){
    pool->GetUserByPermision(roomid, permision);
  }
	MCE_INFO("interface GetUserByPermision usetime : " << ts.getTime());
  return ans;
}



void MucOnlineManagerI::SendPrivateMsg(const JidPtr& sender, const MucUserIdentityPtr& recidentity, const string& msg, const Ice::Current& current){
	TimeStat ts;
	RoomPoolPtr pool = GetPool(recidentity->roomid);
	if(pool){
		pool->SendPrivateMsg(sender, recidentity, msg);
	}
	MCE_INFO("interface SendPrivateMsg usetime : " << ts.getTime());
}
