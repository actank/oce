#include <boost/lexical_cast.hpp>
#include "MessageTask.h"
#include "TalkCommon.h"
#include "IceLogger.h"
#include "XmppTools.h"
#include "PresenceAdapter.h"
#include "OnlineCenterAdapter.h"
#include "CommonTask.h"
#include "TalkDeliverAdapter.h"
#include "MessageHandlerI.h"
#include "MessageHolderAdapter.h"
#include <boost/lexical_cast.hpp>
#include "TalkLoginAdapter.h"
#include "OccupantsAdapter.h"
#include "TalkRoomAdapter.h"
#include "MucDeliverAdapter.h"
#include "TalkProxyAdapter.h"
#include "TalkStatAdapter.h"
#include "MessageType.h"
#include "RestLogicAdapter.h"
#include "TalkRecordAdapter.h"
#include "UgcCommentAdapter.h"
#include "UGCShareLogicAdapter.h"
#include "UgcGossipAdapter.h"
#include "MucGateAdapter.h"
#include "UserNameAdapter.h"
#include "talk/SpamClient/src/OzeinFilterAdapter.h"
using namespace com::xiaonei::ozein::content::generation;
using namespace com::xiaonei::talk::muc;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace MyUtil;
using namespace mop::hi::svc::adapter;
using namespace ::talk::adapter;
using namespace talk::occupants::adapter;
using namespace talk::room::adapter;
using namespace talk::mucdeliver::adapter;
using namespace com::xiaonei::talk::adapter;
using namespace com::xiaonei::talk::record;
using namespace talk::stat;
using namespace talk::online::adapter;
using namespace talk::rest;
using namespace xce::log;
using namespace xce::ucgcomment;
using namespace com::ugcshare;
using namespace xce::adapter::username;

