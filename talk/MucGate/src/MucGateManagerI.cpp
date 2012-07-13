#include "MucDeliverAdapter.h"
#include "MucGateManagerI.h"
#include "ServiceI.h"
#include <boost/lexical_cast.hpp>
#include "OnlineBitmapAdapter.h"
#include "OnlineCenterAdapter.h"
#include "MessageMsgHandlerAdapter.h"
#include "MucTalkRoomAdapter.h"
#include "MucUserRoomAdapter.h"
#include "MucOnlineCenterAdapter.h"
//#include "OzeinFilterAdapter.h"
#include "XmppTools.h"
#include "TalkCacheLoaderAdapter.h"
#include "FeedMemcProxy/client/talk_cache_client.h"
#include "talk/SpamClient/src/OzeinFilterAdapter.h"
#include "PageCacheAdapter.h"
#include "MucTalkRecordAdapter.h"
#include "Date.h"
#include "QueryRunner.h"
#include "ConnectionQuery.h"
#include "MucTask.h"
#include "client/linux/handler/exception_handler.h"
using namespace com::xiaonei::xce;
using namespace talk::http;
using namespace xce::talk;
using namespace com::xiaonei::talk;
using namespace talk::mucdeliver::adapter;
using namespace com::xiaonei::talk::muc;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk::common;
using namespace talk::online;
using namespace talk::adapter;
using namespace talk::online::adapter;
using namespace talk::onlinebitmap;
using namespace MyUtil;
using namespace std;
using namespace com::xiaonei::ozein::content::generation;
bool MyCallback(const char *dump_path,
		const char *minidump_id,
		void *context,
		bool succeeded) {
	printf("got crash %s %s\n", dump_path, minidump_id);
	return true;
}
void MyUtil::initialize(){
	static google_breakpad::ExceptionHandler eh(".", 0, MyCallback
                                         , 0, true);
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	service.getObjectCache()->registerCategory(GN, "GN", new GroupNameFactoryI, props, true);
	service.getAdapter()->add(&MucGateManagerI::instance(), service.createIdentity("M", ""));
}

Ice::ObjectPtr GroupNameFactoryI::create(Ice::Int id) {
	GroupNamePtr gn = new GroupName;
	Statement sql;
	ostringstream os;
	os << "minigroup_" << (id % 100);
	string tablename = os.str();
	MCE_INFO("GroupNameFactoryI::create --> id = " << id);
	sql << "select name from " << tablename << " where id = " << id;
	mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, tablename).store(sql);
	if (res && res.num_rows() > 0) {
		mysqlpp::Row row = res.at(0);
		gn->groupname = (string)row["name"];
		MCE_INFO("GroupNameFactoryI::create --> groupname = " << gn->groupname);
	}
	return gn;
}

string MucGateManagerI::Permision2Str(Permisions permision){
  switch (permision) {
  case POwner:
    return "owner";
  case PAdmin:
    return "admin";
  case PMember:
    return "member";
  case PNone:
    return "none";
  case POutcast:
    return "outcast";
  }
  return "none";
}
string MucGateManagerI::Psermision2Role(Permisions permision){
  switch (permision) {
  case POwner:
    return "moderator";
  case PAdmin:
    return "moderator";
  case PMember:
    return "participant";
  case PNone:
    return "none";
  case POutcast:
    return "outcast";
  }
  return "none";
}



//---------------------------------------------------------------------------------------------------------------
MucRoomIdPtr MucGateManagerI::MakeRoomIdPtr(const string& roomname, const string& domain){
  MucRoomIdPtr ans = new MucRoomId();
  ans->roomname = roomname;
  ans->domain = domain;
  return ans;
}
MucRoomIdPtr MucGateManagerI::MakeRoomIdPtr(int groupid, const string& groupname, const string& domain){
  string roomname = "";
  try{
		//change_name_request
    //roomname = boost::lexical_cast<string>(groupid) + "_" + groupname;
    roomname = boost::lexical_cast<string>(groupid);
  }
  catch (exception& e){
    MCE_WARN("MucGateManagerI::MakeRoomIdPtr-->boost::lexical_cast to string exception  val = " << groupid);
    return NULL;
  }
  MucRoomIdPtr ans = new MucRoomId();
  ans->roomname = roomname;
  ans->domain = domain;
  return ans;
}

//for memcache test


TalkUserPtr MucGateManagerI::GetUserById(int userid, const Ice::Current& current){
	TalkUserPtr u = TalkCacheClient::instance().GetUserByIdWithLoad(userid);
	return u;
}
TalkUserMap MucGateManagerI::GetUserBySeq(const MyUtil::IntSeq& uids, const Ice::Current& current){
	//TalkUserMap m = TalkCacheClient::instance().GetUserBySeqWithLoad(uids);
	//return m;
}


//------------------------RECEIVE REQUEST--------------------------------------------------------------------------------

