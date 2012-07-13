#include "MessageAnalyst.h"
#include "PresenceTask.h"
#include "PresenceHandlerI.h"
#include "XmppTools.h"

#include "UserBasicAdapter.h"
#include "RestLogicAdapter.h"
#include "OnlineCenterAdapter.h"
#include "PresenceAdapter.h"
#include "OnlineEventMediatorAdapter.h"
#include "IMWindowAdapter.h"
#include "MucGateAdapter.h"
using namespace com::xiaonei::talk::muc;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk;
using namespace talk::online::adapter;
using namespace talk::rest;
using namespace talk::window;
using namespace mop::hi::svc::adapter;
using namespace xce::mediator::onlineevent;
using namespace xce::adapter::userbasic;

void MessageAnalyst::handle(const JidPtr& jid, const string& msg) {
	StatFunc statF("MessageAnalyst::handle");
	xml_document_ptr doc = new xml_document();
	if (!doc->load(msg.c_str())) {
		return;
	}
	const char* nodeName = doc->first_child().name();
	if (nodeName != NULL) {
		PresenceTaskBuilder::instance().build(jid, doc);

	}
}
//-------------------------------------------------

void PresenceTaskBuilder::build(const JidPtr& jid, const xml_document_ptr& doc) {
	StatFunc statF("PresenceTaskBuilder::build");
	string type = doc->child("presence").attribute("type").value();
	if (type == "subscribe") {
		TaskManager::instance().execute(new PresenceSubscribeTask(jid,doc));
	} else if (type == "subscribed") {
		TaskManager::instance().execute(new PresenceSubscribedTask(jid,doc));
	} else if (type == "unsubscribed") {
		TaskManager::instance().execute(new PresenceUnsubscribedTask(jid,doc));
	} //else if (type == "unavailable"){
		//TaskManager::instance().execute(new RoomPresenceUnavailableTask(jid,doc));
	//}
	else{
		string to = doc->child("presence").attribute("to").value();
		IdType idtype = idType(to);
		if (!to.empty() && (idtype==ROOMJID||idtype==ROOMID)) {
			MCE_INFO("idtype = roomjid or roomid");
			if(type == "unavailable"){
				MCE_INFO("PresenceTaskBuilder::build --> RoomPresenceUnavailableTask");
				TaskManager::instance().execute(new RoomPresenceUnavailableTask(jid,doc));
			}
			else{
				MCE_INFO("PresenceTaskBuilder::build --> RoomPresence");
				TaskManager::instance().execute(new RoomPresenceTask(jid,doc));
			}
		}else {
			MCE_DEBUG("PresenceTaskBuilder::build --> PresenceDefult");
			//TaskManager::instance().execute(new PresenceDefultTask(jid,doc));
			//should not start a thread
			processPresence(jid, doc);
		}
	}
}

void PresenceTaskBuilder::processPresence(const JidPtr& jid, const xml_document_ptr& doc){
	xml_node presenceNode = doc->child("presence");
	string id = presenceNode.attribute("id").value();
	//string toJid = presenceNode.attribute("to").value();
	string type = presenceNode.attribute("type").value();

	string ver;
	string subver;
	string ext;
	string show;
	string secver;
	xml_node cNode = presenceNode.child("c");
	if (cNode) {
		ver = cNode.attribute("ver").value();
		ext = cNode.attribute("ext").value();
		subver = cNode.attribute("subver").value();
		secver = cNode.attribute("secver").value();
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
			MCE_INFO("~~PresenceDefultTask::presence --> direct invoke OnlineCenter " << jidToString(jid)
				<< " show:" << show
				<< " ver:" << ver
				<< " subver:" << subver
				<< " secver:" << secver
				<< " ext:" << ext);
			OnlineCenterAdapter::instance().presence(jid,onlineStat(show),ver,ext, Str2StrMap());
		} catch(Ice::Exception& e) {
			MCE_WARN("PresenceDefultTask::handle-->OnlineCenterAdapter::presence-->"<<e);
		}
		try{
			MucGateAdapter::instance().UserOnline(jid);
		}catch(Ice::Exception& e){
			MCE_WARN("PresenceDefultTask::handle-->MucGateAdapter::UserOnline-->" << jidToString(jid) << "err" << e);
		}
		try{
			MyUtil::IntSeq s;
			s.push_back(jid->userId);
			//IMWindowAdapter::instance().UsersAdReminder("/imadinfo/zhuomianhuodong.do", s);
		}catch(Ice::Exception& e){
			MCE_WARN("PresenceDefultTask::handle-->IMWindow --> userid:" << jid->userId << " error:" << e);
		}
	}else if (jid->index > 1) {
		try{
			MCE_INFO("~~PresenceDefultTask::presence --> invoke RestLogic " << jidToString(jid)
					<< " show:" << show
					<< " ver:" << ver
					<< " ext:" << ext
					<< " subver:" << subver
					<< " status:" << status);
			RestLogicAdapter::instance().setStatus(jid, onlineStat(show), status);
		}catch(Ice::Exception& e){
			MCE_WARN("PresenceDefultTask::handle-->RestLogicAdapter::setStatus-->"<< e << " " << jidToString(jid));
		}
	}


	if (jid->endpoint[0] == 'W') {
		try{
			OnlineEventMediatorAdapter::instance().sendOnlineEvent(jid->userId,1);
		}catch(Ice::Exception& e){
			MCE_WARN("PresenceDefultTask::handle-->OnlineEventMediatorAdapter::sendOnlineEvent-->" << e);
		}
	} else {
		try{
			OnlineEventMediatorAdapter::instance().sendOnlineEvent(jid->userId,2);
		}catch(Ice::Exception& e){
			MCE_WARN("PresenceDefultTask::handle-->OnlineEventMediatorAdapter::sendOnlineEvent" << e);
		}
	}

}