void MessageTask::handle() {
  StatFunc statF("MessageTask::handle");
  //MCE_INFO("@@@@@@@@@@@ --> " << _doc->xml());
  xml_node messageNode = _doc->child("message");
  string from = messageNode.attribute("from").value();
  string to = messageNode.attribute("to").value();
  string type = messageNode.attribute("type").value();

  size_t pos1 = to.find("@");
  size_t pos2 = to.find_last_of("/");
  string domain = to.substr(pos1 + 1, pos2 - pos1 - 1);
  MCE_DEBUG("MessageTask::handle --> domain=" << domain);
  if (type == "common") {
    xml_node messageNode = _doc->child("message");
    xml_node xfeedNode = messageNode.child("xfeed");
    if (xfeedNode) {
      type = xfeedNode.attribute("type").value();
      pos1 = type.find("_");
      string xfeed_type = type.substr(0, pos1);
      MCE_DEBUG("MessageTask::handle --> message_type: common   xfeed_type: "
          << xfeed_type);
      if (xfeed_type == "reply")
        _processReply();
      else if (xfeed_type == "share") {
        _processShare();
      }
    }
    return;
  } else if (type == "groupchat") {
    MCE_DEBUG("MessageTask::handle --> group chat");
    _processRoomTalk();
    return;
  } else if (_doc->child("message").child("x").child("invite").attribute("to").value()
      != "") {
    MCE_DEBUG("MessageTask::handle --> invite");
    _processInvite();
    return;
  } else if ((domain != "talk.xiaonei.com") && (domain != "talk.renren.com")) {
    MCE_DEBUG("MessageTask::handle --> private msg");
    _processRoomTalkPrivateMsg();
    return;
  }

  JidPtr toJid = stringToJid(to);

  // for command
  if ((_jid->userId == 128487631 || _jid->userId == 227805680)
      && (toJid->userId == 128487631 || toJid->userId == 200865373)) {
    MCE_DEBUG("MessageTask::handle() --> msg:" << _doc->xml());
    string msg = _doc->child("message").child("body").first_child().value();
    if (!msg.empty()) {
      if (msg.find("[cmd:") == 0 && msg[msg.length() - 1] == ']') {
        string cmd, userid;
        int stat = 0;
        for (size_t i = 5; i < msg.length() - 1; ++i) {
          if (msg[i] == ':') {
            stat = 1;
            continue;
          }
          if (stat == 0) {
            cmd += msg[i];
          } else {
            userid += msg[i];
          }
        }

        int id = -1;
        try {
          id = boost::lexical_cast<int>(userid);
        } catch (...) {

        }

        if (cmd == "offline" && id > 0) {
					JidSeq jseq;
					try{
          //JidSeq jseq = PresenceAdapter::instance().getOnlineUserJidSeq(id);
          	JidSeq jseq = OnlineCenterAdapter::instance().getUserJids(id);
					}
					catch(Ice::Exception& e){
						MCE_WARN("MessageTask::handle-->OnlineCenterAdapter::getUserJids-->" << e);
					}
          for (size_t i = 0; i < jseq.size(); ++i) {
						try{
            	TalkLoginAdapter::instance().unbind(jseq.at(i));
						}
						catch(Ice::Exception& e){
							MCE_WARN("MessageTask::handle-->TalkLoginAdapter::unbind-->" << e);
						}
          }
        }
        MCE_INFO(msg);
        return;
      }
    }
  }
  if (from == "") {
    messageNode.append_attribute("from") = jidToString(_jid).c_str();
  }
  if (to == "") {
    return;
  }
  messageNode.remove_attribute("to");

	//------------Call Spam-------------------
  if (!_jid->endpoint.empty() && _jid->endpoint.at(0) != 'G' && _doc->child("message").child("body")) {
    if (!_doc->child("message").child("sha:shake")) {
			try{
				string msg = _doc->child("message").child("body").first_child().value();
				bool spam_ok = true;
				TimeStat ts_spam; 
				vector<string> v; 
				v.push_back(msg); 
				CheckResultMore spamobj = OzeinFilterClient::instance().checkerAndFilter(_jid->userId, -1, 108, v, ""); 
				if(spamobj.getFlag() == 30){
					spam_ok = false;
				}
				if(!spam_ok){
					MCE_INFO("MessageTask::handle --> do not send after spam ! userid = " << _jid->userId << " msg = " << msg);
					MONITOR("SignForTimeuse::MessageTask::handle-->CallSpam-->" << ts_spam.getTime());
					return;
				}else{
					MONITOR("SignForTimeuse::MessageTask::handle-->CallSpam-->" << ts_spam.getTime());
					MCE_INFO("MessageTask::handle --> call OzeinFilterClient.checkerAndFilter usetime = " << ts_spam.getTime());
				}
			}
			catch(Ice::Exception& e){
				MCE_WARN("MessageTask::handle-->OzeinFilterClient::instance-->checkerAndFilter-->" << e);
			}catch(...){
				MCE_WARN("MessageTask::handle --> call spam error");
			}
    }
	}
	//----------------------------------------



  if (_doc->child("message").child("body")) {
    try {
      string path;
      switch (_jid->endpoint.at(0)) {
      case 'T':
        path = "USE_XNTALK";
        break;
      case 'W':
        path = "USE_WEBPAGGER";
        break;
      case 'P':
        path = "USE_PHONE";
        break;
      default:
        path = "USE_XNTALK";
      }
      ostringstream os;
      os << "USER_" << toJid->userId;
      ActiveTrack::instance().action(_jid->userId, path, "TALK", os.str());
    } catch (Ice::Exception& e) {
      MCE_WARN("MessageTask::handle --> invoke ActiveTrack err, " << e);
    } catch (...) {
      MCE_WARN("MessageTask::handle --> invoke ActiveTrack err");
    }
  }
  JidSeq jseq;
  try {
    //jseq = PresenceAdapter::instance().getOnlineUserJidSeq(toJid->userId);
    jseq = OnlineCenterAdapter::instance().getUserJids(toJid->userId);
  } catch (Ice::Exception& e) {
    MCE_WARN("MessageTask::handle-->OnlineCenterAdapter::getUserJids-->" << e);
  }

  for (JidSeq::iterator it = jseq.begin(); it != jseq.end();) {
    if ((*it)->index == 1) {
      it = jseq.erase(it);
    } else {
      ++it;
    }
  }
  /*
   StatManager::instance().incMsgCount();
   if (jseq.empty()) {
   StatManager::instance().incMsgCount(true);
   }
   */
  Ice::Long msgKey; //= MessageHandlerI::instance().getMessageKey();
  bool isRealMsg = false;
  if (_doc->child("message").child("body")) {
    msgKey = MessageHandlerI::instance().getMessageKey();
    if (!_doc->child("message").child("sha:shake")) {
      isRealMsg = true;
    }
    string msg = _doc->child("message").child("body").first_child().value();
    MCE_INFO("MessageTask::handle    FROM:" << jidToString(_jid) << " TO:"
        << jidToString(toJid) << " KEY:" << msgKey << " --> " << msg);

    //发送到留言板
    int res = 0;
    if (!_jid->endpoint.empty() && _jid->endpoint.at(0) != 'G') {
      try {
        //过滤闪屏
        if (!_doc->child("message").child("sha:shake")) {
          res = UgcGossipAdapter::instance().send(_jid->userId, toJid->userId,
              msg);
        }
      } catch (Ice::Exception& e) {
        MCE_WARN("MessageTask::handle-->UgcGossipAdapter::send-->from"<< _jid->userId << " to:" << toJid->userId << ", err" << e);
      }
    }
    MCE_INFO("MessageTask::handle --> invoke ugc gossip, from:" << _jid->userId
        << " to:" << toJid->userId << ", " << res);
    try {
      TalkRecordAdapter::instance().record(_jid->userId, toJid->userId, msg);
    } catch (Ice::Exception& e) {
        MCE_WARN("MessageTask::handle-->TalkRecordAdapter::record-->" << e);
    }
  } else {
    msgKey = -1;
  }
  MessageSeq seq;
  MCE_DEBUG("MessageTask::handle --> size:" << jseq.size());

  //加尾吧
  string xml_message = _doc->xml();
  size_t pos = xml_message.find("</body>");
  if (pos != string::npos) {
    xml_message.insert(pos,
        "（此消息发自“人人桌面”，即时稳定/不丢消息，自动保存聊天记录：http://im.renren.com）");

  }

  for (size_t i = 0; i < jseq.size(); ++i) {
		MCE_DEBUG("index1:" << jseq.at(i)->index << "    to index:" << toJid->index);
		if(_jid->endpoint.empty()){
			continue;
		}
    //if (!_jid->endpoint.empty() && _jid->endpoint.at(0) == 'G' && !jseq.at(i)->endpoint.empty() && jseq.at(i)->endpoint.at(0) == 'P') {
    //  continue;
    //}

    if (jseq.at(i)->index == toJid->index) {
      MessagePtr msg = new Message();
      msg -> from = _jid;
      msg -> to = toJid;
      if (_jid->endpoint.at(0) == 'T' && jseq.at(i)->endpoint.at(0) == 'W'
          && FirstSend2WTimer::instance().IsSend(_jid->userId,
              jseq.at(i)->userId)) {
        msg->msg = xml_message;
        FirstSend2WTimer::instance().Push(_jid->userId, jseq.at(i)->userId);
      } else {
        msg->msg = _doc->xml();
      }
      msg -> msg = _doc->xml();
      msg -> msgKey = msgKey;

      if (isRealMsg) {
				try{
          MessageHolderAdapter::instance().message(msg);
				}
				catch(Ice::Exception& e){
					MCE_WARN("MessageTask::handle-->MessageHolderAdapter::message-->" << e);
				}
				try{
					if(jseq.at(i)->endpoint.at(0) == 'W'){
						MCE_INFO("ugcstat --> deliver msg to pager from:" << jidToString(_jid) << " to:" << jidToString(jseq.at(i)));
					}
          TalkDeliverAdapter::instance().deliver(msg);
				}
				catch(Ice::Exception& e){
					MCE_WARN("MessageTask::handle-->TalkDeliverAdapter::deliver-->" << e);	
				}
      } else if (!toJid->endpoint.empty() && toJid->endpoint.at(0) == 'T') {
				try{
          TalkDeliverAdapter::instance().deliver(msg);
				}
				catch(Ice::Exception& e){
					MCE_WARN("MessageTask::handle-->TalkDeliverAdapter::deliver-->" << e);
				}
      }
      MCE_INFO("MessageTask::handle --> direct deliver from:" << jidToString(
          _jid) << " to:" << jidToString(jseq.at(i)));
      return;
    }
    if (isRealMsg || (!jseq.at(i)->endpoint.empty() && jseq.at(i)->endpoint.at(
        0) == 'T')) {
      MCE_INFO("MessageTask::handle --> brodcast deliver from:" << jidToString( _jid) << " from jid->userid = " << _jid->userId << " from jid->endpoint = " << _jid->endpoint << " from jid->index = " << _jid->index << " to:" << jidToString(jseq.at(i)) << " messageKey:" << msgKey);

      MessagePtr msg = new Message();
      msg->from = _jid;
      msg->to = jseq.at(i);
			
      if (!_jid->endpoint.empty() && _jid->endpoint.at(0) == 'T' && jseq.at(i)->endpoint.at(0) == 'W'
          && FirstSend2WTimer::instance().IsSend(_jid->userId,
              jseq.at(i)->userId)) {
        msg->msg = xml_message;
        FirstSend2WTimer::instance().Push(_jid->userId, jseq.at(i)->userId);
      } else {
        msg->msg = _doc->xml();
      }
      msg->type = MESSAGE;
      msg->msgKey = msgKey;
      seq.push_back(msg);
    }
  }
	//MCE_INFO("MessageTask::handle --> from = " << _jid->userId << " seq.size = " << seq.size());
  if (seq.empty()) {
    MessagePtr msg = new Message();
    msg->from = _jid;
    msg->to = toJid;
    msg->msg = _doc->xml();
    msg->type = MESSAGE;
    msg->msgKey = msgKey;

    if (isRealMsg) {
			try{
        MessageHolderAdapter::instance().message(msg);
			}
			catch(Ice::Exception& e){
			  MCE_WARN("MessageTask::handle-->MessageHolderAdapter::message-->" << e);	
			}
    }
  } else {
    if (isRealMsg) {
      MessagePtr msg = new Message();
      msg->from = seq.at(0)->from;
      msg->to = new Jid;
      msg->to->userId = seq.at(0)->to->userId;
      msg->to->index = -1;
      msg->msg = seq.at(0)->msg;
      msg->type = seq.at(0)->type;
      msg->msgKey = seq.at(0)->msgKey;
			try{
        MessageHolderAdapter::instance().message(msg);
			}
			catch(Ice::Exception& e){
				MCE_WARN("MessageTask::handle --> call MessageHolderAdapter::message err" << e);
			}
    }
		try{
			for(MessageSeq::iterator mit = seq.begin(); mit != seq.end(); ++mit){
				MessagePtr m = (*mit);
				if(!(m->to->endpoint.empty()) && ('W' == m->to->endpoint[0])){
					MCE_INFO("ugcstat --> deliver msg to pager from:" << jidToString(_jid) << " to:" << jidToString(m->to));
				}
			}
      TalkDeliverAdapter::instance().deliver(seq);
		}
		catch(Ice::Exception& e){
			MCE_WARN("MessageTask::handle-->TalkDeliverAdapter::deliver-->" << e);
		}
  }
}

