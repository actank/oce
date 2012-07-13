#include "MucUserRoomAdapter.h"
#include <boost/lexical_cast.hpp>
#include <string>
using namespace MyUtil;
using namespace com::xiaonei::talk::muc;
using namespace boost;
using namespace std;
MucUserRoomManagerPrx MucUserRoomAdapter::getManager(int id) {
  return locate<MucUserRoomManagerPrx> (_managers, "M", id, TWO_WAY);
}

MucUserRoomManagerPrx MucUserRoomAdapter::getManagerOneway(int id) {
  return locate<MucUserRoomManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}

MucRoomIdSeq MucUserRoomAdapter::GetUserRooms(int userid, const string& domain){
  return getManager(userid)->GetUserRooms(userid, domain);
}
bool MucUserRoomAdapter::AddRoom(int userid, const MucRoomIdPtr& roomid, const MucMemberPtr& member){
  return getManager(userid)->AddRoom(userid, roomid, member);
}
bool MucUserRoomAdapter::RemoveRoom(int userid, const MucRoomIdPtr& roomid){
  return getManager(userid)->RemoveRoom(userid, roomid);
}
MucMemberPtr MucUserRoomAdapter::GetMemberPermision(int userid, const MucRoomIdPtr& roomid){
  return getManager(userid)->GetMemberPermision(userid, roomid);
}
//MucMemberMap MucUserRoomAdapter::GetMemberPermisionBySeq(const MyUtil::IntSeq& userids, const MucRoomIdPtr& roomid){
//  return getManager(userid)->GetMemberPermisionBySeq(userids, roomid);
//}
MucRoomMemberSeq MucUserRoomAdapter::GetUserRoomAndMember(int userid){
  return getManager(userid)->GetUserRoomAndMember(userid);
}
void MucUserRoomAdapter::SetUserPermision(int targetuser, const MucRoomIdPtr& roomid, Permisions permision){
  getManagerOneway(targetuser)->SetUserPermision(targetuser, roomid, permision);
}
void MucUserRoomAdapter::UserOnline(const JidPtr& onlinejid){
  getManagerOneway(onlinejid->userId)->UserOnline(onlinejid);
}
void MucUserRoomAdapter::UserOffline(const JidPtr& offlinejid){
  getManagerOneway(offlinejid->userId)->UserOffline(offlinejid);
}
void MucUserRoomAdapter::EnterRoomForTemporary(const MucActiveUserPtr& enteruser){
	getManagerOneway(enteruser->jid->userId)->EnterRoomForTemporary(enteruser);
}
void MucUserRoomAdapter::AwayRoomForTemporary(const JidPtr& jid, const MucRoomIdPtr& roomid){
	getManagerOneway(jid->userId)->AwayRoomForTemporary(jid, roomid);
}