void MucGateManagerI::ChatSetingChange(int userid, const MucRoomIdPtr& roomid, int set, const Ice::Current& current){
	TimeStat ts;
	if(!roomid || PERMANENT != roomid->domain){
		return;
	}
	try{
    MucOnlineCenterAdapter::instance().ChatSetingChange(userid, roomid, set);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::ChatSetingChange --> call MucOnlineCenter.ChatSetingChange ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::ChatSetingChange --> call MucOnlineCenter.ChatSetingChange ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface ChatSetingChange usetime : " << ts.getTime());
}

//FOR GROUPS
void MucGateManagerI::CreateGroupForUgc(int groupowner, const MucRoomIdPtr& roomid, const IntSeq& inviteusers, Permisions invited_permision, const Ice::Current& current){
	TimeStat ts;
	MCE_INFO("interface CreateGroupForUgc usetime : " << ts.getTime());
	MCE_DEBUG("MucGateManagerI::CreateGroupForUgc --> owner = " << groupowner << " roomid = " << mucRoomIdToString(roomid));
	if(!roomid || PERMANENT != roomid->domain){
		return;
	}
	TaskManager::instance().execute(new CreateMiniGroupTask(groupowner, roomid, inviteusers, invited_permision));
	MCE_INFO("interface CreateGroupForUgc usetime : " << ts.getTime());
}


void MucGateManagerI::CreateGroup(const MucRoomIdPtr& roomid, const Ice::Current& current){
  //MucRoomIdPtr roomid = MakeRoomIdPtr(groupid, roomname, PERMANENT);
	TimeStat ts;
	MCE_DEBUG("MucGateManagerI::CreateGroup --> roomname = " << roomid->roomname << " domain = " << roomid->domain);
  if(!roomid || PERMANENT != roomid->domain){
    return;
  }
  try{
    MucTalkRoomAdapter::instance().CreateRoom(roomid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::CreateGroup --> call MucTalkRoomAdapter.CreateRoom ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::CreateGroup --> call MucTalkRoomAdapter.CreateRoom ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface CreateGroup usetime : " << ts.getTime());
}


void MucGateManagerI::RemoveGroup(const MucRoomIdPtr& roomid, const Ice::Current& current){
  //MucRoomIdPtr roomid = MakeRoomIdPtr(groupid, "_", PERMANENT);
	TimeStat ts;
  if(!roomid || PERMANENT != roomid->domain){
    return;
  }
  try{
    MucTalkRoomAdapter::instance().RemoveRoom(roomid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::RemoveGroup --> call MucTalkRoomAdapter.RemoveRoom ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::RemoveGroup --> call MucTalkRoomAdapter.RemoveRoom ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface RemoveGroup usetime : " << ts.getTime());
}


void MucGateManagerI::ReloadGroupInfo(int roomid, const string& groupname, const Ice::Current& current){
	TimeStat ts;
	if(!roomid){
		return;
	}
	try{
		MCE_DEBUG("MucGateManagerI::ReloadGroupInfo --> call PageCache.UpdateRoomName roomid = " << roomid << " groupname = " << groupname);
    PageCacheAdapter::instance().UpdateRoomName(roomid, groupname);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::ReloadGroupInfo--> call PageCache.UpdateRoomName ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::ReloadGroupInfo--> call PageCache.UpdateRoomName ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_DEBUG("MucGateManagerI::ReloadGroupInfo --> roomid = " << roomid);
	GroupNamePtr oldobj = ServiceI::instance().locateObject<GroupNamePtr>(GN,roomid);
	if(oldobj){
		if(oldobj->groupname == groupname){
			return;
		}
		GroupNamePtr newobj = new GroupName;
		newobj->groupname = groupname;
		ServiceI::instance().removeObject(GN, roomid);
		ServiceI::instance().addObject(newobj, GN, roomid);
	}
	MCE_INFO("interface ReloadGroupInfo usetime : " << ts.getTime());
}



void MucGateManagerI::AddGroupMemberBySeq(const MyUtil::IntSeq& userids, const MucRoomIdPtr& roomid,  Permisions permision, const Ice::Current& current){
	TimeStat ts;
	for(IntSeq::const_iterator it = userids.begin(); it != userids.end(); ++it){
		AddGroupMember((*it), roomid, permision);
	}
	MCE_INFO("interface AddGroupMemberBySeq usetime : " << ts.getTime());
}
void MucGateManagerI::AddGroupMember(int userid, const MucRoomIdPtr& roomid, Permisions permision, const Ice::Current& current){
	TimeStat ts;
  //MucRoomIdPtr roomid = MakeRoomIdPtr(groupid, groupname, PERMANENT);
  if(!roomid || PERMANENT != roomid->domain){
    return;
  }

  //ADD MEMBER FOR ROOM
  try{
		MCE_DEBUG("MucGateManagerI::AddGroupMember --> CALL MucTalkRoom.AddRoomMember");
    MucTalkRoomAdapter::instance().AddRoomMember(userid, roomid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::AddGroupMember --> call MucTalkRoomAdapter.AddRoomMember ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::AddGroupMember --> call MucTalkRoomAdapter.AddRoomMember ERR  line:"<<__LINE__<<" err:"<<e.what());
  }

  //ADD ROOM FOR USER
  MucMemberPtr member = new MucMember();
  member->userid = userid;
  member->permision = permision;
  try{
		MCE_DEBUG("MucGateManagerI::AddGroupMember --> CALL MucUserRoom.AddRoom");
    MucUserRoomAdapter::instance().AddRoom(userid, roomid, member);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::AddGroupMember --> call MucUserRoomAdapter.AddRoom ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::AddGroupMember --> call MucUserRoomAdapter.AddRoom ERR  line:"<<__LINE__<<" err:"<<e.what());
  }

  //GET USER JIDS;
  JidSeq jids, imjids;
  try{
    jids = OnlineCenterAdapter::instance().getUserJids(userid);
		MCE_DEBUG("MucGateManagerI::AddGroupMember --> CALL ONLINECENTER jids.size = " << jids.size());
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::AddGroupMember --> call OnlineCenterAdapter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::AddGroupMember --> call OnlineCenterAdapter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e.what());
  }

  //PUSH THE WEB USER JIDS INTO THE ROOM AND INVITE THE IMUSERS;
  if(!jids.empty()){
    string username = "";
    try{
      TalkUserPtr userptr = TalkCacheClient::instance().GetUserByIdWithLoad(userid);
			if(userptr){
      	username = userptr->name;
			}
    }
    catch(Ice::Exception& e){
      MCE_WARN("MucGateManagerI::AddGroupMember --> call TalkCacheClient.GetUserByIdWithLoad ERR  line:"<<__LINE__<<" err:"<<e);
    }
    catch(std::exception& e){
      MCE_WARN("MucGateManagerI::AddGroupMember --> call TalkCacheClient.GetUserByIdWithLoad ERR  line:"<<__LINE__<<" err:"<<e.what());
    }
    if(username.empty()){
      username = boost::lexical_cast<string>(userid);
    }
    JidSeq::iterator jit = jids.begin();
    for(; jit != jids.end(); ++jit){
			if('W' == (*jit)->endpoint[0]){
				MucUserIdentityPtr identity = new MucUserIdentity();
				identity->roomid = roomid;
				identity->nickname = makeMucNickname((*jit), username);
				identity->userid = (*jit)->userId;
				MucActiveUserPtr p = new MucActiveUser();
				p->identity = identity;
				p->jid = (*jit);
				p->permision = permision;
				try{
					MCE_DEBUG("MucGateManagerI::AddGroupMember --> CALL MucOnlineCenterAdapter.EnterRoom info : jid = " << jidToString((*jit)) << " roomname = " << roomid->roomname);
					MucOnlineCenterAdapter::instance().EnterRoom(p);
				}
				catch(Ice::Exception& e){
					MCE_WARN("MucGateManagerI::AddGroupMember --> call MucOnlineCenterAdapter.EnterRoom ERR  line:"<<__LINE__<<" err:"<<e);
				}
				catch(std::exception& e){
					MCE_WARN("MucGateManagerI::AddGroupMember --> call MucOnlineCenterAdapter.EnterRoom ERR  line:"<<__LINE__<<" err:"<<e.what());
				}
			}else if('T' == (*jit)->endpoint[0]){
				imjids.push_back((*jit));
			}
    }
		if(!imjids.empty()){
			InviteIMUsers(roomid, imjids);
		}
  }
	MCE_INFO("interface AddGroupMember usetime : " << ts.getTime());
}

void MucGateManagerI::RemoveGroupMember(int userid, const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
  if(!roomid || PERMANENT != roomid->domain){
    return;
  }
  try{
    MCE_DEBUG("MucGateManagerI::RemoveGroupMember --> CALL MucTalkRoom.RemoveRoomMember");
    MucTalkRoomAdapter::instance().RemoveRoomMember(userid, roomid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::RemoveGroupMember --> call MucTalkRoomAdapter.RemoveRoomMember ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::RemoveGroupMember --> call MucTalkRoomAdapter.RemoveRoomMember ERR  line:"<<__LINE__<<" err:"<<e.what());
  }

  try{
    MCE_DEBUG("MucGateManagerI::RemoveGroupMember --> CALL MucUserRoomAdapter.RemoveRoom");
    MucUserRoomAdapter::instance().RemoveRoom(userid, roomid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::RemoveGroupMember --> call MucUserRoomAdapter.RemoveRoom ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::RemoveGroupMember --> call MucUserRoomAdapter.RemoveRoom ERR  line:"<<__LINE__<<" err:"<<e.what());
  }

  //GET USER JIDS;
  JidSeq jids;
  try{
    jids = OnlineCenterAdapter::instance().getUserJids(userid);
    MCE_DEBUG("MucGateManagerI::RemoveGroupMember --> CALL OnlineCenterAdapter.getUserJids jids.size = " << jids.size());
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::RemoveGroupMember--> call OnlineCenterAdapter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::RemoveGroupMember--> call OnlineCenterAdapter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e.what());
  }

  //NOTIFY MUCUSERROOM USER OFFLINE;
  if(!jids.empty()){
		MessageSeq seq;
		JidSeq::iterator jit = jids.begin();
    for(; jit != jids.end(); ++jit){
      try{
        MCE_DEBUG("MucGateManagerI::RemoveGroupMember--> CALL MucOnlineCenterAdapter.AwayRoom( " << jidToString(*jit) << ", " << roomid->roomname << ")");
        MucOnlineCenterAdapter::instance().AwayRoom((*jit), roomid);
      }
      catch(Ice::Exception& e){
        MCE_WARN("MucGateManagerI::RemoveGroupMember--> call MucOnlineCenterAdapter.AwayRoom ERR  line:"<<__LINE__<<" err:"<<e);
      }
      catch(std::exception& e){
        MCE_WARN("MucGateManagerI::RemoveGroupMember--> call MucOnlineCenterAdapter.AwayRoom ERR  line:"<<__LINE__<<" err:"<<e.what());
      }
			if('W' == (*jit)->endpoint[0]){
				//NotifyIMUser self exit room.
				MucUserIdentityPtr identity = new MucUserIdentity();
				identity->roomid = roomid;
				identity->nickname = "";
				identity->userid = userid;
				string activeuser_identity_str = mucUserIdentityToString(identity);
				xml_document doc;
				xml_node presenceNode = doc.append_child();
				presenceNode.set_name("presence");
				presenceNode.append_attribute("from").set_value(activeuser_identity_str.c_str());
				presenceNode.append_attribute("to").set_value(jidToString((*jit)).c_str());
				presenceNode.append_attribute("type").set_value("exitgroup");
				xml_node onlineTypeNode = presenceNode.append_child();
				onlineTypeNode.set_name("onlineType");
				onlineTypeNode.append_child(node_pcdata).set_value("0");
				MessagePtr msg = new Message();
				msg->from = (*jit);
				msg->to = (*jit);
				msg->msg = doc.xml();
				msg->type = MUC_PRESENCE;
				seq.push_back(msg);
				MCE_DEBUG("MucGateManagerI::RemoveGroupMember -->  WEBJID = " << jidToString((*jit)) << " EXITGROUP~~~~~ : " << doc.xml());
			}
    }
		if(!seq.empty()){
			try{
				MCE_INFO("MucGateManagerI::RemoveGroupMember --> CALL MucDeliver.deliver seq.size = " << seq.size());
				MucDeliverAdapter::instance().deliver(seq);
			} catch(Ice::Exception& e){
				MCE_WARN("MucGateManagerI::RemoveGroupMember --> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e);
			}
			catch(std::exception& e){
				MCE_WARN("MucGateManagerI::RemoveGroupMember --> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e.what());
			}
		}
  }
	MCE_INFO("interface RemoveGroupMember usetime : " << ts.getTime());
}

//给ugc用的设置权限的接口
void MucGateManagerI::SetPermision(int operateuser, const MucRoomIdPtr& roomid, int targetuser, Permisions permision, const Ice::Current& current){
	TimeStat ts;
  string domain = roomid->domain;
	//踢人||人自己退出
	if(permision < PMember){
		RemoveGroupMember(targetuser, roomid);
		return;
	}
	//设置群成员权限
	try{
		MCE_DEBUG("MucGateManagerI::SetPermision --> CALL MucUserRoomAdapter.SetUserPermision");
		MucUserRoomAdapter::instance().SetUserPermision(targetuser, roomid, permision);
	}
	catch(Ice::Exception& e){
		MCE_WARN("MucGateManagerI::SetPermision --> call MucUserRoomAdapter.SetUserPermision ERR  line:"<<__LINE__<<" err:"<<e);
	}
	catch(std::exception& e){
		MCE_WARN("MucGateManagerI::SetPermision --> call MucUserRoomAdapter.SetUserPermision ERR  line:"<<__LINE__<<" err:"<<e.what());
	}
  try{
    MCE_DEBUG("MucGateManagerI::SetPermision --> MucOnlineCenterAdapter.SetPermision");
    MucOnlineCenterAdapter::instance().SetPermision(operateuser, permision, targetuser, roomid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::SetMemberPermision --> call MucOnlineCenterAdapter.SetUserPermision ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::SetMemberPermision --> call MucOnlineCenterAdapter.SetUserPermision ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface SetMemberPermision usetime : " << ts.getTime());
}


//IM自己调用的设置权限的接口
bool MucGateManagerI::SetPermisionForIM(int operateuser, const MucRoomIdPtr& roomid, int targetuser, Permisions permision, const Ice::Current& current){
	TimeStat ts;
	if(PERMANENT == roomid->domain){
		//im不允许用户对固定群权限进行设置。
		return false;
	}
  bool setok = false;
  try{
    setok = MucOnlineCenterAdapter::instance().SetPermisionForIM(operateuser, permision, targetuser, roomid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::SetMemberPermisionForIM --> call MucOnlineCenterAdapter.SetMemberPermisionForIM ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::SetMemberPermisionForIM --> call MucOnlineCenterAdapter.SetMemberPermisionForIM ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface SetPermisionForIM usetime : " << ts.getTime());
  return setok;
}

//FOR IM
void MucGateManagerI::EnterRoom(const MucActiveUserPtr& enteruser, const Ice::Current& current){
	TimeStat ts;
	MCE_DEBUG("MucGateManagerI::EnterRoom --> enter rooom jid = " << jidToString(enteruser->jid) << " roomid = " << mucRoomIdToString(enteruser->identity->roomid));
  if(TEMPORARY == enteruser->identity->roomid->domain){
    try{
      MCE_DEBUG("MucGateManagerI::EnterRoom --> call MucUserRoom.EnterRoomForTemporary enterjid = " << jidToString(enteruser->jid) << " roomname = " << enteruser->identity->roomid->roomname);
      MucUserRoomAdapter::instance().EnterRoomForTemporary(enteruser);
    }
    catch(Ice::Exception& e){
      MCE_WARN("MucGateManagerI::EnterRoom --> call MucUserRoomAdapter.EnterRoomForTemporary ERR  line:"<<__LINE__<<" err:"<<e);
    }
    catch(std::exception& e){
      MCE_WARN("MucGateManagerI::EnterRoom --> call MucUserRoomAdapter.EnterRoomForTemporary ERR  line:"<<__LINE__<<" err:"<<e.what());
    }
		MCE_INFO("interface EnterRoom usetime : " << ts.getTime());
    return;
  }
	try{
		MCE_DEBUG("MucGateManagerI::EnterRoom --> enter rooom jid = " << jidToString(enteruser->jid) << " domain = group.talk.xiaonei.com");
		MucOnlineCenterAdapter::instance().EnterRoom(enteruser);
  }
  catch(Ice::Exception& e){
   	MCE_WARN("MucGateManagerI::EnterRoom --> call MucOnlineCenterAdapter.EnterRoom ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::EnterRoom --> call MucOnlineCenterAdapter.EnterRoom ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface EnterRoom usetime : " << ts.getTime());
}

void MucGateManagerI::UserInviteUser(const JidPtr& opjid, const MucRoomIdPtr& roomid, int targetuser, const Ice::Current& current){
	TimeStat ts;
  string roomstr = mucRoomIdToString(roomid);
  if(roomstr.empty()){
    return;
  }
	MCE_DEBUG("MucGateManagerI::UserInviteUser --> opjid = " << jidToString(opjid) << " roomid = " << mucRoomIdToString(roomid) << " targetuser = " << targetuser);
	JidSeq imusers;
  try{
    imusers = OnlineCenterAdapter::instance().getUserJids(targetuser, 4);
		MCE_DEBUG("MucGateManagerI::UserInviteUser --> CALL ONLINECENTER  get IM jids.size = " << imusers.size());
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::UserInviteUser --> call OnlineCenterAdapter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::UserInviteUser --> call OnlineCenterAdapter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MessageSeq seq;
  JidSeq::const_iterator jit = imusers.begin();
  for(; jit != imusers.end(); ++jit){
    xml_document doc;
    xml_node messageNode = doc.append_child();
		messageNode.set_name("message");
    messageNode.append_attribute("from").set_value(roomstr.c_str());
    messageNode.append_attribute("to").set_value(jidToString((*jit)).c_str());
    xml_node xNode = messageNode.append_child();
		xNode.set_name("x");
    xNode.append_attribute("xmlns").set_value("http://jabber.org/protocol/muc#user");
    xml_node inviteNode = xNode.append_child();
		inviteNode.set_name("invite");
    inviteNode.append_attribute("from").set_value(jidToString(opjid).c_str());
    MessagePtr msg = new Message();
    msg->from = opjid;
    msg->to = (*jit);
    msg->msg = doc.xml();
    msg->type = MESSAGE;//none
    msg->msgKey = -1;
    seq.push_back(msg);
  }
	try{
		MCE_INFO("MucGateManagerI::UserInviteUser--> CALL MucDeliver.deliver seq.size = " << seq.size());
		MucDeliverAdapter::instance().deliver(seq);
	} catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::UserInviteUser --> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::UserInviteUser --> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface UserInviteUser usetime : " << ts.getTime());
}

void MucGateManagerI::SendPrivateMsg(const JidPtr& sender, const MucUserIdentityPtr& recidentity, const string& msg, const Ice::Current& current){
	TimeStat ts;
	MucUserIdentitySeq ans;
  try{
    MCE_DEBUG("MucGateManagerI::SendPrivateMsg --> MucOnlineCenterAdapter.SendPrivateMsg sender = " << jidToString(sender) << " recidentity = " << mucUserIdentityToString(recidentity) << " msg = " << msg);
    MucOnlineCenterAdapter::instance().SendPrivateMsg(sender, recidentity, msg);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::SendPrivateMsg--> call MucOnlineCenter.SendPrivateMsg ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::SendPrivateMsg--> call MucOnlineCenter.SendPrivateMsg ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface SendPrivateMsg usetime : " << ts.getTime());
}
string MucGateManagerI::GetGroupName(int roomid, const Ice::Current& current){
	TimeStat ts;
	GroupNamePtr info = ServiceI::instance().locateObject<GroupNamePtr>(GN,roomid);
	MCE_INFO("interface GetGroupName usetime : " << ts.getTime());
	if(info){
		MCE_DEBUG("MucGateManagerI::GetGroupName --> roomid = " << roomid << " roomname = " << info->groupname);
		return info->groupname;
	}
	MCE_WARN("MucGateManagerI::GetGroupName --> roomid = " << roomid << " failed....");
	return "";
}
MucUserIdentitySeq MucGateManagerI::GetRoomUserList(const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
  MucUserIdentitySeq ans;
  try{
    MCE_DEBUG("MucGateManagerI::GetRoomUserList --> MucOnlineCenterAdapter.GetActiveUsers roomname = " << roomid->roomname);
    ans = MucOnlineCenterAdapter::instance().GetActiveUsers(roomid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::GetRoomUserList --> call MucOnlineCenter.GetActiveUsers ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::GetRoomUserList --> call MucOnlineCenter.GetActiveUsers ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	for(MucUserIdentitySeq::iterator logit = ans.begin(); logit != ans.end(); ++logit){
		MCE_DEBUG("MucGateManagerI::GetRoomUserList --> UserIdentity : " << mucUserIdentityToString((*logit)) << " userid = " << (*logit)->userid);
	}
	MCE_INFO("interface GetRoomUserList usetime : " << ts.getTime());
  return ans;
}

MucActiveUserSeq MucGateManagerI::GetUserByPermision(const MucRoomIdPtr& roomid, Permisions permision, const Ice::Current& currnet){
	TimeStat ts;
	MucActiveUserSeq ans;
	try{
		ans = MucOnlineCenterAdapter::instance().GetUserByPermision(roomid, permision);
	}catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::GetUserByPermision --> call MucOnlineCenter.GetActiveUsers ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::GetUserByPermision --> call MucOnlineCenter.GetActiveUsers ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface GetUserByPermision usetime : " << ts.getTime());
	return ans;
}

//IM && PAGER(groups.renren.com)
void MucGateManagerI::UserOnline(const JidPtr& onlinejid, const Ice::Current& current){
	TimeStat ts;
  try{
    MCE_DEBUG("MucGateManagerI::UserOnline --> CALL MucUserRoomAdapter.UserOnline jid = " << jidToString(onlinejid));
    MucUserRoomAdapter::instance().UserOnline(onlinejid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::UserOnline --> call MucUserRoomAdapter.UserOnline ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::UserOnline --> call MucUserRoomAdapter.UserOnline ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface UserOnline usetime : " << ts.getTime());
}


void MucGateManagerI::UserOffline(const JidPtr& offlinejid, const Ice::Current& current){
	TimeStat ts;
  try{
    MCE_DEBUG("MucGateManagerI::UserOffline --> CALL MucUserRoomAdapter.UserOffline jid = " << jidToString(offlinejid));
    MucUserRoomAdapter::instance().UserOffline(offlinejid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::UserOffline --> call MucUserRoomAdapter.UserOffline ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::UserOffline --> call MucUserRoomAdapter.UserOffline ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface UserOffline usetime : " << ts.getTime());
}

MucRoomIdSeq MucGateManagerI::GetUserRooms(int userid, const string& domain, const Ice::Current& current){
	TimeStat ts;
	MCE_DEBUG("MucGateManagerI::GetUserRooms --> userid = " << userid << " domain = " << domain);
	MucRoomIdSeq roomseq;
	try{
    roomseq = MucUserRoomAdapter::instance().GetUserRooms(userid, domain);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::GetUserRooms --> call MucUserRoomAdapter.GetUserRooms ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::GetUserRooms --> call MucUserRoomAdapter.GetUserRooms ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface GetUserRooms usetime : " << ts.getTime());
	return roomseq;
}

void MucGateManagerI::AwayRoom(const JidPtr& opjid, const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
  if(TEMPORARY == roomid->domain){
		try{
	  	MCE_DEBUG("MucGateManagerI::AwayRoom -->  CALL MucUserRoom.AwayRoomForTemporary domain = " << TEMPORARY << " awayjid = " << jidToString(opjid) << " awayroom = " << roomid->roomname);
    	MucUserRoomAdapter::instance().AwayRoomForTemporary(opjid, roomid);
  	}
  	catch(Ice::Exception& e){
    	MCE_WARN("MucGateManagerI::AwayRoom--> call MucUserRoom.AwayRoomForTemporary ERR  line:"<<__LINE__<<" err:"<<e);
  	}
  	catch(std::exception& e){
    	MCE_WARN("MucGateManagerI::AwayRoom--> call MucOnlineCenter.AwayRoomForTemporary ERR  line:"<<__LINE__<<" err:"<<e.what());
  	}
	}
	else if(PERMANENT == roomid->domain){
		try{
	  	MCE_DEBUG("MucGateManagerI::AwayRoom --> CALL MucOnlineCenter.AwayRoom domain = " << PERMANENT << " awayjid = " << jidToString(opjid) << " awayroom = " << roomid->roomname);
    	MucOnlineCenterAdapter::instance().AwayRoom(opjid, roomid);
  	}
  	catch(Ice::Exception& e){
    	MCE_WARN("MucGateManagerI::AwayRoom--> call MucOnlineCenter.AwayRoom ERR  line:"<<__LINE__<<" err:"<<e);
  	}
  	catch(std::exception& e){
    	MCE_WARN("MucGateManagerI::AwayRoom--> call MucOnlineCenter.AwayRoom ERR  line:"<<__LINE__<<" err:"<<e.what());
  	}
	}
	MCE_INFO("interface AwayRoom usetime : " << ts.getTime());
}
void MucGateManagerI::SendTalkMsg(const JidPtr& opjid, const MucRoomIdPtr& roomid, const string& msg, const Ice::Current& current){
	TimeStat ts;
	bool spamok = false;
	try{
		TimeStat ts_spam;
		vector<string> v;
		v.push_back(msg);
		CheckResultMore spamobj = OzeinFilterClient::instance().checkerAndFilter(opjid->userId, -1, 108, v, "");
		if(spamobj.getFlag() != 30){
			spamok = true;
		}
		MCE_INFO("interface OzeinFilter usetime : " << ts_spam.getTime() << " spamflag : " << spamobj.getFlag());
	}
	catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::SendTalkMsg--> call OzeinFilterClient.checkerAndFilter ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::SendTalkMsg--> call OzeinFilterClient.checkerAndFilter ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	if(spamok){
		try{
			MCE_DEBUG("MucGateManagerI::SendTalkMsg --> CALL MucOnlineCenterAdapter.SendTalkMessage jid = " << jidToString(opjid) << " roomname = " << roomid->roomname << " msg = " << msg);
			MucOnlineCenterAdapter::instance().SendTalkMessage(opjid, roomid, msg);
		}
		catch(Ice::Exception& e){
			MCE_WARN("MucGateManagerI::SendTalkMsg--> call MucOnlineCenter.SendTalkMsg ERR  line:"<<__LINE__<<" err:"<<e);
		}
		catch(std::exception& e){
			MCE_WARN("MucGateManagerI::SendTalkMsg--> call MucOnlineCenter.SendTalkMsg ERR  line:"<<__LINE__<<" err:"<<e.what());
		}
	}else{
		MCE_INFO("MucGateManagerI::SendTalkMsg --> userid : " << opjid->userId << " msg : " << msg << " could not send after spam");
	}
	MCE_INFO("interface SendTalkMsg usetime : " << ts.getTime());
}

//------------------------RESPONSE----------------------------------------------------------------------------------------------------



void MucGateManagerI::NotifyUserEnterRoom(const MucActiveUserPtr& activeuser, const MucActiveUserSeq& presenceUsers, int mucstatus, const Ice::Current& current){
	TimeStat ts;
/*  if(activeuser->jid->endpoint[0] != 'T'){
    return;
  }*/
  string activeuser_identity_str = mucUserIdentityToString(activeuser->identity);
  string activeuser_jid_str = jidToString(activeuser->jid);
  if(activeuser_identity_str.empty() || activeuser_jid_str.empty()){
    return;
  }
	MCE_DEBUG("MucGateManagerI::NotifyUserEnterRoom --> activeuser = " << jidToString(activeuser->jid) << " mucstatus = " << mucstatus);
/*
  JidSeq active_user_jids;
  active_user_jids = OnlineCenterAdapter::instance().getUserJids(activeuser->jid->userId);
  bool notifypager = false;
  if(1 == active_user_jids.size()){
    notifypager = true;
  }
*/
	bool pager_get_data = false;
	string active_online_type = "0";
	string active_user_name = "";
	string active_user_tiny = "";
	string minigroup_name = "";

	//Service Sends Presence from Existing activeusers to New activeuser 
  MessageSeq msgs;
  for(MucActiveUserSeq::const_iterator it = presenceUsers.begin(); it != presenceUsers.end(); ++it){
    if(equal((*it)->jid, activeuser->jid)){
      continue;
    }
    xml_document doc;
    xml_node presenceNode = doc.append_child();
    presenceNode.set_name("presence");
    presenceNode.append_attribute("from").set_value(mucUserIdentityToString((*it)->identity).c_str());
    presenceNode.append_attribute("to").set_value(activeuser_jid_str.c_str());
		xml_node priorityNode = presenceNode.append_child();
		priorityNode.set_name("priority");
		priorityNode.append_child(node_pcdata).set_value("0");
    xml_node xNode = presenceNode.append_child();
    xNode.set_name("x");
    xNode.append_attribute("xmlns").set_value("http://jabber.org/protocol/muc#user");
    xml_node itemNode = xNode.append_child();
    itemNode.set_name("item");
    itemNode.append_attribute("affiliation").set_value(Permision2Str((*it)->permision).c_str());
    itemNode.append_attribute("role").set_value(Psermision2Role((*it)->permision).c_str());
    itemNode.append_attribute("jid").set_value(jidToString((*it)->jid).c_str());
    MessagePtr msg = new Message();
    msg->from = (*it)->jid;
    msg->to = activeuser->jid;
    msg->msg = doc.xml();
    msg->type = PRESENCE_COMMON;
    msgs.push_back(msg);
  }

	//Service Sends New activeuser's Presence to All activeusers 
  for(MucActiveUserSeq::const_iterator it = presenceUsers.begin(); it != presenceUsers.end(); ++it){
    if((*it)->jid->endpoint[0] == 'T'){
      xml_document doc;
      xml_node presenceNode = doc.append_child();
      presenceNode.set_name("presence");
      presenceNode.append_attribute("from").set_value(activeuser_identity_str.c_str());
      presenceNode.append_attribute("to").set_value(jidToString((*it)->jid).c_str());
			xml_node priorityNode = presenceNode.append_child();
			priorityNode.set_name("priority");
			priorityNode.append_child(node_pcdata).set_value("0");
      xml_node xNode = presenceNode.append_child();
      xNode.set_name("x");
      xNode.append_attribute("xmlns").set_value("http://jabber.org/protocol/muc#user");
      xml_node itemNode = xNode.append_child();
      itemNode.set_name("item");
      itemNode.append_attribute("affiliation").set_value(Permision2Str(activeuser->permision).c_str());
      itemNode.append_attribute("role").set_value(Psermision2Role(activeuser->permision).c_str());
      //itemNode.append_attribute("jid").set_value(jidToString((*it)->jid).c_str());
      itemNode.append_attribute("jid").set_value(activeuser_jid_str.c_str());
      if(equal(activeuser->jid, (*it)->jid)) {
        xml_node sNode110 = xNode.append_child();
        sNode110.set_name("status");
        sNode110.append_attribute("code").set_value("110");
        xml_node sNode210 = xNode.append_child();
        sNode210.set_name("status");
        sNode210.append_attribute("code").set_value("210");
      }
      MessagePtr msg = new Message();
      msg->from = activeuser->jid;
      msg->to = (*it)->jid;
      msg->msg = doc.xml();
      msg->type = PRESENCE_COMMON;
      msgs.push_back(msg);
    }
    else if((*it)->jid->endpoint[0] == 'W'){
			if(!pager_get_data){
				int irid = GetIdFromMucRoomId(activeuser->identity->roomid);
				GroupNamePtr gn = ServiceI::instance().locateObject<GroupNamePtr>(GN,irid);
				if(gn){
					minigroup_name = gn->groupname;
				}
    		try{
      		TalkUserPtr userptr = TalkCacheClient::instance().GetUserByIdWithLoad(activeuser->jid->userId);
					if(userptr){
      			active_user_name = userptr->name;
      			active_user_tiny= userptr->tinyurl;
					}
    		}
    		catch(Ice::Exception& e){
      		MCE_WARN("MucGateManagerI::NotifyEnterRoom --> call TalkCacheClient.GetUserByIdWithLoad ERR  line:"<<__LINE__<<" err:"<<e);
    		}
    		catch(std::exception& e){
      		MCE_WARN("MucGateManagerI::NotifyEnterRoom --> call TalkCacheClient.GetUserByIdWithLoad ERR  line:"<<__LINE__<<" err:"<<e.what());
    		}
				active_online_type = boost::lexical_cast<string>(mucstatus);
				pager_get_data = true;
			}
			if(!mucstatus || active_user_name.empty() || active_user_tiny.empty()){
				MCE_DEBUG("MucGateManagerI::NotifyEnterRoom --> mucstatus == 0 or name or tiny is empty so conniue");
				continue;
			}
	    xml_document doc;
      xml_node presenceNode = doc.append_child();
      presenceNode.set_name("presence");
      presenceNode.append_attribute("from").set_value(activeuser_identity_str.c_str());
      presenceNode.append_attribute("to").set_value(jidToString((*it)->jid).c_str());
			presenceNode.append_attribute("type").set_value("entergroup");
			xml_node onlineTypeNode = presenceNode.append_child();
			onlineTypeNode.set_name("onlineType");
			onlineTypeNode.append_child(node_pcdata).set_value(active_online_type.c_str());
			xml_node nameNode = presenceNode.append_child();
			nameNode.set_name("username");
			nameNode.append_child(node_pcdata).set_value(active_user_name.c_str());
			xml_node tinyNode = presenceNode.append_child();
			tinyNode.set_name("tinyurl");
			tinyNode.append_child(node_pcdata).set_value(active_user_tiny.c_str());
			xml_node roomNameNode = presenceNode.append_child();
			roomNameNode.set_name("roomname");
			roomNameNode.append_child(node_pcdata).set_value(minigroup_name.c_str());
	    MessagePtr msg = new Message();
 	    msg->from = activeuser->jid;
  	  msg->to = (*it)->jid;
    	msg->msg = doc.xml();
      msg->type = MUC_PRESENCE;
      msgs.push_back(msg);
    }
  }

	if(PERMANENT == activeuser->identity->roomid->domain && mucstatus){
		try{
			MCE_DEBUG("MucGateManagerI::NotifyUserEnterRoom --> call PageCache.MemberStatusChange userid = " << activeuser->jid->userId<< " roomid = " << mucRoomIdToString(activeuser->identity->roomid));
			PageCacheAdapter::instance().MemberStatusChange(GetIdFromMucRoomId(activeuser->identity->roomid), activeuser->jid->userId, mucstatus);
		}
		catch(Ice::Exception& e){
			MCE_WARN("MucGateManagerI::NotifyUserEnterRoom --> call PageCache.MemberStatusChange ERR  line:"<<__LINE__<<" err:"<<e);
		}
		catch(std::exception& e){
			MCE_WARN("MucGateManagerI::NotifyUserEnterRoom --> call PageCache.MemberStatusChange ERR  line:"<<__LINE__<<" err:"<<e.what());
		}
	}


	try{
		MCE_INFO("MucGateManagerI::EnterRoom--> CALL MucDeliver.deliver seq.size = " << msgs.size());
		MucDeliverAdapter::instance().deliver(msgs);
	} catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::EnterRoom--> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::EnterRoom--> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e.what());
  }

	MCE_INFO("interface EnterRoom usetime : " << ts.getTime());
}

void MucGateManagerI::NotifyUserAwayRoom(const MucActiveUserPtr& activeuser, const JidSeq& presenceUsers, int mucstatus, const Ice::Current& current){
	TimeStat ts;
/*  if(activeuser->jid->endpoint[0] != 'T'){
    return;
  }*/
  string activeuser_identity_str = mucUserIdentityToString(activeuser->identity);
  string activeuser_jid_str = jidToString(activeuser->jid);
  if(activeuser_identity_str.empty() || activeuser_jid_str.empty()){
    return;
  }
	MCE_DEBUG("MucGateManagerI::NotifyUserAwayRoom --> activeuser = " << jidToString(activeuser->jid) << " mucstatus = " << mucstatus);
	bool pager_get_data = false;
	string active_online_type = "0";
	string minigroup_name = "";
  MessageSeq msgs;
  for(JidSeq::const_iterator it = presenceUsers.begin(); it != presenceUsers.end(); ++it){
    if((*it)->endpoint[0] == 'T'){
      xml_document doc;
      xml_node presenceNode = doc.append_child();
      presenceNode.set_name("presence");
      presenceNode.append_attribute("from").set_value(activeuser_identity_str.c_str());
      presenceNode.append_attribute("to").set_value(jidToString((*it)).c_str());
      presenceNode.append_attribute("type").set_value("unavailable");
      xml_node xNode = presenceNode.append_child();
      xNode.set_name("x");
      xNode.append_attribute("xmlns").set_value("http://jabber.org/protocol/muc#user");
      xml_node itemNode = xNode.append_child();
      itemNode.set_name("item");
      itemNode.append_attribute("affiliation").set_value(Permision2Str(activeuser->permision).c_str());
      itemNode.append_attribute("role").set_value("none");
      if(equal(activeuser->jid, (*it))) {
        xml_node statNode = xNode.append_child();
        statNode.set_name("status");
        statNode.append_attribute("code").set_value("110");
      }
      MessagePtr msg = new Message();
      msg->from = activeuser->jid;
      msg->to = (*it);
      msg->msg = doc.xml();
      msg->type = PRESENCE_COMMON;
      msgs.push_back(msg);
    }
    else if((*it)->endpoint[0] == 'W'){
			if(!pager_get_data){
				int irid = GetIdFromMucRoomId(activeuser->identity->roomid);
				GroupNamePtr gn = ServiceI::instance().locateObject<GroupNamePtr>(GN,irid);
				if(gn){
					minigroup_name = gn->groupname;
				}
				active_online_type = boost::lexical_cast<string>(mucstatus);
				pager_get_data = true;
			}
	    xml_document doc;
      xml_node presenceNode = doc.append_child();
      presenceNode.set_name("presence");
      presenceNode.append_attribute("from").set_value(activeuser_identity_str.c_str());
      presenceNode.append_attribute("to").set_value(jidToString((*it)).c_str());
			presenceNode.append_attribute("type").set_value("leftgroup");
			xml_node onlineTypeNode = presenceNode.append_child();
			onlineTypeNode.set_name("onlineType");
			onlineTypeNode.append_child(node_pcdata).set_value(active_online_type.c_str());
			xml_node roomNameNode = presenceNode.append_child();
			roomNameNode.set_name("roomname");
			roomNameNode.append_child(node_pcdata).set_value(minigroup_name.c_str());


	    MessagePtr msg = new Message();
 	    msg->from = activeuser->jid;
  	  msg->to = (*it);
    	msg->msg = doc.xml();
      msg->type = MUC_PRESENCE;
      msgs.push_back(msg);
    }
  }
	
	//notify PageCache for group.talk.xiaonei.com
	if(PERMANENT == activeuser->identity->roomid->domain){
		try{
			MCE_DEBUG("MucGateManagerI::NotifyUserAwayRoom --> call PageCache.MemberStatusChange userid = " << activeuser->jid->userId << " roomid = " << mucRoomIdToString(activeuser->identity->roomid));
			PageCacheAdapter::instance().MemberStatusChange(GetIdFromMucRoomId(activeuser->identity->roomid), activeuser->jid->userId, mucstatus);
		}
		catch(Ice::Exception& e){
			MCE_WARN("MucGateManagerI::NotifyUserAwayRoom --> call PageCache.MemberStatusChange ERR  line:"<<__LINE__<<" err:"<<e);
		}
		catch(std::exception& e){
			MCE_WARN("MucGateManagerI::NotifyUserAwayRoom --> call PageCache.MemberStatusChange ERR  line:"<<__LINE__<<" err:"<<e.what());
		}
	}
	
	try{
		MCE_INFO("MucGateManagerI::AwayRoom--> CALL MucDeliver.deliver seq.size = " << msgs.size());
		MucDeliverAdapter::instance().deliver(msgs);
	} catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::AwayRoom--> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::AwayRoom--> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface AwayRoom usetime : " << ts.getTime());
}

void MucGateManagerI::NotifyGroupChat(const MucActiveUserPtr& activeuser, const JidSeq& presenceUsers, const string& msg, const Ice::Current& current){
	if(msg.empty()){
		return;
	}
	MCE_DEBUG("MucGateManagerI::NotifyGroupChat ");
	TaskManager::instance().execute(new NotifyGroupChatTask(activeuser, presenceUsers, msg));
}


void MucGateManagerI::NotifySetPermision(const MucActiveUserPtr& opuser, const JidSeq& presenceUsers, const MucActiveUserSeq& targetusers, Permisions permision, const Ice::Current& current){
	TaskManager::instance().execute(new NotifySetPermisionTask(opuser, presenceUsers, targetusers, permision));
}

void MucGateManagerI::InviteIMUsers(const MucRoomIdPtr& roomid, const JidSeq& imusers, const Ice::Current& current){
	//TaskManager::instance().execute(new InviteIMUsersTask(roomid, imusers));
}


void MucGateManagerI::NotifyPrivateChat(const MucActiveUserPtr& sender, int targetuser, const string& msg, const Ice::Current& current){
	TaskManager::instance().execute(new NotifyPrivateChatTask(sender, targetuser, msg));
}
void MucGateManagerI::NotifyNicknameError(const MucRoomIdPtr& roomid, const JidPtr& jid, const Ice::Current& current){
	//TaskManager::instance().execute(new NotifyNicknameErrorTask(roomid, jid));
}

void MucGateManagerI::NotifyChatSetingChange(const MucRoomIdPtr& roomid, const JidSeq& webjids, int set, const Ice::Current& current){
	string roomstr = mucRoomIdToString(roomid);
	string strset = boost::lexical_cast<string>(set);
  if(roomstr.empty() || strset.empty()){
    return;
  }
	string minigroup_name = "";
	int irid = GetIdFromMucRoomId(roomid);
	GroupNamePtr gn = ServiceI::instance().locateObject<GroupNamePtr>(GN,irid);
	if(gn){
		minigroup_name = gn->groupname;
	}
  MessageSeq seq;
  JidSeq::const_iterator jit = webjids.begin();
  for(; jit != webjids.end(); ++jit){
    xml_document doc;
		xml_node presenceNode = doc.append_child();
		presenceNode.set_name("presence");
		presenceNode.append_attribute("from").set_value(roomstr.c_str());
		presenceNode.append_attribute("to").set_value(jidToString((*jit)).c_str());
		presenceNode.append_attribute("type").set_value("settingchange");
		xml_node setNode = presenceNode.append_child();
		setNode.set_name("set");
		setNode.append_child(node_pcdata).set_value(strset.c_str());
		xml_node roomNameNode = presenceNode.append_child();
		roomNameNode.set_name("roomname");
		roomNameNode.append_child(node_pcdata).set_value(minigroup_name.c_str());
    MessagePtr msg = new Message();
    msg->from = (*jit);
    msg->to = (*jit);
    msg->msg = doc.xml();
		MCE_DEBUG("MucGateManagerI::NotifyChatSetingChange --> dox.xml() = " << doc.xml());
    msg->type = MUC_PRESENCE;
    seq.push_back(msg);
  }
	try{
		MCE_INFO("MucGateManagerI::NotifyChatSetingChange --> CALL MucDeliver.deliver seq.size = " << seq.size());
		MucDeliverAdapter::instance().deliver(seq);
	} catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::NotifyChatSetingChange --> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::NotifyChatSetingChange --> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
}