void MessageTask::_processRoomTalkPrivateMsg() {
	xml_node messageNode = _doc->child("message");
	string from = messageNode.attribute("from").value();
	string to = messageNode.attribute("to").value();
	string msg = messageNode.child("body").first_child().value();
	if(msg.empty()){
		return;
	}
	MucUserIdentityPtr identity = stringToMucUserIdentity(to);
	if(!identity){
		return;
	}
	try{
		MCE_DEBUG("Message::_processRoomTaklPrivateMsg CALL MucGate.SendPrivateMsg from :" << from << "  TO:" << to << " msg :" << msg);
		MucGateAdapter::instance().SendPrivateMsg(_jid, identity , msg);
	} catch (Ice::Exception& e) {
    MCE_WARN("MessageTask::_processRoomTalkPrivateMsg-->MucGateAdapter::SendPrivateMsg-->" << e);
  } catch (...) {
    MCE_WARN("MessageTask::_processRoomTalkPrivateMsg --> invoke MucGate.SendPrivateMsg err");
  }
	return;
	/*
  xml_node messageNode = _doc->child("message");
  string from = messageNode.attribute("from").value();
  string to = messageNode.attribute("to").value();
  MCE_DEBUG("MessageTask::_processRoomTalkPrivateMsg -->" << "  TO:" << to);

  if (to == "") {
    return;
  }
  MCE_DEBUG("MessageTask::handle    FROM:" << jidToString(_jid) << "  TO:"
      << to);

  RoomJidPtr roomJid = stringToRoomJid(to);
  OccupantPtr occupantFrom;
  OccupantPtr occupantTo;
  try {
    occupantTo = OccupantsAdapter::instance().getOccupant(roomJid);
    occupantFrom = OccupantsAdapter::instance().getOccupantByJid(roomJid->rid,
        _jid);
  } catch (Ice::Exception& e) {
    MCE_WARN("MessageTask::_processRoomTalkPrivateMsg --> invoke Occupants "
        << " " << "Ice::Exception:" << e);
  } catch (...) {
    MCE_WARN("MessageTask::_processRoomTalkPrivateMsg --> invoke Occupants err");
  }
  MCE_DEBUG(
      "MessageTask::_processRoomTalkPrivateMsg --> invoke Occupants succeed");

  messageNode.append_attribute("from")
      = roomJidToString(occupantFrom->rJid).c_str();

  Ice::Long msgKey = MessageHandlerI::instance().getMessageKey();
  MessageSeq seq;

  //MCE_DEBUG("MessageTask::_processRoomTalkPrivateMsg --> jids=" << occupantTo->jids.size());
  //for(size_t j=0; j<occupantTo->jids.size(); j++){
  MessagePtr msg = new Message();
  msg->from = _jid;
  msg->to = occupantTo->fJid;
  messageNode.append_attribute("to") = jidToString(occupantTo->fJid).c_str();

  msg->msg = _doc->xml();
  msg->type = 1111;
  msg->msgKey = msgKey;
  MCE_DEBUG(" " << _doc->xml());
  seq.push_back(msg);
  //}

  MCE_DEBUG("MessageTask::_processRoomTalk --> msg count " << seq.size());
  MucDeliverAdapter::instance().deliver(seq);
	*/
}

