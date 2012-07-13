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
//#include "OzeinFilterAdapter.h"
using namespace com::xiaonei::ozein::content::generation;
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


void NotifyGroupChatTask::handle(){
	string activeuser_identity_str = mucUserIdentityToString(activeuser->identity);
  string activeuser_jid_str = jidToString(activeuser->jid);
	int gid = GetIdFromMucRoomId(activeuser->identity->roomid);
  if(activeuser_identity_str.empty() || activeuser_jid_str.empty()){
    return;
  }
	if('T' == activeuser->jid->endpoint[0] && PERMANENT == activeuser->identity->roomid->domain){
		return;
	}
	bool load_group_name = false;
	string minigroup_name = "";
	
	MCE_DEBUG("NotifyGroupChatTask::handle");
	MessageSeq seq;
	for(JidSeq::iterator it = presenceUsers.begin(); it != presenceUsers.end(); ++it){
		xml_document doc;
		xml_node messageNode = doc.append_child();
		messageNode.set_name("message");
		messageNode.append_attribute("from") = activeuser_identity_str.c_str();
		messageNode.append_attribute("to") = "";
		messageNode.append_attribute("type").set_value("groupchat");
		xml_node bodyNode = messageNode.append_child();
		bodyNode.set_name("body");
		bodyNode.append_child(node_pcdata).set_value(msg.c_str());
		if('W' == (*it)->endpoint[0]){
			if(!load_group_name){
				GroupNamePtr gn = ServiceI::instance().locateObject<GroupNamePtr>(GN,gid);
				if(gn){
					minigroup_name = gn->groupname;
				}
				load_group_name = true;
			}
			xml_node roomNameNode = messageNode.append_child();
			roomNameNode.set_name("roomname");
			MCE_DEBUG("NotifyGroupChatTask::handle --> minigroup_name = " << minigroup_name);
			roomNameNode.append_child(node_pcdata).set_value(minigroup_name.c_str());
		}
		MessagePtr message = new Message();
		message->from = activeuser->jid;
		message->to = (*it);
		messageNode.attribute("to").set_value(jidToString((*it)).c_str());
		message->msg = doc.xml();
		MCE_DEBUG("NotifyGroupChatTask::handle -->doc = " << doc.xml());
		MCE_DEBUG("NotifyGroupChatTask::handle -->doc = " << doc.child("message").child("roomname").first_child().value());
		message->type = MUC_MESSAGE;//none
		message->msgKey = -1;
		seq.push_back(message);
	}
	if(PERMANENT == activeuser->identity->roomid->domain){
		bool spamok = false;
		try{
			TimeStat ts_spam;
			vector<string> v;
			v.push_back(msg);
			CheckResultMore spamobj = OzeinFilterClient::instance().checkerAndFilter(activeuser->jid->userId, -1, 109, v, "");
			if(!spamobj.getFlag()){
				spamok = true;
			}
			MCE_INFO("interface OzeinFilter usetime : " << ts_spam.getTime() << " spamflag : " << spamobj.getFlag());
		}
		catch(Ice::Exception& e){
			MCE_WARN("NotifyGroupChatTask::handle --> call OzeinFilterClient.checkerAndFilter ERR  line:"<<__LINE__<<" err:"<<e);
		}
		catch(std::exception& e){
			MCE_WARN("NotifyGroupChatTask::handle --> call OzeinFilterClient.checkerAndFilter ERR  line:"<<__LINE__<<" err:"<<e.what());
		}
		if(spamok){
			try{
				MucTalkRecordAdapter::instance().RecordTalkMsg(activeuser->jid->userId, gid, msg);
			} catch(Ice::Exception& e){
				MCE_WARN("NotifyGroupChatTask::handle --> call MucTalkRecordAdapter.RecordTalkMsg ERR  line:"<<__LINE__<<" err:"<<e);
			}
			catch(std::exception& e){
				MCE_WARN("NotifyGroupChatTask::handle--> call MucTalkRecordAdapter.RecordTalkMsg ERR  line:"<<__LINE__<<" err:"<<e.what());
			}
		}
	}
	try{
		MucDeliverAdapter::instance().deliver(seq);
	} catch(Ice::Exception& e){
    MCE_WARN("NotifyGroupChatTask::handle --> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("NotifyGroupChatTask::handle--> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void NotifySetPermisionTask::handle(){
	MucActiveUserSeq::const_iterator targetit = targetusers.begin();
  MessageSeq seq;
  for(; targetit != targetusers.end(); ++targetit){//每个即将改变权限的用户
    string from_identity_str = mucUserIdentityToString((*targetit)->identity);
    string from_jid_str = jidToString((*targetit)->jid);
    if(from_identity_str.empty() || from_jid_str.empty()){
      continue;
    }
    JidSeq::const_iterator jit = presenceUsers.begin();
    for(; jit != presenceUsers.end(); ++jit){
      if((*jit)->endpoint[0] == 'T'){
        xml_document doc;
        xml_node presenceNode = doc.append_child();
        presenceNode.set_name("presence");
        presenceNode.append_attribute("from").set_value(from_identity_str.c_str());
        presenceNode.append_attribute("to").set_value(jidToString((*jit)).c_str());
				if(permision < PMember){
        	presenceNode.append_attribute("type").set_value("unavailable");
				}
        xml_node xNode = presenceNode.append_child();
        xNode.set_name("x");
        xNode.append_attribute("xmlns").set_value("http://jabber.org/protocol/muc#user");
        xml_node itemNode = xNode.append_child();
        itemNode.set_name("item");
        itemNode.append_attribute("affiliation").set_value(MucGateManagerI::instance().Permision2Str(permision).c_str());
        itemNode.append_attribute("role").set_value(MucGateManagerI::instance().Psermision2Role(permision).c_str());
        if(equal((*targetit)->jid, (*jit)) && permision < PMember){
          xml_node actorNode = itemNode.append_child();
					actorNode.set_name("actor");
          actorNode.append_attribute("jid").set_value(jidToString(opuser->jid).c_str());
        }
        else{
          itemNode.append_attribute("jid").set_value(from_jid_str.c_str());
        }
        MessagePtr msg = new Message();
        msg->from = opuser->jid;
        msg->to = (*jit);
        msg->msg = doc.xml();
        msg->type = PRESENCE_COMMON;//none
        seq.push_back(msg);
      }
    }
  }
	try{
		MCE_INFO("MucGateManagerI::NotifySetPermision--> CALL MucDeliver.deliver seq.size = " << seq.size());
		MucDeliverAdapter::instance().deliver(seq);
	} catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::NotifySetPermision--> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e);
  }catch(std::exception& e){
    MCE_WARN("MucGateManagerI::NotifySetPermision--> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------


void InviteIMUsersTask::handle(){
	MCE_DEBUG("InviteIMUsersTask::handle --> BEGIN");
	string roomstr = mucRoomIdToString(roomid);
  if(roomstr.empty()){
    return;
  }
  MessageSeq seq;
  JidSeq::const_iterator jit = imusers.begin();
  for(; jit != imusers.end(); ++jit){
		MCE_DEBUG("InviteIMUsersTask::handle--> invite imusers invite_jid is = " << jidToString((*jit)));
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
    inviteNode.append_attribute("from").set_value("group@talk.renren.com");
    //TODO here from is who
    MessagePtr msg = new Message();
    msg->from = (*jit);
    msg->to = (*jit);
    msg->msg = doc.xml();
    msg->type = MESSAGE;//none
    msg->msgKey = -1;
    seq.push_back(msg);
  }
	try{
		MCE_INFO("InviteIMUsersTask::handle--> CALL MucDeliver.deliver seq.size = " << seq.size());
		MucDeliverAdapter::instance().deliver(seq);
	} catch(Ice::Exception& e){
    MCE_WARN("InviteIMUsersTask::handle--> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("InviteIMUsersTask::handle--> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
}

void NotifyPrivateChatTask::handle(){
  string sender_identity_str = mucUserIdentityToString(sender->identity);
  if(sender_identity_str.empty()){
    return;
  }
	MCE_DEBUG("MucGateManagerI::NotifyPrivateChat--> senderjid = " << jidToString(sender->jid) << " roomid = " << mucRoomIdToString(sender->identity->roomid) << " targetuser = " << targetuser << " msg = " << msg);
	JidSeq imusers;
  try{
    imusers = OnlineCenterAdapter::instance().getUserJids(targetuser, 4);
		MCE_DEBUG("MucGateManagerI::NotifyPrivateChat --> CALL ONLINECENTER  get IM jids.size = " << imusers.size());
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::NotifyPrivateChat --> call OnlineCenterAdapter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::NotifyPrivateChat --> call OnlineCenterAdapter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MessageSeq seq;
  JidSeq::const_iterator jit = imusers.begin();
  for(; jit != imusers.end(); ++jit){
    xml_document doc;
    xml_node messageNode = doc.append_child();
		messageNode.set_name("message");
    messageNode.append_attribute("from").set_value(sender_identity_str.c_str());
    messageNode.append_attribute("to").set_value(jidToString((*jit)).c_str());
		messageNode.append_attribute("type").set_value("chat");
    xml_node bodyNode = messageNode.append_child();
		bodyNode.set_name("body");
		bodyNode.append_child(node_pcdata).set_value(msg.c_str());
    MessagePtr msg = new Message();
    msg->from = sender->jid;
    msg->to = (*jit);
    msg->msg = doc.xml();
    msg->type = MESSAGE;//none
    msg->msgKey = -1;
    seq.push_back(msg);
  }
	try{
		MCE_INFO("MucGateManagerI::NotifyPrivateChat --> CALL MucDeliver.deliver seq.size = " << seq.size());
		MucDeliverAdapter::instance().deliver(seq);
	} catch(Ice::Exception& e){
    MCE_WARN("MucGateManagerI::NotifyPrivateChat --> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucGateManagerI::NotifyPritateChat--> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e.what());
  }

}

void CreateMiniGroupTask::handle(){
	//create a room
	MucGateManagerI::instance().CreateGroup(roomid);
	//add owner, permision == POwner
	MucGateManagerI::instance().AddGroupMember(groupowner, roomid, POwner);
	//add the invited users permison = ugc permision
	if(inviteusers.empty()){
		return;
	}
	MucGateManagerI::instance().AddGroupMemberBySeq(inviteusers, roomid, invited_permision);
}


void NotifyNicknameErrorTask::handle(){
	if(!roomid || !jid){
		return;
	}
	string from = mucRoomIdToString(roomid);
	string to = jidToString(jid);
	if(from.empty() || to.empty()){
		return;
	}
	xml_document doc;
	xml_node presenceNode = doc.append_child();
	presenceNode.set_name("presence");
	presenceNode.append_attribute("from").set_value(from.c_str());
	presenceNode.append_attribute("to").set_value(to.c_str());
	presenceNode.append_attribute("type").set_value("error");
	xml_node xNode = presenceNode.append_child();
	xNode.set_name("x");
	xNode.append_attribute("xmlns").set_value("http://jabber.org/protocol/muc");
	xml_node errorNode = presenceNode.append_child();
	errorNode.set_name("error");
	errorNode.append_attribute("type").set_value("cancel");
	xml_node conflictNode = errorNode.append_child();
	conflictNode.set_name("conflict");
	conflictNode.append_attribute("xmlns").set_value("urn:ietf:params:xml:ns:xmpp-stanzas");
	MessageSeq seq;
	MessagePtr msg = new Message();
	msg->from = jid;
	msg->to = jid;
	msg->msg = doc.xml();
	msg->type = PRESENCE_COMMON;//none
	seq.push_back(msg);
	try{
		MCE_INFO("NotifyNicknameErrorTask::handle --> CALL MucDeliver.deliver seq.size = " << seq.size());
		MucDeliverAdapter::instance().deliver(seq);
	} catch(Ice::Exception& e){
    MCE_WARN("NotifyNicknameErrorTask::handle --> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("NotifyNicknameErrorTask::handle --> call MucDeliver.deliver ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
}


