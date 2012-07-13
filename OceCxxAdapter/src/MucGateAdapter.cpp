#include "MucGateAdapter.h"
#include <boost/lexical_cast.hpp>
#include <string>
using namespace MyUtil;
using namespace com::xiaonei::talk::muc;
using namespace boost;
using namespace std;
MucGateManagerPrx MucGateAdapter::getManager(int id) {
  return locate<MucGateManagerPrx> (_managers, "M", id, TWO_WAY);
}

MucGateManagerPrx MucGateAdapter::getManagerOneway(int id) {
  return locate<MucGateManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}
MucRoomIdPtr MucGateAdapter::MakeRoomIdPtr(int groupid, const string& domain){
  string roomname = "";
  try{
		//change_name_request
    //roomname = boost::lexical_cast<string>(groupid) + "_" + groupname;
    roomname = boost::lexical_cast<string>(groupid);
  }
  catch (std::exception& e){
    MCE_WARN("MucGateAdapter::MageRoomIdPtr-->boost::lexical_cast to string exception  val = " << groupid);
    return NULL;
  }
  MucRoomIdPtr ans = new MucRoomId();
  ans->roomname = roomname;
  ans->domain = domain;
  return ans;
}

Permisions MucGateAdapter::Int2Permision(int p){
	if(1 == p){
		p = 2;
	}
	else if(2 == p){
		p = 3;
	}
	else if(3 == p){
		p = 4;
	}
	else if(5 == p){
		p = 0;
	}
	else{
		p = 0;
	}
	return Permisions(p);
}

TalkUserPtr MucGateAdapter::GetUserById(int userid){
	return getManager(0)->GetUserById(userid);
}
TalkUserMap MucGateAdapter::GetUserBySeq(const MyUtil::IntSeq& uids){
	return getManager(0)->GetUserBySeq(uids);
}
//for memcache test

void MucGateAdapter::ChatSetingChange(int userid, int groupid, int set){
	MucRoomIdPtr roomid = MakeRoomIdPtr(groupid, GROUPDOMAIN);
  getManagerOneway(userid)->ChatSetingChange(userid, roomid, set);
}
void MucGateAdapter::CreateGroupForUgc(int groupowner, int groupid, const IntSeq& inviteusers, int permision){
	MucRoomIdPtr roomid = MakeRoomIdPtr(groupid, GROUPDOMAIN);
	Permisions p = Int2Permision(permision);
  getManagerOneway(groupowner)->CreateGroupForUgc(groupowner, roomid, inviteusers, p);
}

void MucGateAdapter::CreateGroup(int groupid){
	MucRoomIdPtr roomid = MakeRoomIdPtr(groupid, GROUPDOMAIN);
  getManagerOneway(groupid)->CreateGroup(roomid);
}
void MucGateAdapter::RemoveGroup(int groupid){
	MucRoomIdPtr roomid = MakeRoomIdPtr(groupid, GROUPDOMAIN);
  getManagerOneway(groupid)->RemoveGroup(roomid);
}

void MucGateAdapter::ReloadGroupInfo(int groupid, const string& groupname){
	//MucRoomIdPtr roomid = MakeRoomIdPtr(groupid, GROUPDOMAIN);
	getManagerOneway(groupid)->ReloadGroupInfo(groupid, groupname);
}
void MucGateAdapter::AddGroupMember(int userid, int groupid, int permision){
	MucRoomIdPtr roomid = MakeRoomIdPtr(groupid, GROUPDOMAIN);
	Permisions p = Int2Permision(permision);
  getManagerOneway(userid)->AddGroupMember(userid, roomid, p);
}
void MucGateAdapter::AddGroupMemberBySeq(const MyUtil::IntSeq& userids, int groupid, int permision){
	MucRoomIdPtr roomid = MakeRoomIdPtr(groupid, GROUPDOMAIN);
	Permisions p = Int2Permision(permision);
  getManagerOneway((*(userids.begin())))->AddGroupMemberBySeq(userids, roomid, p);
}
void MucGateAdapter::RemoveGroupMember(int userid, int groupid){
	MucRoomIdPtr roomid = MakeRoomIdPtr(groupid, GROUPDOMAIN);
  getManagerOneway(userid)->RemoveGroupMember(userid, roomid);
}
void MucGateAdapter::SetPermision(int operateuser, int groupid, int targetuser, int permision){
	MucRoomIdPtr roomid = MakeRoomIdPtr(groupid, GROUPDOMAIN);
	Permisions p = Int2Permision(permision);
  getManagerOneway(operateuser)->SetPermision(operateuser, roomid, targetuser, p);
}
//FOR IM
void MucGateAdapter::EnterRoom(const MucActiveUserPtr& enteruser){
  getManagerOneway(enteruser->jid->userId)->EnterRoom(enteruser);
}
bool MucGateAdapter::SetPermisionForIM(int operateuser, const MucRoomIdPtr& roomid, int targetuser, Permisions permision){
  return getManager(operateuser)->SetPermisionForIM(operateuser, roomid, targetuser, permision);
}
MucActiveUserSeq MucGateAdapter::GetUserByPermision(const MucRoomIdPtr& roomid, Permisions permision){
  int irid = GetIdFromMucRoomId(roomid);
	return getManager(irid)->GetUserByPermision(roomid, permision);
}