void MessageTask::_processRoomTalk() {

  xml_node messageNode = _doc->child("message");
  string from = messageNode.attribute("from").value();
  string to = messageNode.attribute("to").value();
	string msg = messageNode.child("body").first_child().value();
  MCE_DEBUG("MessageTask::_processRoomTalk -->" << "  TO:" << to);
  if (to.empty() || msg.empty()) {
    return;
  }

	MucRoomIdPtr mucroomid = stringToMucRoomId(to);
	try{
		MCE_DEBUG("Message::_processRoomTalk  CALL SendTalkMessage FROM:" << jidToString(_jid) << "  TO:" << to << " msg :" << msg);
		MucGateAdapter::instance().SendTalkMsg(_jid, mucroomid, msg);
	} catch (Ice::Exception& e) {
    MCE_WARN("MessageTask::_processRoomTalk-->MucGateAdapter::SendTalkMsg-->" << e);
  } catch (...) {
    MCE_WARN("MessageTask::_processRoomTalk --> call MucGateAdapter::SendTalkMsg --> invoke MucGateAdapter err");
  }
	return;
  
/*	
	
	
	RoomIdPtr roomId = stringToRoomId(to);
  OccupantSeq occupants;
  try {
    occupants = OccupantsAdapter::instance().getOccupants(roomId);
  } catch (Ice::Exception& e) {
    MCE_WARN("MessageTask::_processRoomTalk --> invoke Occupants " << " "
        << "Ice::Exception:" << e);
  } catch (...) {
    MCE_WARN("MessageTask::_processRoomTalk --> invoke Occupants err");
  }
  MCE_DEBUG(
      "MessageTask::_processRoomTalk --> invoke Occupants succeed, count="
          << occupants.size());

  OccupantPtr fromOccupant;
  for (size_t i = 0; i < occupants.size(); i++) {
    if (jidToString(occupants.at(i)->fJid) == jidToString(_jid)) {
      fromOccupant = occupants.at(i);
      break;
    }
  }
  if (!fromOccupant) {
    MCE_DEBUG("MessageTask::_processRoomTalk --> occupant does not exist");
    return;
  }
  MCE_DEBUG("MessageTask::_processRoomTalk --> " << roomJidToString(
      fromOccupant->rJid));
  messageNode.append_attribute("from")
      = roomJidToString(fromOccupant->rJid).c_str();

  Ice::Long msgKey = MessageHandlerI::instance().getMessageKey();
  MessageSeq seq;
  for (size_t i = 0; i < occupants.size(); i++) {
    OccupantPtr occupant = occupants.at(i);
    //for(size_t j=0; j<occupant->jids.size(); j++){
    MessagePtr msg = new Message();
    msg->from = _jid;
    msg->to = occupant->fJid;
    messageNode.attribute("to").set_value(jidToString(occupant->fJid).c_str());
    msg->msg = _doc->xml();
    msg->type = 1111;
    msg->msgKey = msgKey;
    MCE_DEBUG(" " << _doc->xml());
    seq.push_back(msg);
    //}
  }
  MCE_DEBUG("MessageTask::_processRoomTalk --> msg count " << seq.size());
  TalkStatAdapter::instance().mucMsgCount(1, seq.size());
  MucDeliverAdapter::instance().deliver(seq);
*/
}

