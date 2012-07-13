#include <boost/lexical_cast.hpp>
#include "PresenceTask.h"
//#include "TalkRightAdapter.h"
#include "IceLogger.h"
#include "TaskManager.h"
#include "BuddyCoreAdapter.h"
#include "XmppTools.h"
#include "CommonTask.h"
//#include "UserBasicAdapter.h"
#include "PresenceHandlerI.h"
#include "BuddyByIdReplicaAdapter.h"
#include "PresenceAdapter.h"
#include "XmppErrorCode.h"
#include "TalkRoomAdapter.h"
#include "OccupantsAdapter.h"
#include "TalkDeliverAdapter.h"
#include "MucDeliverAdapter.h"
#include "RestLogicAdapter.h"
//#include "OnlineStatDispatchAdapter.h"
#include "OnlineCenterAdapter.h"
#include "TalkProxyAdapter.h"
#include "MucGateAdapter.h"
#include "MucUserRoomAdapter.h"

#include "FeedMemcProxy/client/talk_cache_client.h"
using namespace xce::talk;
using namespace com::xiaonei::talk::muc;
using namespace com::xiaonei::talk::cache;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::adapter;
using namespace com::xiaonei::service::buddy;
using namespace com::xiaonei::talk::util;
using namespace talk::error;
using namespace MyUtil;
using namespace talk::room::adapter;
using namespace talk::occupants::adapter;
using namespace talk::mucdeliver::adapter;
using namespace talk::online::adapter;
using namespace talk::rest;
//using namespace xce::adapter::userbasic;
using namespace com::xiaonei::talk::adapter;
using namespace xce::buddy::adapter;
/*
void PresenceDefultTask::handle() {
	//StatFunc statF("PresenceDefultTask::handle");

	xml_node presenceNode = _doc->child("presence");
	string id = presenceNode.attribute("id").value();
	//string toJid = presenceNode.attribute("to").value();
	string type = presenceNode.attribute("type").value();

	string ver;
	string ext;
	string show;
	xml_node cNode = presenceNode.child("c");
	if (cNode) {
		ver = cNode.attribute("ver").value();
		ext = cNode.attribute("ext").value();
	}

	if (type == "invisible") {
		show = "invisible";
	} else if(type==""){
		xml_node showNode = presenceNode.child("show");
		if (showNode) {
			show = showNode.first_child().value();
		}
	}
	string status = presenceNode.child("status").first_child().value();

	if(status.empty()){
		try {
			if(_jid){
				MCE_INFO("~~PresenceDefultTask::presence --> direct invoke OnlineCenter" << jidToString(_jid)
						<< " show:" << show
						<< " ver:" << ver
						<< " ext:" << ext);
			}

			OnlineCenterAdapter::instance().presence(_jid,onlineStat(show),ver,ext, Str2StrMap());
			PresenceAdapter::instance().presence(_jid,show,ver,ext);
		} catch(Ice::Exception& e) {
			MCE_WARN("PresenceDefultTask::handle --> invode OnlineCenter::presence err:"<<e);
		}
	}else if (_jid->index > 1) {
		if(_jid){
			MCE_INFO("~~PresenceDefultTask::presence --> invoke RestLogic" << jidToString(_jid)
					<< " show:" << show
					<< " ver:" << ver
					<< " ext:" << ext
					<< " status:" << status);
		}
		//call user to update status
		Ice::Context ctx;
		ctx["source"] = "xntalk";
		UserBasicAdapter::instance().updateDoing(_jid->userId, status, ctx);
		RestLogicAdapter::instance().setStatus(_jid, onlineStat(show), status);
	}

	//TaskManager::instance().execute(new PresenceNotifyTask(_jid,PRESENCE_COMMON,_doc));

}*/
//------------------------
void PresenceSubscribeTask::handle() {
	StatFunc statF("PresenceSubscribeTask::handle");

	xml_node presenceNode = _doc->child("presence");
	string id = presenceNode.attribute("id").value();
	string toJid = presenceNode.attribute("to").value();
	xml_node statusNode = presenceNode.child("status");
	string status;
	if (statusNode) {
		status = statusNode.first_child().value();
	}
	Applyship apply;
	apply.applicant = _jid->userId;
	apply.accepter = stringToJid(toJid)->userId;
	Ice::Context ctx;
	ctx["id"] = id;
	ctx["status"] = status;


	//---------------------------------------------add by guoqing.liu : 不论from 或者 to 的好友数超过2000的时候，都不允许调用BuddyCore.addFriend---------------
	try{
		int bc  = 0;
		bc = BuddyByIdCacheAdapter::instance().getFriendCount(apply.applicant);
		if(bc >= 2000){
			MCE_INFO("PresenceUnsubscribedTask::handle --> from user : " << apply.applicant << " has " << bc << " friends so exit");
			return;
		}
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceUnsubscribedTask::handle -->PresenceUnsubscribedTask::getFriendCount --> error : " << e);
	}

	try{
		int bc = 0;
		bc = BuddyByIdCacheAdapter::instance().getFriendCount(apply.accepter);
		if(bc >= 2000){
			MCE_INFO("PresenceUnsubscribedTask::handle --> to user : " << apply.applicant << " has " << bc << " friends so exit");
			return;
		}
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceUnsubscribedTask::handle -->PresenceUnsubscribedTask::getFriendCount --> error : " << e);
	}
	//----------------------------------------------------------------------------------------------------------------------


	MCE_INFO("PresenceSubscribeTask::handle --> from:" << apply.applicant << " to:" << apply.accepter);
  try {
		BuddyCoreAdapter::instance().addApply(apply, ApplyInfo(), ctx);
	} catch(Ice::Exception& e) {
		MCE_WARN("PresenceSubscribeTask::handle-->BuddyCoreAdapter::addApply-->type:subscribe:"<<e);
	}
}
//------------------------
void PresenceSubscribedTask::handle() {
	StatFunc statF("PresenceSubscribedTask::handle");

	xml_node presenceNode = _doc->child("presence");
	string id = presenceNode.attribute("id").value();
	string toJid = presenceNode.attribute("to").value();

	Applyship apply;
	apply.applicant = _jid->userId;
	apply.accepter = stringToJid(toJid)->userId;
	Ice::Context ctx;
	ctx["id"] = id;
	//---------------------------------------------add by guoqing.liu : 不论from 或者 to 的好友数超过2000的时候，都不允许调用BuddyCore.addFriend---------------
	try{
		int bc = 0;
		bc = BuddyByIdCacheAdapter::instance().getFriendCount(apply.applicant);
		if(bc >= 2000){
			MCE_INFO("PresenceUnsubscribedTask::handle --> from user : " << apply.applicant << " has " << bc << " friends so exit");
			return;
		}
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceUnsubscribedTask::handle -->PresenceUnsubscribedTask::getFriendCount --> error : " << e);
	}

	try{
		int bc = 0;
		bc = BuddyByIdCacheAdapter::instance().getFriendCount(apply.accepter);
		if(bc >= 2000){
			MCE_INFO("PresenceUnsubscribedTask::handle --> to user : " << apply.applicant << " has " << bc << " friends so exit");
			return;
		}
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceUnsubscribedTask::handle -->PresenceUnsubscribedTask::getFriendCount --> error : " << e);
	}
	//----------------------------------------------------------------------------------------------------------------------

	MCE_INFO("PresenceSubscribedTask::handle --> from:" << apply.applicant << " to:" << apply.accepter);
	try {
		BuddyCoreAdapter::instance().addApply(apply, ApplyInfo(), ctx);
	} catch(Ice::Exception& e) {
		MCE_WARN("PresenceSubscribedTask::handle-->BuddyCoreAdapter::addApply-->[type:subscribed]"<<e);
	}
}
//------------------------
void PresenceUnsubscribedTask::handle() {
	StatFunc statF("PresenceUnsubscribedTask::handle");

	xml_node presenceNode = _doc->child("presence");
	string id = presenceNode.attribute("id").value();
	string toJid = presenceNode.attribute("to").value();

	Applyship apply;
	apply.applicant = stringToJid(toJid)->userId;
	apply.accepter = _jid->userId;
	Ice::Context ctx;
	ctx["id"] = id;
	MCE_INFO("PresenceUnsubscribedTask::handle --> from:" << apply.applicant << " to:" << apply.accepter);
	try {
		BuddyCoreAdapter::instance().denyApply(apply, ctx);
	} catch(Ice::Exception& e) {
		MCE_WARN("PresenceUnsubscribedTask::handle-->BuddyCoreAdapter::denyApply-->[type:unsubscribed]:"<<e);
	}
}

