#include "MucOnlineCenterAdapter.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include "XmppTools.h"
using namespace MyUtil;
using namespace com::xiaonei::talk::muc;
using namespace boost;
using namespace std;
using namespace com::xiaonei::talk::util;
MucOnlineCenterManagerPrx MucOnlineCenterAdapter::getManager(int id) {
  return locate<MucOnlineCenterManagerPrx> (_managers, "M", id, TWO_WAY);
}

MucOnlineCenterManagerPrx MucOnlineCenterAdapter::getManagerOneway(int id) {
  return locate<MucOnlineCenterManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}

void MucOnlineCenterAdapter::ChatSetingChange(int userid, const MucRoomIdPtr& roomid, int set){
	int irid = GetIdFromMucRoomId(roomid);
  getManagerOneway(irid)->ChatSetingChange(userid, roomid, set);
}

void MucOnlineCenterAdapter::EnterRoom(const MucActiveUserPtr& activeuser){
	int irid = GetIdFromMucRoomId(activeuser->identity->roomid);
	MCE_DEBUG("MucOnlineCenterAdapter::EnterRoom --> GET IRID FROM XMPPTOOLS irid = " << irid);
  getManagerOneway(irid)->EnterRoom(activeuser);
}
void MucOnlineCenterAdapter::UserOnline(const MucActiveUserPtr& activeuser){
	int irid = GetIdFromMucRoomId(activeuser->identity->roomid);
  getManagerOneway(irid)->UserOnline(activeuser);
}
void MucOnlineCenterAdapter::AwayRoom(const JidPtr& opuser, const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
  getManagerOneway(irid)->AwayRoom(opuser, roomid);
}
void MucOnlineCenterAdapter::SendTalkMessage(const JidPtr& opuser, const MucRoomIdPtr& roomid, const string& msg){
	int irid = GetIdFromMucRoomId(roomid);
  getManagerOneway(irid)->SendTalkMessage(opuser, roomid, msg);
}
int MucOnlineCenterAdapter::GetUserCount(const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
  return getManager(irid)->GetUserCount(roomid);
}
void MucOnlineCenterAdapter::SetPermision(int opuser, Permisions permision, int targetuser, const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
  getManagerOneway(irid)->SetPermision(opuser, permision, targetuser, roomid);
}
MucUserIdentitySeq MucOnlineCenterAdapter::GetActiveUsers(const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
  return getManager(irid)->GetActiveUsers(roomid);
}
JidSeq MucOnlineCenterAdapter::GetActiveJids(const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
	return getManager(irid)->GetActiveJids(roomid);
}
bool MucOnlineCenterAdapter::SetPermisionForIM(int opuser, Permisions permision, int targetuser, const MucRoomIdPtr& roomid){
	int irid = GetIdFromMucRoomId(roomid);
  return getManager(irid)->SetPermisionForIM(opuser, permision, targetuser, roomid);
}
MucActiveUserSeq MucOnlineCenterAdapter::GetUserByPermision(const MucRoomIdPtr& roomid, Permisions permision){
	int irid = GetIdFromMucRoomId(roomid);
  return getManager(irid)->GetUserByPermision(roomid, permision);
}

void MucOnlineCenterAdapter::SendPrivateMsg(const JidPtr& sender, const MucUserIdentityPtr& recidentity, const string& msg){
	int irid = GetIdFromMucRoomId(recidentity->roomid);
	getManagerOneway(irid)->SendPrivateMsg(sender, recidentity, msg);
}