void MessageTask::_processInvite() {
	xml_node messageNode = _doc->child("message");
	string to = messageNode.attribute("to").value();
	MucRoomIdPtr roomid = stringToMucRoomId(to);
	if(!roomid){
		return;
	}
  xml_node iNode = messageNode.child("x").child("invite");
	string inviteTo = iNode.attribute("to").value();
	MCE_DEBUG("MessageTask::_processInvite --> invite to " << inviteTo);
	JidPtr toJid = stringToJid(inviteTo);
	if(!toJid){
		return;
	}
	try{
		MCE_DEBUG("Message::__processInvite CALL MucGate.UserInviteUser FROM:" << jidToString(_jid) << "  TO:" << to << " inviteTo :" << inviteTo);
		MucGateAdapter::instance().UserInviteUser(_jid, roomid, toJid->userId);
	} catch (Ice::Exception& e) {
    MCE_WARN("MessageTask::_processInvite-->MucGateAdapter::UserInviteUser-->" << e);
  } catch (...) {
    MCE_WARN("MessageTask::_processInvite --> invoke MucGateAdapter err");
  }
	return;
	/*
  xml_node messageNode = _doc->child("message");
  string from = messageNode.attribute("from").value();
  string to = messageNode.attribute("to").value();
  string id = messageNode.attribute("id").value();
  MCE_DEBUG("MessageTask::_processInvite -->" << "  TO:" << to);
  if (to == "") {
    return;
  }
  RoomIdPtr roomId = stringToRoomId(to);
	*/
  /*	RoomPtr room;
   try {
   MCE_DEBUG("MessageTask::_processInvite --> invoke getRoom, " << roomIdToString(roomId));
   room = TalkRoomAdapter::instance().getRoom(roomId);
   } catch(Ice::Exception& e) {
   MCE_WARN("MessageTask::_processInvite --> invoke getRoom " << " " <<"Ice::Exception:"<<e);
   } catch(...) {
   MCE_WARN("MessageTask::_processInvite --> invoke getRoom err");
   }
   if(!room){
   MCE_DEBUG("MessageTask::_processInvite --> room does not exist");
   return;
   }
   MCE_DEBUG("RoomManagerI::roomPresence --> room type, hidden="
   << room->type->hidden
   << ", memberOnly=" << room->type->memberOnly
   << ", moderated=" << room->type->moderated
   << ", anonymousType=" << room->type->anonymousType
   << ", secured=" << room->type->secured
   << ", persistent=" << room->type->persistent
   );

   MCE_WARN("MessageTask::_processInvite --> invoke getRoom success");
   */
  /*xml_node tNode = messageNode.child("x").child("invite");
  string inviteTo = tNode.attribute("to").value();
  MCE_DEBUG("MessageTask::_processInvite --> invite to " << inviteTo);
  JidPtr toJid = stringToJid(inviteTo);
	
  OccupantSeq occupants;
	*/
  //	bool flag = false;
  /*	if(room->type->memberOnly){
   try {
   RoomMemberPtr roomMember = new RoomMember;
   roomMember->rid = roomId;
   roomMember->userId = toJid->userId;
   roomMember->aType = Member;

   tmpOccupant = OccupantsAdapter::instance().getOccupant();
   MCE_DEBUG("MessageTask::_processInvite --> invoke addRoomMember");
   occupants = OccupantsAdapter::instance().setAffiliation(_jid, roomMember);
   } catch(Ice::Exception& e) {
   MCE_WARN("MessageTask::_processInvite --> invoke addRoomMember " << " " <<"Ice::Exception:"<<e);
   } catch(...) {
   MCE_WARN("MessageTask::_processInvite --> invoke addRoomMember err");
   }
   }
   //MCE_WARN("MessageTask::_processInvite --> invoke addRoomMember success");
   if(occupants.empty()){
   MCE_DEBUG("MessageTask::_processInvite --> add member error");
   //return;
   }*/
/*
  bool suc = false;
  try {
    suc = OccupantsAdapter::instance().invite(_jid, roomId, toJid);
  } catch (Ice::Exception& e) {
    MCE_WARN("MessageTask::_processInvite --> invoke invite " << " "
        << "Ice::Exception:" << e);
  } catch (...) {
    MCE_WARN("MessageTask::_processInvite --> invoke invite err");
  }
  if (!suc) {
    ostringstream os;
    os << "<message from='" << to << "' to='" << jidToString(_jid)
        << "' type='error' id='" << id << "'>"
        << "<x xmlns='http://jabber.org/protocol/muc#user'>" << "<invite to='"
        << inviteTo << "'/>" << "</x>" << "<error code='405' type='cancel'>"
        << "<not-allowed xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
        << "</error>" << "</message>";

    TalkProxyAdapter::instance().express(_jid, os.str());
    return;
  }

  JidSeq jseq;
  try {
    jseq = OnlineCenterAdapter::instance().getUserJids(toJid->userId);
  } catch (Ice::Exception& e) {
    MCE_WARN("MessageTask::_processInvite --> do get JidSeq err:" << e);
  }
  MCE_DEBUG("MessageTask::_processInvite --> getOnlineUserJidSeq count "
      << jseq.size());

  messageNode.append_attribute("from") = to.c_str();
  messageNode.append_attribute("type") = "normal";
  tNode.remove_attribute("to");
  tNode.append_attribute("from") = jidToString(_jid, true).c_str();
  Ice::Long msgKey = MessageHandlerI::instance().getMessageKey();
  MessageSeq seq;
  for (JidSeq::iterator it = jseq.begin(); it != jseq.end(); ++it) {
    if ((*it)->endpoint.at(0) == 'T') {
      messageNode.append_attribute("to") = jidToString(*it, true).c_str();

      MessagePtr msg = new Message();
      msg->from = _jid;
      msg->to = *it;
      msg->msg = _doc->xml();
      msg->type = MESSAGE;
      msg->msgKey = msgKey;
      MCE_DEBUG(" " << _doc->xml());
      seq.push_back(msg);
    }
  }
  MCE_DEBUG("MessageTask::_processInvite --> msg count " << seq.size());
  MucDeliverAdapter::instance().deliver(seq);
*/
}