//-------------------------
string RoomPresenceTask::_affiliationStr(Affiliation affiliation) {
	switch (affiliation) {
	case Owner:
		return "owner";
	case Admin:
		return "admin";
	case Member:
		return "member";
	case None:
		return "none";
	case Outcast:
		return "outcast";
	}
	return "none";
}

string RoomPresenceTask::_roleStr(RoomRole role) {
	switch (role) {
	case Moderator:
		return "moderator";
	case Participant:
		return "participant";
	case Visitor:
		return "visitor";
	case NoneRole:
		return "none";
	}
	return "none";
}

void RoomPresenceTask::handle() {
	MCE_DEBUG("RoomPresenceTask::handle --> ...");
	xml_node presenceNode = _doc->child("presence");
	string id = presenceNode.attribute("id").value();
	if (id == "") {
		id = boost::lexical_cast<string>(time(NULL));
	}

	string to = presenceNode.attribute("to").value();
	MCE_DEBUG(" " << _doc->xml());
	MCE_DEBUG("RoomPresenceTask::handle --> " << to);
	
	MucUserIdentityPtr identity = stringToMucUserIdentity(to);
	identity->userid = _jid->userId;
	if(!identity){
		MCE_DEBUG("RoomPresenceTask::handle --> identity is null so return");
		return;
	}
	Permisions permision = PMember;
	if("group.talk.xiaonei.com" == identity->roomid->domain){
		try{
			MucMemberPtr member = MucUserRoomAdapter::instance().GetMemberPermision(_jid->userId, identity->roomid);
			if(!member){
				MCE_DEBUG("RoomPresenceTask::handle --> member is null so return");
				return;
			}
			permision = member->permision;
		}catch(Ice::Exception& e){
			MCE_WARN(__FUNCTION__ << "RoomPresenceTask::handle-->MucUserRoomAdapter::GetMemberPermision-->" << e);
		}catch(...){
			MCE_WARN(__FUNCTION__ << " --> invoke MucUserRoom.GetMemberPermision error");
			return;
		}
	}
	//第三方客户端进入固定群会，统一昵称。
	if("group.talk.xiaonei.com" == identity->roomid->domain){
		bool changenick = true;
		string strid = boost::lexical_cast<string>(_jid->userId);
		size_t pos = identity->nickname.find(":");
		if(string::npos != pos){
			if(strid == identity->nickname.substr(pos + 1, identity->nickname.size())){
				MCE_DEBUG("RoomPresenceTask::handle --> RRDESKTOP want into the room DON'T ALLOW");
				return;
			}	
		}
		if(changenick){
			//第三方    //或者是支持固定群群聊的人人桌面进入房间
			TalkUserPtr u;
			try{
				u = TalkCacheClient::instance().GetUserByIdWithLoad(_jid->userId);
			}
			catch(Ice::Exception& e){
				MCE_WARN(__FUNCTION__ << "RoomPresenceTask::handle-->TalkCacheClient::GetUserByIdWithLoad-->" << e);
			}catch(...){
				MCE_WARN(__FUNCTION__ << " --> invoke TalkCacheClient.GetUserByIdWithLoad error");
			}
			if(!u){
				return;
			}
			identity->nickname = makeMucNickname(_jid, u->name);
		}
	}
	MucActiveUserPtr activeuser = new MucActiveUser();
	activeuser->identity = identity;
	activeuser->permision = permision;
	activeuser->jid = _jid;
	try{
		MucGateAdapter::instance().EnterRoom(activeuser);
	}catch(Ice::Exception& e){
		MCE_WARN(__FUNCTION__ << "RoomPresenceTask::handle-->MucGateAdapter::EnterRoom" << e);
	}catch(...){
		MCE_WARN(__FUNCTION__ << " --> invoke MucGate.EnterRoom error");
		return;
	}

/*	
	bool success = true;
	OccupantPtr occupant = 0;
	try{
		occupant = OccupantsAdapter::instance().getOccupantByJid(roomJid->rid, _jid);
	}catch(Ice::Exception& e){
		MCE_WARN(__FUNCTION__ << " --> invoke getOccupantByJid error, " << e);
	}catch(...){
		MCE_WARN(__FUNCTION__ << " --> invoke getOccupantByJid error");
		return;
	}

	bool isEntered = false;
	if (occupant) {
		if (occupant->rJid->nick != roomJid->nick) {
			_modifyPresence(occupant, roomJid);
			return;
		}
		isEntered = true;
	}

	RoomJidPtr toRJid = stringToRoomJid(to);

	XmppErrorCode code = service_unavailable;
	if(!isEntered) {
		try{
			code = (XmppErrorCode)OccupantsAdapter::instance().enterRoom(roomJid, _jid);
		}catch(Ice::Exception& e){
			MCE_WARN(__FUNCTION__ << " --> invoke enterRoom error, " << e);
			return;
		}catch(...){
			MCE_WARN(__FUNCTION__ << " --> invoke enterRoom error");
			return;
		}
	}

	MessageSeq msgs;

	if (code == none || code == success || isEntered) {

		MCE_DEBUG("RoomPresenceTask::handle --> invoke Occupants::getOccupants --> begin");
		OccupantSeq oseq;
		try{
			oseq = OccupantsAdapter::instance().getOccupants(toRJid->rid);
		}
		catch(Ice::Exception& e){
			MCE_WARN(__FUNCTION__ << " --> invoke getOccupants error, " << e);
		}catch(...){
			MCE_WARN(__FUNCTION__ << " --> invoke getOccupants error");
			return;
		}
		MCE_DEBUG("RoomPresenceTask::handle --> invoke Occupants::getOccupants --> "
				<< oseq.size());

		//OccupantPtr occupant;
		JidSeq jseq;
		IntSeq uids;
		for (size_t i = 0; i < oseq.size(); ++i) {
			if (equal(oseq.at(i)->fJid, _jid)) {
				occupant = oseq.at(i);
			}
			jseq.push_back(oseq.at(i)->fJid);
			uids.push_back(oseq.at(i)->fJid->userId);
		}

		for (size_t i = 0; i < oseq.size(); ++i) {
			// remove all x node
			xml_node removeNode;
			while ( removeNode = presenceNode.child("x"), removeNode) {
				presenceNode.remove_child(removeNode);
			}
			//------------------


			presenceNode.append_attribute("from").set_value(roomJidToString(occupant->rJid).c_str());
			presenceNode.append_attribute("to").set_value(jidToString(oseq.at(i)->fJid).c_str());

			xml_node xNode1 = presenceNode.append_child();
			xNode1.set_name("x");
			xNode1.append_attribute("xmlns").set_value("http://jabber.org/protocol/muc#user");

			xml_node itemNode = xNode1.append_child();
			itemNode.set_name("item");
			itemNode.append_attribute("affiliation").set_value(_affiliationStr(occupant->aType).c_str());
			itemNode.append_attribute("role").set_value(_roleStr(occupant->role).c_str());
			//if(Moderator == oseq.at(i)->role) {
				itemNode.append_attribute("jid").set_value(jidToString(_jid).c_str());
			//}

			if(code == success) {
				xml_node statNode = xNode1.append_child();
				statNode.set_name("status");
				statNode.append_attribute("code").set_value("201");
			}
			if(equal(oseq.at(i)->fJid,_jid)) {
				xml_node statNode = xNode1.append_child();
				statNode.set_name("status");
				statNode.append_attribute("code").set_value("110");
			}

			MessagePtr msg = new Message();
			msg->from = _jid;
			msg->to = oseq.at(i)->fJid;
			msg->msg = _doc->xml();
			msg->type = PRESENCE_COMMON;

			msgs.push_back(msg);
		}

		if(!isEntered) {
			AvatarSeq avatars;
			try{
				avatars = OnlineCenterAdapter::instance().getAvatars(jseq);
			}catch(Ice::Exception& e){
				MCE_WARN(__FUNCTION__ << " --> invoke getOnlineUserInfo error, "<< e);
			}
			ObjectResultPtr uInfo;
			try{
				uInfo = TalkCacheAdapter::instance().getUserList(uids,1000);
			}catch(Ice::Exception& e){
				MCE_WARN(__FUNCTION__ << " --> invoke getUserInfoList error, "<< e);
			}
			if(!uInfo){
				uInfo = new ObjectResult();
			}


			for(size_t i = 0; i < oseq.size(); ++i) {
				//Service Sends Presence from Existing Occupants to New Occupant
				if(oseq.at(i)->rJid->nick == occupant->rJid->nick) {
					continue;
				}

				ostringstream os2;
				os2 << "<presence from='"<<roomJidToString(oseq.at(i)->rJid)<<"' to='"<<jidToString(_jid)<<"' id='"<<id<<"'>"
				<< "<x xmlns='http://jabber.org/protocol/muc#user'>"
				<< "<item affiliation='"<<_affiliationStr(oseq.at(i)->aType)<<"' role='"<< _roleStr(oseq.at(i)->role)
                << "' jid='" << jidToString(oseq.at(i)->fJid)<<"'/>"
				<< "</x>";
				if(avatars.size() > i && avatars.at(i)) {
					string show = onlineStatStr(avatars.at(i)->stat);
					if(show != "") {
						os2 << "<show>"<< show <<"</show>";
					}
					if(avatars.at(i)->ext != "" && avatars.at(i)->ver != "") {
						os2 << "<c xmlns='http://jabber.org/protocol/caps' node='http://talk.xiaonei.com/xmpp/client/caps' ext='"<<avatars.at(i)->ext<<"' ver='"<<avatars.at(i)->ver<<"'/>";
					}
				}
				ObjectMap::iterator it = uInfo->data.find(uids.at(i));
				if(it != uInfo->data.end()) {
					UserPtr u = UserPtr::dynamicCast(it->second);
					if(u->statusOriginal != "") {
						os2 << "<status>"<< xmlEncode(u->statusOriginal) <<"</status>";
					}
					if(u->headurl != "") {
						os2 << "<x xmlns='vcard-temp:x:update'><photo>"
						<< "<EXTVAL>"<< buildHeadUrl(u->headurl) <<"</EXTVAL>"
						<<"</photo></x>";
					}
				}
				os2 << "</presence>";

				MessagePtr msg2 = new Message();
				msg2->from = oseq.at(i)->fJid;
				msg2->to = _jid;
				msg2->msg = os2.str();
				msg2->type = PRESENCE_COMMON;
				MCE_DEBUG(" " << os2.str());
				msgs.push_back(msg2);
			}
		}
	} else {
		//		string from = jidToString(_jid);
		//		xml_node presenceNode = _doc->child("Presence");
		//		presenceNode.attribute("from") = to.c_str();
		//		presenceNode.attribute("to") = from.c_str();
		//		presenceNode.attribute("type") = "error";
		MCE_DEBUG("RoomPresenceTask::handle --> Presence error");

		string errorNode = error(code);
		MCE_DEBUG("RoomPresenceTask::handle --> Presence error " << errorNode);
		ostringstream os;
		os << "<presence from='"<<to<<"' to='"<<jidToString(_jid)<<"' type='error' id='" << id << "'>"
		<< "<x xmlns='http://jabber.org/protocol/muc'/>"
		<< errorNode
		<<"</presence>";

		MCE_DEBUG("RoomPresenceTask::handle --> Presence error: " << os.str());
		MessagePtr msg = new Message();
		msg->from = _jid;
		msg->to = _jid;
		msg->msg = os.str();
		msg->type = PRESENCE_COMMON;
		MCE_DEBUG(" " << os.str());

		msgs.push_back(msg);

	}
	MCE_DEBUG("RoomPresenceTask::handle --> deliver msgs " << msgs.size());
	MucDeliverAdapter::instance().deliver(msgs);
	*/
}
void RoomPresenceTask::_modifyPresence(OccupantPtr occupant,
		RoomJidPtr newRoomJid) {
	return;
	MCE_DEBUG("RoomPresenceTask::_changeNick --> from "
			<< roomJidToString(occupant->rJid) << " to "
			<< roomJidToString(newRoomJid));
	bool flag = false;
	MCE_DEBUG("RoomPresenceTask::_changeNick --> invoke Occupants changeNick");
	try{
	flag = OccupantsAdapter::instance().changeNick(newRoomJid, _jid);
	}
	catch(Ice::Exception& e){
		MCE_WARN("RoomPresenceTask::_modifyPresence-->OccupantsAdapter::changeNick-->" << e);
		}
	MCE_DEBUG("RoomPresenceTask::_changeNick --> invoke Occupants changeNick"
			<< flag);

	MessageSeq msgs;
	if (false == flag) {
		ostringstream os;
		os << "<presence from='"<<roomIdToString(newRoomJid->rid)<<"' to='"
				<<jidToString(_jid) <<"' type='error' id='" << _doc->child("presence").attribute("id").value() << "'>"
				<< "<x xmlns='http://jabber.org/protocol/muc'/>"
				<< "<error type='cancel'>"
				<< "<conflict xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
				<< "</error>" <<"</presence>";

		MessagePtr msg = new Message();
		msg->from = _jid;
		msg->to = _jid;
		msg->msg = os.str();
		msg->type = PRESENCE_COMMON;
		MCE_DEBUG(" " << os.str());
		msgs.push_back(msg);

		MCE_DEBUG("RoomPresenceTask::handle --> deliver msgs " << msgs.size());
		try{
		MucDeliverAdapter::instance().deliver(msgs);
		}catch(Ice::Exception& e){
			MCE_WARN("RoomPresenceTask::handle-->MucDeliverAdapter::deliver-->err" << e);
			}
		return;
	}
	MCE_DEBUG("RoomPresenceTask::_changeNick --> invoke Occupants getOccupants");
	OccupantSeq oseq;
	try{
	oseq = OccupantsAdapter::instance().getOccupants(newRoomJid->rid);
	}catch(Ice::Exception& e){
		MCE_WARN("RoomPresenceTask::handle-->OccupantsAdapter::getOccupants-->" << e);
		}
	MCE_DEBUG("RoomPresenceTask::_changeNick --> invoke Occupants getOccupants "
			<< oseq.size());

	// remove all x node
	xml_node presenceNode = _doc->child("presence");
	xml_node removeNode;
	while (removeNode = presenceNode.child("x"), removeNode) {
		presenceNode.remove_child(removeNode);
	}
	//------------------
	for (size_t i = 0; i < oseq.size(); ++i) {
		//for(size_t j = 0; j < oseq.at(i)->jids.size(); ++j){
		ostringstream os;
		os << "<presence from='"<<roomJidToString(occupant->rJid)<<"' to='"
				<<jidToString(oseq.at(i)->fJid)<<"' type='unavailable'>"
				<< "<x xmlns='http://jabber.org/protocol/muc#user'>"
				<< "<item affiliation='"<<_affiliationStr(occupant->aType)
				<< "' nick='" << newRoomJid->nick << "' role='"
				<< _roleStr(occupant->role);
		//if (Moderator == oseq.at(i)->role) {
			os << "' jid='" << jidToString(_jid);
		//}
		os << "'/>" << "<status code='303'/>" << "</x>" << "</presence>";
		MessagePtr msg = new Message();
		msg->from = _jid;
		msg->to = oseq.at(i)->fJid;
		msg->msg = os.str();
		msg->type = PRESENCE_COMMON;
		MCE_DEBUG(" " << os.str());
		msgs.push_back(msg);

		presenceNode.append_attribute("from").set_value(roomJidToString(newRoomJid).c_str());
		presenceNode.append_attribute("to").set_value(jidToString(oseq.at(i)->fJid).c_str());
		if (i == 0) {
			xml_node xNode = presenceNode.append_child();
			xNode.set_name("x");
			xNode.append_attribute("xmlns").set_value("http://jabber.org/protocol/muc#user");

			xml_node itemNode = xNode.append_child();
			itemNode.set_name("item");
			itemNode.append_attribute("affiliation").set_value(_affiliationStr(occupant->aType).c_str());
			itemNode.append_attribute("role").set_value(_roleStr(occupant->role).c_str());
			itemNode.append_attribute("jid").set_value(jidToString(_jid).c_str());
		}
		msg = new Message();
		msg->from = _jid;
		msg->to = oseq.at(i)->fJid;
		msg->msg = _doc->xml();// os2.str();
		msg->type = PRESENCE_COMMON;
		//	MCE_DEBUG(" " << os2.str());
		msgs.push_back(msg);
		//}
	}
	MCE_DEBUG("RoomPresenceTask::handle --> deliver msgs " << msgs.size());
	try{
	MucDeliverAdapter::instance().deliver(msgs);
	}catch(Ice::Exception& e){
		MCE_WARN("RoomPresenceTask::handle-->MucDeliverAdapter::deliver-->err" << e);
		}
}
//-------------------------
string RoomPresenceUnavailableTask::_affiliationStr(Affiliation affiliation) {
	switch (affiliation) {
	case Owner:
		return "owner";
	case Admin:
		return "admin";
	case Member:
		return "member";
	case None:
		return "none";
	case Outcast:
		return "outcast";
	}
	return "none";
}

