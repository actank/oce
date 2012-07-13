#include "MucTalkRoomManagerI.h"
#include "PageCacheAdapter.h"
#include "ServiceI.h"
#include <boost/lexical_cast.hpp>

using namespace com::xiaonei::talk::muc;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace std;
using namespace talk::http;

void MyUtil::initialize(){
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&MucTalkRoomManagerI::instance(), service.createIdentity("M", ""));
	MucTalkRoomManagerI::instance().Init();
}

void MucTalkRoomManagerI::Init(){
	for(size_t i = 0; i < POOLSIZE; ++i){
		RoomPoolPtr rmptr = new RoomPool();
		_pool.push_back(rmptr);
	}
}


RoomPoolPtr MucTalkRoomManagerI::GetPool(const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
	int index = irid % POOLSIZE;
	MCE_DEBUG("MucTalkRoomManagerI::GetPool --> index = " << index);
	if(index >= 0 && index < POOLSIZE){
		return _pool[index];
	}
	else{
		MCE_WARN("MucTalkRoomManagerI::GetPool --> roomid = " << mucRoomIdToString(roomid) << " get pool error");
	}
	return 0;
}


void MucTalkRoomManagerI::CreateRoom(const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
	RoomPoolPtr pool = GetPool(roomid);
	if(pool){
		pool->CreateRoom(roomid);
	}
	MCE_INFO("interface CreateRoom usetime : " << ts.getTime());
}

void MucTalkRoomManagerI::RemoveRoom(const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
	RoomPoolPtr pool = GetPool(roomid);
	if(pool){
	  pool->RemoveRoom(roomid);
	}
	MCE_INFO("interface RemoveRoom usetime : " << ts.getTime());
}


void MucTalkRoomManagerI::AddRoomMember(int userid, const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
	RoomPoolPtr pool = GetPool(roomid);
	if(pool){
	  pool->AddRoomMember(userid, roomid);
	}
	MCE_INFO("interface AddRoomMember usetime : " << ts.getTime());
}
void MucTalkRoomManagerI::RemoveRoomMember(int userid, const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
  RoomPoolPtr pool = GetPool(roomid);
  if(pool){
    pool->RemoveRoomMember(userid, roomid);
  }
	MCE_INFO("interface RemoveRoomMember usetime : " << ts.getTime());
}


IntSeq MucTalkRoomManagerI::GetRoomMember(const MucRoomIdPtr& roomid , const Ice::Current& current){
	TimeStat ts;
	IntSeq ans;
  RoomPoolPtr pool = GetPool(roomid);
  if(pool){
    ans = pool->GetRoomMember(roomid);
  }
	MCE_INFO("interface GetRoomMember usetime : " << ts.getTime());
  return ans;
}


MucActiveUserSeq MucTalkRoomManagerI::GetRoomActiveUserSeq(const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
	MucActiveUserSeq ans;
	MCE_DEBUG("MucTalkRoomManagerI::GetRoomActiveUserSeq --> roomname = " << roomid->roomname);
  RoomPoolPtr pool = GetPool(roomid);
  if(pool){
		MCE_DEBUG("MucTalkRoomManagerI::GetRoomActiveUserSeq --> has pool ");
    ans = pool->GetRoomActiveUserSeq(roomid);
  }
	MCE_INFO("interface GetRoomActiveUserSeq usetime : " << ts.getTime());
  return ans;
}


/*
void MucTalkRoomManagerI::PageNotifyOnline(int onlineuser, const MucRoomIdPtr& roomid, const Ice::Current& current){
	IntSeq members;
	RoomPoolPtr pool = GetPool(roomid);
	if(pool){
		pool->PageNotifyOnline(onlineuser, roomid);
	}
}
void MucTalkRoomManagerI::PageNotifyOffline(int offlineuser, const MucRoomIdPtr& roomid, const Ice::Current& current){
	IntSeq members;
	RoomPoolPtr pool = GetPool(roomid);
	if(pool){
		pool->PageNotifyOffline(offlineuser, roomid);
	}
}
*/