void MessageTask::_processReply() {
  xml_node messageNode = _doc->child("message");
  xml_node xfeedNode = messageNode.child("xfeed");
  if (!xfeedNode) {
    MCE_WARN("MessageTask::_processReply--> protol error");
    return;
  }

  string strReplyType = xfeedNode.attribute("type").value();

  string strResId = xfeedNode.child("resid").first_child().value();
  string strOwnerId = xfeedNode.child("ownerid").first_child().value();
  string strReplyId = xfeedNode.child("replyid").first_child().value();
  string content = xfeedNode.child("content").first_child().value();
  string strType = xfeedNode.child("type").first_child().value();//悄悄话
  if (strResId.empty() && strReplyType == "reply_status") {
    //兼容老的协议
    strResId = xfeedNode.child("statusid").first_child().value();
  }
	MCE_INFO("MessageTask::_processReply --> " << strReplyType << " userid:" << _jid->userId << " resid:" << strResId << " ownerId:" << strOwnerId << " replyId:" << strReplyId << " content:" << content << " type:" << strType);
	MONITOR("SignForTimeuse::MessageTask::_processReply-->" << strReplyType << " userid:" << _jid->userId << " resid:" << strResId << " ownerId:" << strOwnerId << " replyId:" << strReplyId << " content:" << content << " type:" << strType);
  Ice::Long resid = 0;
  int ownerid = 0;
  int replyid = 0;
  int type = 0;
  if (strResId.empty() || strOwnerId.empty() || content.empty()) {
    MCE_INFO("MessageTask::_processReplyStatus --> parameter incomplete");
    return;
  }
  try {
    resid = boost::lexical_cast<Ice::Long>(strResId.c_str());
    ownerid = boost::lexical_cast<int>(strOwnerId.c_str());
    if (!strReplyId.empty()) {
      replyid = boost::lexical_cast<int>(strReplyId.c_str());
    }
    if (!strType.empty()) {
      type = boost::lexical_cast<int>(strType.c_str());
    }
  } catch (...) {
    MCE_WARN("MessageTask::_processReplyStatus --> lexical cast error");
    return;
  }

  try {
    if (strReplyType == "reply_status") {
			try{
        RestLogicAdapter::instance().replyStatus(_jid->userId, resid, ownerid,
          replyid, content);
			}
			catch(Ice::Exception& e){
				MCE_WARN("MessageTask::_processReply-->RestLogicAdapter::replyStatus-->" << e);
			}
      //UgcCommentAdapter::instance().addReply(_jid->userId, ownerid, STATUS_REPLY, resid, replyid, content);
			try{
      TalkStatAdapter::instance().incReplyStatusCount();
			}
			catch(Ice::Exception& e){
			  MCE_WARN("MessageTask::_processReply-->TalkStatAdapter::incReplyStatusCount-->" << e);	
			}
    } 
		else{
			if(replyid){//guoqing.liu    添加  “回复xxx” 的前缀
				string toname = "";
				UserNameInfoPtr un;
				try{
					un = UserNameAdapter::instance().getUserName(replyid);
				}
				catch(Ice::Exception& e){
					MCE_WARN("MessageTask::_processReply-->UserNameAdapter::getUserName-->error replyid:" << replyid << " exception:" << e);
				}
				if(un){
					content = "回复"+un->name()+"："+content;
				}
			}
			if (strReplyType == "reply_blog") {
				//RestLogicAdapter::instance().replyBlog(_jid->userId, resid, ownerid, replyid, content, type);
				UgcCommentAdapter::instance().addReply(_jid->userId, ownerid, BLOG_REPLY,
						resid, replyid, content);
				TalkStatAdapter::instance().incReplyBlogCount();
			} else if (strReplyType == "reply_photo") {
				//RestLogicAdapter::instance().replyPhoto(_jid->userId, resid, ownerid,    replyid, content, type);
				UgcCommentAdapter::instance().addReply(_jid->userId, ownerid, PHOTO_REPLY, resid, replyid, content);
				TalkStatAdapter::instance().incReplyPhotoCount();
			} else if (strReplyType == "reply_album") {
				try{
					RestLogicAdapter::instance().replyAlbum(_jid->userId, resid, ownerid,
							replyid, content, type);
				}
				catch(Ice::Exception& e){
					MCE_INFO("call RestLogicAdapter::replyAlbum err" << e);	
				}
				//UgcCommentAdapter::instance().addReply(_jid->userId, ownerid, ALBUM_REPLY, resid, replyid, content);
			} else if (strReplyType == "reply_share") {
				UgcCommentAdapter::instance().addReply(_jid->userId, ownerid, SHARE_REPLY, resid, replyid, content);
				MCE_INFO("MessageTask::_processReply-->from:" << _jid->userId << " owner:" << ownerid << " ReplyType:" << SHARE_REPLY << " sourceId:" << resid << " to:" << replyid << " body:" << content);
				TalkStatAdapter::instance().incReplyShareCount();
			}
		}
  } catch (UGCCommentNewException& e) {
		MONITOR("SignForTimeuse::UgcCommentAdapter::addReplyerror-->UgcCommenterror-->" << e.msg);
		if (strReplyType == "reply_blog") {
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_blog UgcCommenterror" << e.msg)
		} else if (strReplyType == "reply_photo") {
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << "reply_photo UgcCommenterror" << e.msg)
		} else if (strReplyType == "reply_share") {
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << "reply_share UgcCommenterror" << e.msg)
		} 
  }catch(Ice::ConnectTimeoutException& e){
		MONITOR("SignForTimeuse::UgcCommentAdapter::addReplyerror-->ConnectTimeoutException-->" << e);
		if (strReplyType == "reply_blog") {
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_blog ConnectTimeoutException" << e)
		} else if (strReplyType == "reply_photo") {
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_photo ConnectTimeoutException" << e)
		} else if (strReplyType == "reply_share") {
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_share ConnectTimeoutException" << e)
		} else if (strReplyType == "reply_status") {
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_status ConnectTimeoutException" << e)
		} else if (strReplyType == "reply_status") {
			MCE_WARN("MessageTask::_processReply--> exception at reply_status userid = " << _jid->userId << " ConnectTimeoutException error : " << e)
		}
	}catch (Ice::TimeoutException& e) {
		if (strReplyType == "reply_blog") {
			TalkStatAdapter::instance().incReplyBlogCount();
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_blog TimeoutException" << e)
		} else if (strReplyType == "reply_photo") {
			TalkStatAdapter::instance().incReplyPhotoCount();
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_photo TimeoutException" << e)
		} else if (strReplyType == "reply_share") {
			TalkStatAdapter::instance().incReplyShareCount();
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_share TimeoutException" << e)
		} else if (strReplyType == "reply_status") {
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_status TimeoutException" << e)
		} else if (strReplyType == "reply_status") {
			MCE_WARN("MessageTask::_processReply--> exception at reply_status userid = " << _jid->userId << " TimeoutException error : " << e)
		}
  }catch(Ice::Exception& e){
		MONITOR("SignForTimeuse::UgcCommentAdapter::addReplyerror-->IceException-->" << e);
		if (strReplyType == "reply_blog") {
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_blog exception" << e)
		} else if (strReplyType == "reply_photo") {
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_photo exception" << e)
		} else if (strReplyType == "reply_share") {
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_share exception" << e)
		} else if (strReplyType == "reply_status") {
			MCE_WARN("MessageTask::_processReply-->UgcCommentAdapter::addReply-->userid=" << _jid->userId << " reply_status exception" << e)
		} else if (strReplyType == "reply_status") {
			MCE_WARN("MessageTask::_processReply--> exception at reply_status userid = " << _jid->userId << " exception error : " << e)
		}
	}
}