void MucGateAdapter::UserInviteUser(const JidPtr& opjid, const MucRoomIdPtr& roomid, int targetuser){
	return getManagerOneway(opjid->userId)->UserInviteUser(opjid, roomid, targetuser);
}
void MucGateAdapter::SendPrivateMsg(const JidPtr& sender, const MucUserIdentityPtr& recidentity, const string& msg){
	return getManagerOneway(sender->userId)->SendPrivateMsg(sender, recidentity, msg);
}
//FOR PAGER
MucUserIdentitySeq MucGateAdapter::GetRoomUserList(const MucRoomIdPtr& roomid){
  int irid = GetIdFromMucRoomId(roomid);
  return getManager(irid)->GetRoomUserList(roomid);
}
string MucGateAdapter::GetGroupName(int groupid){
	return getManager(groupid)->GetGroupName(groupid);
}
//IM && PAGER(groups.renren.com)
void MucGateAdapter::UserOnline(const JidPtr& onlinejid){
  getManagerOneway(onlinejid->userId)->UserOnline(onlinejid);
}
void MucGateAdapter::UserOffline(const JidPtr& offlinejid){
  getManagerOneway(offlinejid->userId)->UserOffline(offlinejid);
}
MucRoomIdSeq MucGateAdapter::GetUserRooms(int userid, const string& domain){
	MucRoomIdSeq ans;
  ans = getManager(userid)->GetUserRooms(userid, domain);
	MCE_DEBUG("MucGateAdapter::GetUserRooms --> ans.size = " << ans.size());
	return ans;
}
void MucGateAdapter::AwayRoom(const JidPtr& opjid, const MucRoomIdPtr& roomid){
  getManagerOneway(opjid->userId)->AwayRoom(opjid, roomid);
}
void MucGateAdapter::SendTalkMsg(const JidPtr& opjid, const MucRoomIdPtr& roomid, const string& msg){
  getManagerOneway(opjid->userId)->SendTalkMsg(opjid, roomid, msg);
}
//------------------------RESPONSE---------
// 这个notify 的几个方法，一定要按照roomid散，不然，gate内部的cache会有问题。
void MucGateAdapter::NotifyUserEnterRoom(const MucActiveUserPtr& activeuser, const MucActiveUserSeq& presenceUsers, int mucstatus){
  int irid = GetIdFromMucRoomId(activeuser->identity->roomid);
  getManagerOneway(irid)->NotifyUserEnterRoom(activeuser, presenceUsers, mucstatus);
}
void MucGateAdapter::NotifyUserAwayRoom(const MucActiveUserPtr& awayuser, const JidSeq& presenceUsers, int mucstatus){
  int irid = GetIdFromMucRoomId(awayuser->identity->roomid);
  getManagerOneway(irid)->NotifyUserAwayRoom(awayuser, presenceUsers, mucstatus);
}
void MucGateAdapter::NotifyGroupChat(const MucActiveUserPtr& activeuser, const JidSeq& presenceUsers, const string& msg){
  int irid = GetIdFromMucRoomId(activeuser->identity->roomid);
  getManagerOneway(irid)->NotifyGroupChat(activeuser, presenceUsers, msg);
}
void MucGateAdapter::NotifySetPermision(const MucActiveUserPtr& opuser, const JidSeq& presenceUsers, const MucActiveUserSeq& targetusers, Permisions permision){
  int irid = GetIdFromMucRoomId(opuser->identity->roomid);
  getManagerOneway(irid)->NotifySetPermision(opuser, presenceUsers, targetusers, permision);
}
void MucGateAdapter::InviteIMusers(const MucRoomIdPtr& roomid, const JidSeq& imusers){
  int irid = GetIdFromMucRoomId(roomid);
  getManagerOneway(irid)->InviteIMUsers(roomid, imusers);
}

void MucGateAdapter::NotifyPrivateChat(const MucActiveUserPtr& sender, int targetuser, const string& msg){
  int irid = GetIdFromMucRoomId(sender->identity->roomid);
	getManagerOneway(irid)->NotifyPrivateChat(sender, targetuser, msg);
}
void MucGateAdapter::NotifyNicknameError(const MucRoomIdPtr& roomid, const JidPtr& jid){
  int irid = GetIdFromMucRoomId(roomid);
	getManagerOneway(irid)->NotifyNicknameError(roomid, jid);
}
void MucGateAdapter::NotifyChatSetingChange(const MucRoomIdPtr& roomid, const JidSeq& jids, int set){
  int irid = GetIdFromMucRoomId(roomid);
	getManagerOneway(irid)->NotifyChatSetingChange(roomid, jids, set);
}