string RoomPresenceUnavailableTask::_roleStr(RoomRole role) {
	switch (role) {
	case Moderator:
		return "moderator";
	case Participant:
		return "participant";
	case Visitor:
		return "visitor";
	case NoneRole:
		return "none";
	}
	return "none";
}
void RoomPresenceUnavailableTask::handle() {
	xml_node presenceNode = _doc->child("presence");
	string toStr = presenceNode.attribute("to").value();
	MucUserIdentityPtr identity = stringToMucUserIdentity(toStr);
	if(!identity){
		return;
	}
	try{
		MCE_INFO("RoomPresenceUnavailableTask::handle --> CALL MucGate.AwayRoom _jid = " << jidToString(_jid) << " identity = " << toStr);
		MucGateAdapter::instance().AwayRoom(_jid, identity->roomid);
	}catch(Ice::Exception& e){
		MCE_WARN(__FUNCTION__ << "RoomPresenceUnavailableTask::handle-->MucGateAdapter::AwayRoom-->" << e);
	}catch(...){
		MCE_WARN(__FUNCTION__ << " --> invoke MucGate.EnterRoom error");
		return;
	}

/*
	MCE_DEBUG("RoomPresenceUnavailableTask::handle --> ...");

	xml_node presenceNode = _doc->child("presence");
	string id = presenceNode.attribute("id").value();
	if (id == "") {
		id = boost::lexical_cast<string>(time(NULL));
	}
	string to = presenceNode.attribute("to").value();
	RoomJidPtr toRJid = stringToRoomJid(to);
	if(!toRJid) {
		return;
	}
	OccupantPtr o = OccupantsAdapter::instance().exitRoom(toRJid->rid,_jid);
	if(!o) {
		MCE_DEBUG("RoomPresenceUnavailableTask::handle --> invoke Occupants::exitRoom error");
		return;
	}
	MessageSeq mseq;

	ostringstream os;
	os << "<presence from='"<<roomJidToString(o->rJid)<<"' to='"<<jidToString(_jid)<<"' type='unavailable' id='"<<id<<"'>"
	 << "<status>offline</status>"
	 << "<x xmlns='http://jabber.org/protocol/muc#user'>"
	 << "<item affiliation='"<<_affiliationStr(o->aType)<<"' role='none'/>"
	 << "</x>"
	 << "</presence>";

	MessagePtr msg = new Message();
	msg->from = _jid;
	msg->to = _jid;
	msg->type = PRESENCE_COMMON;
	msg->msg = os.str();
	mseq.push_back(msg);

	if(!o){
		MCE_DEBUG("RoomPresenceUnavailableTask::handle --> exit room error");
		return;
	}
	MCE_DEBUG("RoomPresenceTask::handle --> invoke Occupants::getOccupants --> begin" );
	OccupantSeq oseq = OccupantsAdapter::instance().getOccupants(toRJid->rid);
	MCE_DEBUG("RoomPresenceTask::handle --> invoke Occupants::getOccupants --> " << oseq.size());

	for(size_t i = 0; i < oseq.size(); ++i) {

			ostringstream os;
			os << "<presence from='"<<roomJidToString(o->rJid)<<"' to='"<<jidToString(oseq.at(i)->fJid)<<"' type='unavailable' id='"<<id<<"'>"
			 << "<status>offline</status>"
			 << "<x xmlns='http://jabber.org/protocol/muc#user'>"
			 << "<item affiliation='"<<_affiliationStr(o->aType)<<"' role='none'/>"
			 << "</x>"
			 << "</presence>";

			MessagePtr msg = new Message();
			msg->from = _jid;
			msg->to = oseq.at(i)->fJid;
			msg->msg = os.str();
			msg->type = PRESENCE_COMMON;

			mseq.push_back(msg);


	}
	MucDeliverAdapter::instance().deliver(mseq);
*/
}