void MessageTask::_processShare() {
  xml_node xfeedNode = _doc->child("message").child("xfeed");
  string strShareType = xfeedNode.attribute("type").value();
  string strResId = xfeedNode.child("resid").first_child().value();
  string strOwnerId = xfeedNode.child("ownerid").first_child().value();
  string comment = xfeedNode.child("comment").first_child().value();
  MCE_INFO("MessageTask::_processShare --> type: " << strShareType << " client_ip:" << _ip << " userid:" << _jid->userId << " resid:" << strResId << " ownerId:" << strOwnerId << "comment :" << comment);
  MONITOR("SignForTimeuse::MessageTask::_processShare --> type: " << strShareType << " client_ip:" << _ip << " userid:" << _jid->userId << " resid:" << strResId << " ownerId:" << strOwnerId << "comment :" << comment);
  Ice::Long resid = 0;
  int ownerid = 0;
  if (strResId.empty() || strOwnerId.empty()) {
    MCE_INFO("MessageTask::_processShare --> parameter incomplete");
    return;
  }
  try {
    resid = boost::lexical_cast<Ice::Long>(strResId.c_str());
    ownerid = boost::lexical_cast<int>(strOwnerId.c_str());
  } catch (...) {
    MCE_WARN("MessageTask::_processShare --> lexical cast error");
    return;
  }

  try {
    if (strShareType == "share_createinternal") {
      int intCreateType = 0;
      string strCreateType = xfeedNode.child("sharetype").first_child().value();
      if (strCreateType == "blog") {
        intCreateType = 1;
      } else if (strCreateType == "album") {
        intCreateType = 8;
      } else if (strCreateType == "photo") {
        intCreateType = 2;
      }
      MCE_INFO("share_createinternal : intCteateType = " << intCreateType);
      UGCShareLogicAdapter::instance().createInternalShare(resid, ownerid,
          intCreateType, _jid->userId, _ip, comment);
    } else if (strShareType == "share_createshare") {
      MCE_INFO("share_createsharetoshare : intCteateType");
      UGCShareLogicAdapter::instance().createShareToShare(resid, ownerid,
          _jid->userId, _ip, comment);
    }
  } catch (UGCShareException& e) {
		MONITOR("SignForTimeuse::UGCShareLogic-->ShareError-->UGCShareException");
		if (strShareType == "share_createinternal") {
			MCE_WARN("MessageTask::_processShare-->UGCShareLogicAdapter::createInternalShare-->UGCShareException" << e.msg);
		}else if (strShareType == "share_createshare") {
			MCE_WARN("MessageTask::_processShare-->UGCShareLogicAdapter::createShareToShare-->UGCShareException" << e.msg);
		}
  }catch(Ice::ConnectTimeoutException& e){
		MONITOR("SignForTimeuse::UGCShareLogic-->ShareError-->ConnectTimeoutException");
		if (strShareType == "share_createinternal") {
			MCE_WARN("MessageTask::_processShare-->UGCShareLogicAdapter::createInternalShare-->ConnectionException" << e);
		}else if (strShareType == "share_createshare") {
			MCE_WARN("MessageTask::_processShare-->UGCShareLogicAdapter::createShareToShare-->ConnectionException" << e);
		}
	} catch (Ice::Exception& e) {
		if (strShareType == "share_createinternal") {
			MCE_WARN("MessageTask::_processShare-->UGCShareLogicAdapter::createInternalShare-->" << e);
		}else if (strShareType == "share_createshare") {
			MCE_WARN("MessageTask::_processShare-->UGCShareLogicAdapter::createShareToShare-->" << e);
		}
  }

}

void MucTestTask::handle(){
	MucRoomIdSeq rooms;
	try{
		rooms = MucGateAdapter::instance().GetUserRooms(_jid->userId, "group.talk.xiaonei.com");
	}catch(Ice::Exception& e){
		MCE_WARN("MucTestTask::handle --> call MucGate::GetUserRooms error : " << e);
	}
	if(rooms.empty()){
		return;
	}
	for(MucRoomIdSeq::iterator rit = rooms.begin(); rit != rooms.end(); ++rit){
		try{
			for(int i = 0; i < 10; ++i){
				try{
				MucGateAdapter::instance().GetRoomUserList((*rit));
				}
				catch(Ice::Exception& e){
				MCE_WARN("MucTestTask::handle --> call MucGate.SendTalkMsg error" << e);	
				}
			}
			for(int i = 0; i < 20; ++i){
				MucGateAdapter::instance().SendTalkMsg(_jid, (*rit), "1");
			}
		}catch(Ice::Exception& e){
			MCE_WARN("MucTestTask::handle-->MucGate.SendTalkMsg-->" << e);
		}
	}
}

