#include "MucTalkRoomAdapter.h"
#include <boost/lexical_cast.hpp>
#include "XmppTools.h"
#include <string>
using namespace MyUtil;
using namespace com::xiaonei::talk::muc;
using namespace boost;
using namespace std;
using namespace com::xiaonei::talk::util;
MucTalkRoomManagerPrx MucTalkRoomAdapter::getManager(int id) {
  return locate<MucTalkRoomManagerPrx> (_managers, "M", id, TWO_WAY);
}

MucTalkRoomManagerPrx MucTalkRoomAdapter::getManagerOneway(int id) {
  return locate<MucTalkRoomManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}

void MucTalkRoomAdapter::CreateRoom(const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
  getManagerOneway(irid)->CreateRoom(roomid);
}
void MucTalkRoomAdapter::RemoveRoom(const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
  getManagerOneway(irid)->RemoveRoom(roomid);
}
void MucTalkRoomAdapter::AddRoomMember(int userid, const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
  getManagerOneway(irid)->AddRoomMember(userid, roomid);
}
void MucTalkRoomAdapter::RemoveRoomMember(int userid, const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
  getManagerOneway(irid)->RemoveRoomMember(userid, roomid);
}

IntSeq MucTalkRoomAdapter::GetRoomMember(const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
  return getManager(irid)->GetRoomMember(roomid);
}
MucActiveUserSeq MucTalkRoomAdapter::GetRoomActiveUserSeq(const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
  return getManager(irid)->GetRoomActiveUserSeq(roomid);
}
/*
void MucTalkRoomAdapter::PageNotifyOnline(int onlineuser, const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
	getManagerOneway(irid)->PageNotifyOnline(onlineuser, roomid);
}
void MucTalkRoomAdapter::PageNotifyOffline(int offlineuser, const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
	getManagerOneway(irid)->PageNotifyOffline(offlineuser, roomid);
}
*/