void KickUserTimer::handle(){
	while(true){
		pair<int, int> p;
		{
			IceUtil::Mutex::Lock lock(_mutex);
			if(_users.empty()){
				break;
			}
			p = _users.front();
			if((time(NULL)-p.second) < 1*60){
				break;
			}
			_users.pop_front();
		}

		JidSeq jids;
		try{
			jids = OnlineCenterAdapter::instance().getUserJids(p.first);
		}catch(Ice::Exception& e){
			MCE_WARN("KickUserTimer::handle-->OnlineCenterAdapter::getUserJids-->" << e);
		}
		MCE_INFO("KickUserTimer::handle --> user:" << p.first << " onlinesize:" << jids.size() << " size:" << _users.size());
		if(jids.empty()){
			continue;
		}	
		for(int i=0; i<jids.size(); i++){
			try{
				MCE_INFO("KickUserTimer::handle -- > offline " << jidToString(jids.at(i)));
				OnlineCenterAdapter::instance().offline(jids.at(i));
			}catch(Ice::Exception& e){
				MCE_WARN("KickUserTimer::handle-->OnlineCenterAdapter::offline-->" << e << " " << jidToString(jids.at(i)));
			}
			try{
				MCE_INFO("KickUserTimer::handle -- > close " << jidToString(jids.at(i)));
				TalkProxyAdapter::instance().kick(jids.at(i));
			}catch(Ice::Exception& e){
				MCE_WARN("KickUserTimer::handle-->TalkProxyAdapter::kick-->" << e << " " << jidToString(jids.at(i)));
			}
		}
	}
}
