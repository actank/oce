#include "IqTask.h"
#include "BuddyCoreAdapter.h"
//#include "BuddyCacheAdapter.h"
#include "BuddyByIdReplicaAdapter.h"
#include "IceLogger.h"
#include "TalkRightAdapter.h"
#include "MessageType.h"
#include "XmppTools.h"
#include "PhotoUrlTools.h"
#include "TalkCacheAdapter.h"
#include "BuddyGroupAdapter.h"
#include "PresenceAdapter.h"
#include "OnlineCenterAdapter.h"
#include "TalkProxyAdapter.h"
#include <boost/lexical_cast.hpp>
#include "ConnectionQuery.h"
#include "TalkCacheLoaderAdapter.h"
#include "TalkDeliverAdapter.h"
#include "QueryRunner.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "IqHandlerI.h"
//#include "WEventAdapter.h"
#include "WTransitAdapter.h"
#include "WTransitForIqAdapter.h"
#include "TalkRoomAdapter.h"
#include "UserPassportAdapter.h"
#include "OccupantsAdapter.h"
#include "XmppErrorCode.h"
#include "UserRoomAdapter.h"
#include "MucDeliverAdapter.h"
#include "Utf8Format.h"
#include "UserCacheAdapter.h"
#include "NotifyIndexAdapter.h"
#include "RestLogicAdapter.h"
#include "MucGateAdapter.h"
#include "IMWindowAdapter.h"
#include "TicketAdapter.h"
using namespace com::xiaonei::talk::muc;

#include "FeedMemcProxy/client/talk_cache_client.h"
using namespace talk::window;
using namespace xce::talk;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::service::buddy;
using namespace com::xiaonei::talk::adapter;
using namespace com::xiaonei::talk::util;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace talk::adapter;
using namespace talk::room::adapter;
using namespace talk::occupants::adapter;
using namespace talk::userroom::adapter;
using namespace talk::mucdeliver::adapter;
using namespace talk::online::adapter;
using namespace xce::buddy::adapter;
using namespace xce::usercache;
using namespace xce::notify;
using namespace talk::rest;
using namespace xce::adapter::userpassport;


void IqGetTask::handle() {
	StatFunc statF("IqGetTask::handle");
	ostringstream os;
	os << "IqGetTask::";
	TimeStat ts;
	xml_document reply;
	xml_node iqNode = reply.append_child();
	iqNode.set_name("iq");
	iqNode.append_attribute("type").set_value("result");

	if (!_doc->child("iq").attribute("from").empty()) {
		iqNode.append_attribute("to") = jidToString(_jid).c_str();//_doc->child("iq").attribute("from").value().c_str();
	}
	if (!_doc->child("iq").attribute("to").empty()) {
		iqNode.append_attribute("from") = _doc->child("iq").attribute("to").value().c_str();
	}
	if (!_doc->child("iq").attribute("id").empty()) {
		iqNode.append_attribute("id") = _doc->child("iq").attribute("id").value().c_str();
	} else {
		iqNode.append_attribute("id") = (int)time(NULL);
	}

	for (xml_node n = _doc->child("iq").first_child(); n; n = n.next_sibling()) {
		if (strcmp(n.name(), "query") == 0) {

			string xmlns = n.attribute("xmlns").value();

			if (xmlns == "http://jabber.org/protocol/disco#items") {
				_getItems(reply);
				os << "_getItems";
			} else if (xmlns == "http://jabber.org/protocol/disco#info") {
				_discoverService(reply);
				os << "_discoverService";
			} else if (xmlns == "http://jabber.org/protocol/muc#owner") {
				_getRoomConfig();
				os << "_getRoomConfig";
			} else if (xmlns == "http://jabber.org/protocol/muc#admin") {
				_getMemberList();
				os << "_getMemberList";
			} else if (xmlns == "jabber:iq:privacy") {
				getPrivacyHandle(reply);
				os << "getPrivacyHandle";
			} else if (xmlns == "jabber:iq:private") {
				getPrivateHandle(reply);
				os << "getPrivateHandle";
			} else if (xmlns == "jabber:iq:last") {
				try{
					TalkProxyAdapter::instance().express(_jid, reply.xml());
				}catch(Ice::Exception& e){
					MCE_WARN("IqGetTask::handle-->TalkProxyAdapter::express-->err" << e);
				}
			} else if (xmlns == "jabber:iq:roster") {
				getRosterHandle(reply);
				os << "getRosterHandle";
			} else if (xmlns == "http://talk.xiaonei.com/xfeed" || xmlns == "http://talk.renren.com/xfeed") {
				getXFeedHandle(reply);
				os << "getXFeedHandle";
			} else if (xmlns == "http://talk.xiaonei.com/transfer" || xmlns == "http://talk.renren.com/transfer") {
				getTransferHandle(*_doc);
				os << "getTransferHandle";
			} else if (xmlns == "http://talk.renren.com/xfeed/reply" || xmlns == "http://talk.xiaonei.com/xfeed/reply"){
				_getFeedReply();
				os << "_getFeedReply";
			} else if (xmlns == "http://talk.renren.com/xfeed/replycount" || xmlns == "http://talk.xiaonei.com/xfeed/replycount"){
				_getFeedReplyCount();
				os << "_getFeedReplyCount";
			} else if(xmlns == "http://talk.renren.com/getsessionkey" || xmlns == "http://talk.xiaonei.com/getsessionkey"){
			  _getSessionKey();
				os << "_getSessionKey";
			} else if (xmlns == "http://talk.renren.com/webticket"  || xmlns == "http://talk.xiaonei.com/webticket") {
        _getWebTicket(reply);
        os << "_getWebTicket";
      }

		} else if (strcmp(n.name(), "vCard") == 0) {
			getVcardHandle(reply);
			os << "getVcardHandle";
		} else if (strcmp(n.name(), "unique") == 0) {
			getUniqueRoomName(reply);
			os << "getUniqueRoomName";
		} else if(strcmp(n.name(), "ping") == 0){
			string xmlns = n.attribute("xmlns").value();
			if("urn:xmpp:ping" == xmlns){
				try{
					TalkProxyAdapter::instance().express(_jid, reply.xml());
				}catch(Ice::Exception& e){
					MCE_WARN("IqGetTask::handle-->TalkProxyAdapter::express-->err" << e);
				}
			}
		} else {
			_default();
			return;
		}
	}
	FunStatManager::instance().Stat(os.str(), ts.getTime(), (ts.getTime() >= 250.0));
	//MCE_INFO(os.str());
}
void IqGetTask::_getSessionKey(){
  int id = 0;
  try{
    string tmp = _doc->child("iq").attribute("id").value();
    id = boost::lexical_cast<int>(tmp);
  }catch(...){
    MCE_WARN("IqGetTask::_getSessionKey --> cast parameter err");
    return;
  }
  //MCE_INFO("IqGetTask::_getSessionKey --> " << jidToString(_jid) << " id = " << id);
  try{
    RestLogicAdapter::instance().getSessionKey(_jid, id);
  }catch(Ice::Exception& e){
    MCE_WARN("IqGetTask::_getSessionKey-->RestLogicAdapter::getSessionKey-->err" << e);
  }
}


void IqGetTask::_default(){
	xml_node iqNode = _doc->child("iq");
	iqNode.append_attribute("type") = "error";
	if(iqNode.attribute("from")){
		iqNode.remove_attribute("from");
	}
	iqNode.append_attribute("to") = jidToString(_jid).c_str();

	xml_node errNode = iqNode.append_child();
	errNode.set_name("error");
	errNode.append_attribute("code") = "503";
	errNode.append_attribute("type") = "cancel";

	xml_node unavailableNode = errNode.append_child();
	unavailableNode.set_name("service-unavailable");
	unavailableNode.append_attribute("xmlns") = "urn:ietf:params:xml:ns:xmpp-stanzas";
	try{
		TalkProxyAdapter::instance().express(_jid, _doc->xml());
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::_default-->TalkProxyAdapter::express-->" << e);
	}

}

void IqGetTask::getTransferHandle(xml_document& reply) {
	MCE_DEBUG("IqGetTask::getTransferHandle");
	if (!reply.child("iq").attribute("to")) {
		return;
	}

	JidPtr to = stringToJid(reply.child("iq").attribute("to").value());
	reply.child("iq").append_attribute("from") = jidToString(_jid).c_str();

	reply.child("iq").remove_attribute("to");

	MCE_DEBUG("IqGetTask::getTransferHandle --> to:"<< jidToString(to)
			<< ", msg:"<<reply.xml());
	try{
	TalkProxyAdapter::instance().express(to, reply.xml());
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::getTransferHandle-->TalkProxyAdapter::express-->" << e);
		}
}

void IqGetTask::_discoverService(xml_document& reply) {

	xml_node iqNode = reply.child("iq");
	//	iqNode.append_attribute("to") = jidToString(_jid).c_str();
	//	xml_node cn = iqNode.append_child();
	//	cn.set_name("query");
	//	cn.append_attribute("xmlns").set_value("http://jabber.org/protocol/disco#info");

	string domain = iqNode.attribute("from").value();

	ostringstream msg;
	if (domain == "talk.xiaonei.com" || domain == "talk.renren.com") {
		msg << "<iq from='"<< domain <<"' to='" << jidToString(_jid)
				<< "' id='"<< _doc->child("iq").attribute("id").value() <<"' type='result'>"
				<< "<query xmlns='http://jabber.org/protocol/disco#info'>"
				<< "</query>" << "</iq>";
	} else {
		msg << "<iq from='" << domain << "' to='" << jidToString(_jid)
				<< "' id='" << _doc->child("iq").attribute("id").value() << "' type='result'>"
				<< "<query xmlns='http://jabber.org/protocol/disco#info'>"
				<< "<identity category='conference' type='text' name='Chatrooms'/>" //display room list in another window
				<< "<feature var='http://jabber.org/protocol/muc'/>" //display join
				<< "</query>" << "</iq>";
	}
  try{
	TalkProxyAdapter::instance().express(_jid, msg.str());
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::_discoverService-->TalkProxyAdapter::express-->err" << e);
		}
}

void IqGetTask::_getItems(xml_document& reply) {

	xml_node iqNode = reply.child("iq");
	xml_node cn = iqNode.append_child();
	cn.set_name("query");
	cn.append_attribute("xmlns").set_value("http://jabber.org/protocol/disco#items");

	string domain = iqNode.attribute("from").value();
	if (domain == "talk.xiaonei.com" || domain == "talk.renren.com") {

		xml_node ccn = cn.append_child();
		ccn.set_name("item");
		ccn.append_attribute("jid").set_value("conference.talk.xiaonei.com");
		xml_node ccn2 = cn.append_child();
		ccn2.set_name("item");
		ccn2.append_attribute("jid").set_value("group.talk.xiaonei.com");

	} 
	else if (domain.find("group") == 0) {
		MucRoomIdSeq ridSeq;
		try{
	  ridSeq = MucGateAdapter::instance().GetUserRooms(_jid->userId, "group.talk.xiaonei.com");
		}catch(Ice::Exception& e){
			MCE_WARN("IqGetTask::_getItems-->MucGateAdapter::GetUserRooms-->err" << e);
			}
		for (size_t i = 0; i < ridSeq.size(); ++i) {
			xml_node ccn = cn.append_child();
			ccn.set_name("item");
			ccn.append_attribute("jid").set_value(mucRoomIdToString(ridSeq.at(i)).c_str());
			ccn.append_attribute("name").set_value(ridSeq.at(i)->roomname.c_str());
		}
	} else if (domain.find("conference") == 0) {
		MucRoomIdSeq ridSeq;
		try{
		ridSeq = MucGateAdapter::instance().GetUserRooms(_jid->userId, "conference.talk.xiaonei.com");
		}catch(Ice::Exception& e){
			MCE_WARN("IqGetTask::_getItems-->MucGateAdapter::GetUserRooms-->err" << e);
			}
		for (size_t i = 0; i < ridSeq.size(); ++i) {
			xml_node ccn = cn.append_child();
			ccn.set_name("item");
			ccn.append_attribute("jid").set_value(mucRoomIdToString(ridSeq.at(i)).c_str());
			ccn.append_attribute("name").set_value(ridSeq.at(i)->roomname.c_str());
		}
	}
	try{
	TalkProxyAdapter::instance().express(_jid, reply.xml());
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::_getItems-->TalkProxyAdapter::express-->err" << e);
		}
}

void IqGetTask::_getRoomConfig() {
	string strTo = _doc->child("iq").attribute("to").value();
	string strId = _doc->child("iq").attribute("id").value();
	//RoomIdPtr roomId = stringToRoomId(strTo);
	ostringstream os;
	os << "<iq from=" << strTo << "' to = '" << jidToString(_jid) << "' id ='" 
	<< strId << "' type = 'result'>"
	<< "</iq>";
	//RoomPtr room = TalkRoomAdapter::instance().getRoom(roomId);
	//MCE_DEBUG("IqGetTask::_getRoomConfig --> invoke TalkRoom geRoom end, "
	//		<< strTo);
	//if (!room) {
	//	return;
	//}
	/*
	ostringstream os;
	os << "<iq from='" << strTo << "' to='" << jidToString(_jid) << "' id='"
			<< strId << "' type='result'>"
			<< "<query xmlns='http://jabber.org/protocol/muc#owner'>"
			<< "<instructions>You need an x:data capable client to configure room</instructions>"
			<< "<x xmlns='jabber:x:data' type='form'>"
			<< "<title>Configuration for " << strTo << "</title>"

			<< "<field type='text-single' label='Room title' var='muc#roomconfig_roomname'>"
			<< "<value>" << room->title << "</value>" << "</field>"

			<< "<field type='text-single' label='Room description' var='muc#roomconfig_roomdesc'>"
			<< "<value>" << room->descr << "</value>" << "</field>"

			<< "<field type='boolean' label='Make room persistent' var='muc#roomconfig_persistentroom'>"
			<< "<value>"<<room->type->persistent<< "</value>" << "</field>"
			<< "<field type='boolean' label='Make room public searchable' var='muc#roomconfig_publicroom'>"
			<< "<value>"<<!(room->type->hidden)<< "</value>" << "</field>"
			<< "<field type='boolean' label='Make room password protected' var='muc#roomconfig_passwordprotectedroom'>"
			<< "<value>"<<room->type->secured<< "</value>" << "</field>"
			<< "<field type='text-private' label='Password' var='muc#roomconfig_roomsecret'>"
			<< "<value></value>" << "</field>"

			<< "<field type='list-single' label='Maximum Number of Occupants' var='muc#roomconfig_maxusers'>"
			<< "<value>"<<room->maxOccupantsCount<< "</value>"
			<< "<option label='5'>" << "<value>"<<5<< "</value>" << "</option>"
			<< "<option label='10'>" << "<value>"<<10<< "</value>"
			<< "</option>" << "<option label='20'>" << "<value>"<<20
			<< "</value>" << "</option>" << "<option label='30'>" << "<value>"
			<<30<< "</value>" << "</option>" << "<option label='50'>"
			<< "<value>"<<50<< "</value>" << "</option>"
			<< "<option label='100'>" << "<value>"<<100<< "</value>"
			<< "</option>" << "</field>"
			 << "<field type='list-single' label='Present real JIDs to' var='muc#roomconfig_whois'>"
			 << "<value>"moderators<< "</value>"
			 << "<option label='moderators only'>"
			 << "<value>"moderators<< "</value>"
			 << "</option>"
			 << "<option label='anyone'>"
			 << "<value>"anyone<< "</value>"
			 << "</option>"
			 << "</field>"
			<< "<field type='boolean' label='Make room members-only' var='muc#roomconfig_membersonly'>"
			<< "<value>"<<room->type->memberOnly<< "</value>" << "</field>"

			<< "<field type='boolean' label='Make room moderated' var='muc#roomconfig_moderatedroom'>"
			<< "<value>"<<room->type->moderated<< "</value>" << "</field>"

			<< "<field type='boolean' label='Allow users to send invites' var='muc#roomconfig_allowinvites'>"
			<< "<value>"<<room->allowUserToInvite<< "</value>" << "</field>"

	<< "</x>" << "</query>" << "</iq>";
	*/
	try{
	TalkProxyAdapter::instance().express(_jid, os.str());
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::_getRoomConfig-->TalkProxyAdapter::express-->err" << e);
		}

}

void IqGetTask::_getMemberList() {
	/*	<iq to="asdf@conference.jabber.ru" type="get" id="969">
	 <query xmlns="http://jabber.org/protocol/muc#admin">
	 <item affiliation="member" />
	 </query>
	 </iq>
	 */
	MCE_DEBUG("---IqGetTask::_getMemberList---");
	string strTo = _doc->child("iq").attribute("to").value();
	string strId = _doc->child("iq").attribute("id").value();
	string strPermision = _doc->child("iq").child("query").child("item").attribute("affiliation").value();
	if (strTo == "" || strPermision == "") {
		return;
	}
	Permisions permision =  _mucGetPermision(strPermision);
	MucRoomIdPtr roomId = stringToMucRoomId(strTo);
	MucActiveUserSeq seq;
	try{
	seq = MucGateAdapter::instance().GetUserByPermision(roomId, permision);
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::_getMemberList-->MucGateAdapter::GetUserByPermision-->err" << e);
		}
	MCE_DEBUG("IqGetTask::_getMemberList --> invoke MucGate.GetUserByPermision, size=" << seq.size());

	ostringstream os;
	os << "<iq from='" << strTo << "' to='" << jidToString(_jid) << "' id='"
			<< strId << "' type='result'>"
			<< "<query xmlns='http://jabber.org/protocol/muc#admin'>";
	for (size_t i=0; i<seq.size(); ++i) {
		os << "<item affiliation='" << strPermision << "' jid='" << seq.at(i)->jid->userId
				<< "@talk.xiaonei.com'/>";
	}
	os << "</query>" << "</iq>";
	try{
	TalkProxyAdapter::instance().express(_jid, os.str());
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::_getMemberList-->TalkProxyAdapter::-->err" << e);
		}
}

void IqGetTask::_getWebTicket(xml_document& reply) {
	StatFunc statF("IqGetTask::getWebTicket");

	xml_node queryNode = reply.child("iq").append_child();
	queryNode.set_name("query");
	queryNode.append_attribute("xmlns") = "http://talk.renren.com/webticket";
  xml_node tNode = queryNode.append_child();
  tNode.set_name("t");
  tNode.append_child(node_pcdata).set_value(passport::TicketAdapter::instance().createTicket(_jid->userId,"").c_str());

	try{
  	TalkProxyAdapter::instance().express(_jid, reply.xml());
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::_getWebTicket --->TalkProxyAdapter::express-->err" << e);
	}
}

void IqGetTask::getPrivacyHandle(xml_document& reply) {
	StatFunc statF("IqGetTask::getPrivacyHandle");

	xml_node qNode = _doc->child("iq").child("query");

	vector<xml_node> lNodes;
	for (xml_node n = qNode.child("list"); n; n = n.next_sibling("list")) {
		lNodes.push_back(n);
	}

	if (lNodes.empty()) {

		xml_node queryNode = reply.child("iq").append_child();
		queryNode.set_name("query");
		queryNode.append_attribute("xmlns") = "jabber:iq:privacy";

		xml_node activeNode = queryNode.append_child();
		activeNode.set_name("active");
		activeNode.append_attribute("name") = "blocked";

		xml_node defaultNode = queryNode.append_child();
		defaultNode.set_name("default");
		defaultNode.append_attribute("name") = "blocked";

		xml_node listNode = queryNode.append_child();
		listNode.set_name("list");
		listNode.append_attribute("name") = "blocked";

	} else if (lNodes.size()==1) {

		if (strcmp(lNodes.at(0).attribute("name").value().c_str(), "blocked")) {
			xml_node iqNode = reply.child("iq");
			iqNode.append_attribute("type") = "error";

			xml_node errorNode = iqNode.append_child();
			errorNode.set_name("error");
			errorNode.append_attribute("type") = "cancel";

			xml_node notFindNode = errorNode.append_child();
			notFindNode.set_name("item-not-found");
			notFindNode.append_attribute("xmlns")
					= "urn:ietf:params:xml:ns:xmpp-stanzas";
		} else {
			MyUtil::IntSeq blockIds;
			try {
				blockIds = BuddyCoreAdapter::instance().getBlockSeq(_jid->userId, 0, -1,1000);
			} catch (Ice::Exception& e) {
				MCE_WARN("IqGetTask::getPrivacyHandle-->BuddyCoreAdapter::getBlockSeq-->userId:"<<_jid->userId<<" err:"<<e);
			} catch (...) {
				MCE_WARN("IqGetTask::getPrivacyHandle -> BuddyCoreAdapter::getBlockSeq err userId:"<<_jid->userId);
			}

			xml_node queryNode = reply.child("iq").append_child();
			queryNode.set_name("query");
			queryNode.append_attribute("xmlns") = "jabber:iq:privacy";

			xml_node listNode = queryNode.append_child();
			listNode.set_name("list");
			listNode.append_attribute("name") = "blocked";
			for (size_t i = 0; i < blockIds.size(); ++i) {
				xml_node itemNode = listNode.append_child();
				itemNode.set_name("item");
				itemNode.append_attribute("type") = "jid";
				itemNode.append_attribute("action") = "deny";
				itemNode.append_attribute("order") = blockIds.at(i);
				ostringstream jid;
				jid << blockIds.at(i)<<"@talk.xiaonei.com";
				itemNode.append_attribute("value") = jid.str().c_str();
			}
			MyUtil::StrSeq sysBlockSeq;
			try{
			sysBlockSeq = TalkRightAdapter::instance().getSystemMessageBlock(_jid->userId);
			}catch(Ice::Exception& e){
				MCE_WARN("IqGetTask::getPrivacyHandle-->TalkRightAdapter::getSystemMessageBlock-->err" << e);
				}
			for (size_t i = 0; i < sysBlockSeq.size(); ++i) {
				xml_node itemNode = listNode.append_child();
				itemNode.set_name("item");
				itemNode.append_attribute("type") = "jid";
				itemNode.append_attribute("action") = "deny";
				itemNode.append_attribute("order") = (int)time(NULL);
				ostringstream jid;
				jid << sysBlockSeq.at(i) <<"@news.talk.xiaonei.com";
				itemNode.append_attribute("value") = jid.str().c_str();
			}
			bool isHidden;
			try{
				isHidden = TalkRightAdapter::instance().isHiddenUser(_jid->userId);
			}catch(Ice::Exception& e){
				MCE_WARN("IqGetTask::getPrivacyHandle-->TalkRightAdapter::isHiddenUser-->err" << e);
				}
			if (isHidden) {
				xml_node itemNode = listNode.append_child();
				itemNode.set_name("item");
				itemNode.append_attribute("action") = "deny";
				itemNode.append_attribute("order") = (int)time(NULL);

				xml_node presenceOutNode = itemNode.append_child();
				presenceOutNode.set_name("presence-out");
			}
		}

	} else if (lNodes.size()> 1) {

		xml_node iqNode = reply.child("iq");
		iqNode.append_attribute("type") = "error";

		xml_node errorNode = iqNode.append_child();
		errorNode.set_name("error");
		errorNode.append_attribute("type") = "modify";

		xml_node badRequest = errorNode.append_child();
		badRequest.set_name("bad-request");
		badRequest.append_attribute("xmlns")
				= "urn:ietf:params:xml:ns:xmpp-stanzas";

	}

	MessagePtr msg = new Message();
	msg -> from = _jid;
	msg -> to = _jid;
	msg -> type = IQ_RESULT;
	msg -> msg = reply.xml();
	//TalkDeliverAdapter::instance().deliver(msg);
	try{
	TalkProxyAdapter::instance().deliver(msg);
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::getPrivacyHandle-->TalkProxyAdapter::deliver-->" << e);
		}
}

void IqGetTask::getPrivateHandle(xml_document& reply) {
	StatFunc statF("IqGetTask::getPrivateHandle");

	xml_node iqNode = _doc->child("iq");
	iqNode.append_attribute("type") = "result";
	iqNode.append_attribute("from") = jidToString(_jid, false).c_str();
	//iqNode.append_attribute("to") = jidToString(_jid).c_str();

	MessagePtr msg = new Message();
	msg -> from = _jid;
	msg -> to = _jid;
	msg -> type = IQ_RESULT;
	msg -> msg = _doc->xml();
	try{
	TalkProxyAdapter::instance().deliver(msg);
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::getPrivateHandle-->TalkProxyAdapter::deliver-->" << e);
		}
}

void IqGetTask::getRosterHandle(xml_document& reply) {
	TimeStat ts;
	StatFunc statF("IqGetTask::getRosterHandle");
	xml_node queryNode = reply.child("iq").append_child();
	queryNode.set_name("query");
	queryNode.append_attribute("xmlns") = "jabber:iq:roster";

	MyUtil::IntSeq buddyIds;
	try {
		buddyIds = BuddyByIdCacheAdapter::instance().getFriendListAsc(_jid->userId, 2000);
	} catch(Ice::Exception& e) {
		MCE_WARN("IqGetTask::getRosterHandle-->BuddyByIdCacheAdapter::getFriendListAsc-->"<<_jid->userId<<":"<<e);
	} catch(...) {
		MCE_WARN("call IqGetTask::getRosterHandle -> getFrinedSeq err");
	}
	MCE_DEBUG("call IqGetTask::getRosterHandle -> finish getFriendSeq  userId="
			<<_jid->userId<< "  buddy.size="<<buddyIds.size());

	//MCE_INFO("IqGetTask::getRosterHandle --> usetime BuddyByIdCache --> " << ts.getTime());
	ts.reset();
	if (buddyIds.empty()) {
		//		msg -> msg = reply.xml();
		//		TalkMessenger::instance().deliver(msg);
		//TalkProxyAdapter::instance().express(_jid, reply.xml());
		return;
	}
  TalkUserMap usermaps;
  try{
		usermaps = TalkCacheClient::instance().GetUserBySeqWithLoad(_jid->userId, buddyIds);
		//for(TalkUserMap::iterator uit = usermaps.begin(); uit != usermaps.end(); ++uit){
		//	if()
		//}
		/*
		if(usermaps.size() != buddyIds.size()){
			MCE_INFO("IqGetTask::getRosterHandle --> may be warn get from TalkCacheClient size is not equal actor = " << _jid->userId << " ids.size = " << buddyIds.size() << " map.size = " << usermaps.size());
		}else{
			MCE_INFO("IqGetTask::getRosterHandle --> success get from TalkCacheClient size is equal actor = " << _jid->userId << " ids.size = " << buddyIds.size() << " map.size = " << usermaps.size());
    }
		*/
	}catch(Ice::Exception& e){
    MCE_WARN("IqGetTask::getRosterHandle-->TalkCacheClient::GetUserBySeqWithLoad-->" << e);
  }

	//MCE_INFO("IqGetTask::getRosterHandle --> usetime TalkCacheClient --> " << ts.getTime());
	ts.reset();
	BuddiedGroupsMap gMap;
	try {
		gMap = BuddyGroupAdapter::instance().getBuddiedGroupsMap(_jid->userId,500);
	} catch(Ice::Exception & e) {
		MCE_WARN("IqGetTask::getRosterHandle-->BuddyGroupAdapter::getBuddiedGroupsMap-->"<<e);
	}

	//MCE_INFO("IqGetTask::getRosterHandle --> usetime BuddyGroup --> " << ts.getTime());
	ts.reset();
	MyUtil::IntSeq ids;

	for (size_t i = 0; i < buddyIds.size(); ++i) {
    TalkUserMap::iterator it = usermaps.find(buddyIds.at(i));
		if (it != usermaps.end()) {
			//UserPtr u = UserPtr::dynamicCast(it->second);
      TalkUserPtr u = it->second;
			xml_node itemNode = queryNode.append_child();
			itemNode.set_name("item");
			ostringstream jid;
			jid << u->id << "@" << "talk.xiaonei.com";
			itemNode.append_attribute("jid") = jid.str().c_str();
			Utf8Formatting(const_cast<char*>(u->name.c_str()));
			itemNode.append_attribute("name") = u->name.c_str();
			itemNode.append_attribute("subscription") = "both";

			BuddiedGroupsMap::iterator bit = gMap.find(buddyIds.at(i));
			if (bit != gMap.end()) {
				for (size_t j = 0; j< bit->second.size(); ++j) {
					xml_node groupNode = itemNode.append_child();
					groupNode.set_name("group");
					groupNode.append_child(node_pcdata).set_value(bit->second.at(j).c_str());
				}
			}
		} else {
			ids.push_back(buddyIds.at(i));
		}
	}
	MCE_DEBUG("IqGetTask::getRosterHandle -> finish building item with buddyIds,  userId="
			<<_jid->userId<< "  buddy.size="<<buddyIds.size()
			<< ", missing.size=" << ids.size());
	//MCE_INFO("IqGetTask::getRosterHandle --> " << _jid->userId << " buddysize=" << buddyIds.size() << "miss=" << ids.size());
	//select from db
	if (!ids.empty()) {
		try {
			throw "";
//			ostringstream sql;
//			sql << "select id,  name from user_basic where id in (";
//			for (size_t i = 0; i < ids.size(); ++i) {
//				if (i) {
//					sql <<",";
//				}
//				sql<< ids.at(i);
//			}
//			sql << ")";
//			MCE_DEBUG("IqGetTask::getRosterHandle -> finish query,  userId="
//					<<_jid->userId << ", query=" << sql.str());
//#ifndef NEWARCH
//			mysqlpp::Result res = QueryRunner("im_common", CDbRServer).store(Statement(sql.str()));
//#else
//			mysqlpp::StoreQueryResult res = QueryRunner("im_common", CDbRServer).store(Statement(sql.str()));
//#endif
//			MCE_DEBUG("IqGetTask::getRosterHandle -> finish query,  userId="
//					<<_jid->userId << ", res.size=" << res.size());
//#ifndef NEWARCH
//			for (size_t i= 0; i<res.size(); ++i) {
//#else
//
//			for (size_t i= 0; i<res.num_rows(); ++i) {
//#endif
//				mysqlpp::Row row = res.at(i);
//				xml_node itemNode = queryNode.append_child();
//				itemNode.set_name("item");
//				ostringstream jid;
//				jid << (int)row["id"] << "@" << "talk.xiaonei.com";
//				itemNode.append_attribute("jid") = jid.str().c_str();
//#ifndef NEWARCH
//				itemNode.append_attribute("name") = row["name"].get_string().c_str();//drs->getCol(i,"NAME").get_string().c_str();
//#else
//				itemNode.append_attribute("name") = row["name"].c_str();//c_str();//drs->getCol(i,"NAME").get_string().c_str();
//#endif
//				itemNode.append_attribute("subscription") = "both";
//
//				BuddiedGroupsMap::iterator bit = gMap.find((int)row["id"]);
//				if (bit != gMap.end()) {
//					for (size_t j = 0; j< bit->second.size(); ++j) {
//						xml_node groupNode = itemNode.append_child();
//						groupNode.set_name("group");
//						groupNode.append_child(node_pcdata).set_value(bit->second.at(j).c_str());
//					}
//				}
//			}
		} catch (...) {
			for(size_t i = 0; i < ids.size(); ++i) {
				xml_node itemNode = queryNode.append_child();
				itemNode.set_name("item");
				ostringstream jid;
				jid << ids.at(i) << "@" << "talk.xiaonei.com";
				itemNode.append_attribute("jid") = jid.str().c_str();
				itemNode.append_attribute("name") = ids.at(i);
				itemNode.append_attribute("subscription") = "both";

				BuddiedGroupsMap::iterator bit = gMap.find(ids.at(i));
				if (bit != gMap.end()) {
					for (size_t j = 0; j< bit->second.size(); ++j) {
						xml_node groupNode = itemNode.append_child();
						groupNode.set_name("group");
						groupNode.append_child(node_pcdata).set_value(bit->second.at(j).c_str());
					}
				}
			}
		}
	}
	MCE_DEBUG("IqGetTask::getRosterHandle -> finish all item  userId="
			<<_jid->userId<< "  buddy.size="<<buddyIds.size()
			<< ", missing.size=" << ids.size());
	//msg -> msg = reply.xml();
	try{
		TalkProxyAdapter::instance().express(_jid, reply.xml());
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::getRosterHandle-->TalkProxyAdapter::express-->err" << e);
	}
	/*
	try{
		IMWindowAdapter::instance().UserLoginReminder(_jid);
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::getRosterHandle-->IMWindowAdapter::UserLoginRemind--> jid:" << jidToString(_jid) << " error:" << e);
	}
	*/
	IqHandlerI::instance().GetBuddyApply(_jid);

  string talk_title = "";
	UserPassportInfoPtr passport_info;
	try{
		passport_info = UserPassportAdapter::instance().getUserPassport(_jid->userId);
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::getRosterHandle --> call UserPassportAdapter err " << e << "    uid:"<< _jid->userId);
	}
	int safe_status = 0;
	if(passport_info){
		safe_status = passport_info->safestatus();
	}
  if(3==safe_status || 4==safe_status || 5==safe_status){
    MCE_WARN("IqGetTask::getRosterHandle --> safe_status err, " << safe_status << "    uid:"<< _jid->userId);
    talk_title = "/imadinfo/alert.do";
  }

  if (talk_title == "") {
    AvatarPtr avatar;
    try {
      avatar = OnlineCenterAdapter::instance().getAvatar(_jid);
  	}catch(Ice::Exception& e){
	  	MCE_WARN("IqGetTask::getRosterHandle-->OnlineCenter --> userid:" << _jid->userId << " error:" << e);
  	}
    if (avatar != NULL) {
      if (avatar->ver=="4.047" || avatar->ver=="4.056")
        talk_title = "/imadinfo/support.do";
      else if (avatar->ver == "4.049")
        talk_title = "/imadinfo/zhuomianhuodong.do";
    }
  }
  if (talk_title != "") {
  	try{
  		IMWindowAdapter::instance().UserAdReminderOnly(talk_title.c_str(), _jid);
  	}catch(Ice::Exception& e){
  		MCE_WARN("IqGetTask::getRosterHandle-->IMWindow --> userid:" << _jid->userId << " error:" << e);
  	}
  }
}

void IqGetTask::getVcardHandle(xml_document& reply) {
	TimeStat talkcacheclient_ts, usercache_ts;
	float vtalkcacheclient = 0.0, vusercache = 0.0;
	MessagePtr msg = new Message();
	msg -> from = _jid;
	msg -> to = _jid;
	msg -> type = IQ_RESULT;

	xml_attribute toAttr = _doc->child("iq").attribute("to");
	if (toAttr.empty()) {
		MCE_WARN("IqGetTask::getVcardHandle -> wrong, no to attribute: " << _doc->xml());
		msg -> msg = reply.xml();
		try{
			TalkProxyAdapter::instance().deliver(msg);
		}catch(Ice::Exception& e){
			MCE_WARN("IqGetTask::getVcardHandle-->TalkProxyAdapter::deliver-->" << e);
		}
		return;
	}

	JidPtr tojid = stringToJid(toAttr.value());
  
  TalkUserPtr ui;	
	try {
		talkcacheclient_ts.reset();
		ui = TalkCacheClient::instance().GetUserByIdWithLoad(tojid->userId);
		vtalkcacheclient = talkcacheclient_ts.getTime();
	} catch(Ice::Exception& e) {
		MCE_WARN("IqGetTask::getVcardHandle-->TalkCacheClient::GetUserByIdWithLoad-->"<<tojid->userId<< " err:"<<e);
	}
	if (!ui) {
		MCE_WARN("IqGetTask::getVcardHandle --> TalkCacheClient  UserNotExist:" << tojid->userId);
		msg -> msg = reply.xml();
		try{
			TalkProxyAdapter::instance().deliver(msg);
		}catch(Ice::Exception& e){
			MCE_WARN("IqGetTask::getVcardHandle-->TalkProxyAdapter::deliver-->err" << e);
		}
		return;
	}
  else{
		//MCE_INFO("IqGetTask::getVcardHandle _jid = " << jidToString(_jid) << " tojid = " << tojid << " id:" << ui->id << " name:" << ui->name << " statusOriginal:" << ui->statusOriginal << " statusShifted:" << ui->statusShifted << " headurl:" << ui->headurl << " tinyurl:" << ui->tinyurl);
		if(tojid->userId != ui->id){
			MCE_WARN("IqGetTask::getVcardHandle --> memcerror userid = " << tojid->userId << " error_id = " << ui->id << " after call loader okid = " << ui->id);
			try{
				ui = TalkCacheLoaderAdapter::instance().LoadById(tojid->userId);
			}catch(Ice::Exception& e){
				MCE_WARN("IqGetTask::getVcardHandle-->TalkCacheLoaderAdapter::LoadById-->" << tojid->userId << ":" << e);
			}
		}
  }

	int userId = tojid->userId;
	IntSeq ids;
	ids.push_back(userId);
	map<int, UserCachePtr> userVips;
	try{
		usercache_ts.reset();
		userVips = UserCacheAdapter::instance().GetUserCache(ids);
		vusercache = usercache_ts.getTime();	
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::getVcardHandle-->UserCacheAdapter::GetUserCache-->" << userId << ":" << e);
	}

	int vip = 0;
	UserCachePtr vipInfo;
	map<int, UserCachePtr>::iterator it = userVips.find(userId);
	if(it != userVips.end()){
		vipInfo = it->second;
		vip = vipInfo->level();
		if(vip == 0){
			vip = 1;
		}
		if(vipInfo->IsVipMember()){
		}else if(vipInfo->IsEverVipMember()){
			vip = -vip;
		}else{
			vip = 0;
		}
	}
	string strVip;
	try{
		strVip = boost::lexical_cast<string>(vip);
	}catch(...){
		MCE_WARN("PresenceHandlerI::presence --> lexical cast err");
	}

	xml_node vCardNode = reply.child("iq").append_child();
	vCardNode.set_name("vCard");
	vCardNode.append_attribute("xmlns") = "vcard-temp";
	vCardNode.append_attribute("xmlns:xn") = "xn:talk:vcard";

	xml_node photoNode = vCardNode.append_child();
	photoNode.set_name("PHOTO");
	xml_node extvalNode = photoNode.append_child();
	extvalNode.set_name("EXTVAL");
	//extvalNode.append_child(node_pcdata).set_value(buildHeadUrl(ui->headurl).c_str());
	string headurl = PhotoUrlHelper::instance().GetFullUrl(ui->headurl);
	if(headurl.empty()){
		MCE_WARN("IqGetTask::getVcardHandle--> get headurl from PhotoUrlHelper is empty ui->headurl=" << ui->headurl);
	}
	extvalNode.append_child(node_pcdata).set_value(headurl.c_str());
	//extvalNode.append_child(node_pcdata).set_value("");
	//-----------------
	//string newurl = PhotoUrlHelper::instance().GetFullUrl(ui->headurl);
	//MCE_INFO("VCARD ----------------> ui->headurl=" << ui->headurl << "  newurl" << newurl);
	//-----------------
	xml_node xnstatNode = vCardNode.append_child();
	xnstatNode.set_name("xn:STATUS");
	xnstatNode.append_child(node_pcdata).set_value(ui->statusOriginal.c_str());

	xml_node fnNode = vCardNode.append_child();
	fnNode.set_name("FN");
	Utf8Formatting(const_cast<char*>(ui->name.c_str()));
	fnNode.append_child(node_pcdata).set_value(ui->name.c_str());
	
	//string str = reply.xml();
	//size_t pos = str.rfind("</iq>");
	//string str1 = str.substr(0, pos);
	//string str2 = str.substr(pos);
	//str = str1 + ui->statusShifted + str2;
	
	xml_node embHtmlNode = reply.child("iq").append_child();
	embHtmlNode.set_name("richstatus");
	embHtmlNode.append_child(node_pcdata).set_value(ui->statusShifted.c_str());
	
	if(!strVip.empty()){
		xml_node vipNode = reply.child("iq").append_child();
		vipNode.set_name("vip");
		vipNode.append_child(node_pcdata).set_value(strVip.c_str());
	}

	msg -> msg = reply.xml();
	try{
		TalkProxyAdapter::instance().deliver(msg);
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::getVcardHandle-->TalkProxyAdapter::deliver-->" << e);
	}
	MONITOR("SignForTimeuse::IqGetTask::getVcardHandle-->TalkCacheClient::GetUserByIdWithLoad-->" << vtalkcacheclient << " UserCache::GetUserCache-->" << vusercache);
}

void IqGetTask::_getFeedReply() {
	int owner = 0;
	int type = 0;
	Ice::Long source = 0;
	try{
		string tmp = _doc->child("iq").child("query").child("feedreply").attribute("stype").value();
		type = boost::lexical_cast<int>(tmp);
		tmp = _doc->child("iq").child("query").child("feedreply").attribute("actor").value();
		owner= boost::lexical_cast<int>(tmp);
		tmp = _doc->child("iq").child("query").child("feedreply").attribute("resource").value();
		source = boost::lexical_cast<Ice::Long>(tmp);
	}catch(...){
		MCE_WARN("IqGetTask::_getFeedReply --> cast parameter err");
		return;
	}
	try{
		MCE_INFO("IqGetTask::_getFeedReply --> userid:" << _jid->userId << " owner:" << owner << " type:" << type << " source:" << source);
		WTransitForIqAdapter::instance().loadOfflineFeedReply(_jid, type, owner, source, _doc->child("iq").attribute("id").value());
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::_getFeedReply-->WTransitForIqAdapter::loadOfflineFeedReply-->id:" << _jid->userId << " e:" << e);
	}

	//---------------------------------
	try{
		WTransitAdapter::instance().loadOfflineFeedReply(_jid, type, owner, source, _doc->child("iq").attribute("id").value());
	}catch(Ice::Exception& e){
	}
	//---------------------------------
	
	/*ostringstream os;
	os << "<iq type='result' id='" << _doc->child("iq").attribute("id") << "'>"
	<< "<query xmlns='" << _doc->child("iq").child("query").attribute("xmlns").value() << "'>";
	os << "</query></iq>";

	MessagePtr msg = new Message;
	msg->to = _jid;
	msg->from = _jid;
	msg->type = IQ_RESULT;
	msg->msg = os.str();
	try{
		MCE_INFO("IqGetTask::_getFeedReply --> deliver " << jidToString(_jid) << " msg:" << os.str());
		TalkProxyAdapter::instance().express(_jid, os.str());
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::_getFeedReply --> call deliver err, " << e);
	}*/
}

void IqGetTask::_getFeedReplyCount() {
	vector<Ice::Long> feedids; 
	string id = _doc->child("iq").attribute("id").value();
  string xmlns = _doc->child("iq").child("query").attribute("xmlns").value();
	xml_node node = _doc->child("iq").child("query").child("feedreply");
	string strIds = node.child("feedid").first_child().value();
	vector<string> splitVec;
	boost::split(splitVec, strIds, boost::is_any_of(","), boost::token_compress_on);
	for(int i=0; i<(int)splitVec.size(); i++){
		Ice::Long feedid = 0;
		try{
			feedid = boost::lexical_cast<Ice::Long>(splitVec.at(i));
		}catch(...){
			continue;
		}
		feedids.push_back(feedid);
	}
  try{
		MCE_INFO("IqGetTask::_getFeedReplyCount-->id:" << _jid->userId);
		WTransitForIqAdapter::instance().deliverFeedReplyCount(_jid, feedids, id, xmlns);
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::_getFeedReplyCount-->WTransitForIqAdapter::deliverFeedReplyCount-->id:" << _jid->userId << " e:" << e);
	}


	//---------------------------------
	try{
		WTransitAdapter::instance().deliverFeedReplyCount(_jid, feedids, id, xmlns);
	}catch(Ice::Exception& e){
	}
	//---------------------------------


  /*
	map<Ice::Long, int> res;
	try{
		res = WTransitForIqAdapter::instance().getFeedReplyCount(_jid->userId, feedids);
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::getFeedReplyCount --> call WTransit err, " << e);
	}
	MCE_INFO("IqGetTask::getFeedReplyCount --> user:" << _jid->userId << " feedid size:" << feedids.size() << " res:" << res.size());
	ostringstream os;
	os << "<iq type='result' id='" << id << "'>"
	<< "<query xmlns='" << _doc->child("iq").child("query").attribute("xmlns").value() << "'>";
	map<Ice::Long, int>::iterator it = res.begin();
	for(; it!=res.end(); ++it){
		os << "<feedreply id='" << it->first << "' count='" << it->second << "'/>";
	}
	os << "</query></iq>";
	
	MessagePtr msg = new Message;
	msg->to = _jid;
	msg->from = _jid;
	msg->type = IQ_RESULT;
	msg->msg = os.str();
	try{
		MCE_INFO("IqGetTask::getFeedReplyCount --> deliver " << jidToString(_jid) << " msg:" << os.str());
		TalkProxyAdapter::instance().express(_jid, os.str());
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::getFeedReplyCount --> call deliver err, " << e);
	}
  */
}

void IqGetTask::getXFeedHandle(xml_document& reply) {

	MCE_DEBUG("IqGetTask::getXFeedHandle");
	xml_attribute iqIdAttr = _doc->child("iq").attribute("id");
	int iqId = time(NULL);
	if (iqIdAttr) {
		try {
			iqId = boost::lexical_cast<int>(iqIdAttr.value());
		} catch(...) {};
	}
	//	xml_attribute beginIdAttr = _doc->child("iq").child("query").child("xfeed").attribute("begin");
	//	if (beginIdAttr) {
	//		try {
	//			long beginId = boost::lexical_cast<long>(beginIdAttr.value());
	//			MCE_DEBUG("IqGetTask::getXFeedHandle --> call WEventAdapter  jid:"<<jidToString(_jid)<< " begin:"<<beginId<<"   iqId:"<<iqId);
	//			WEventAdapter::instance().loadOfflineXFeed(_jid,beginId,iqId);
	//		} catch(Ice::Exception& e) {
	//			MCE_WARN("IqGetTask::getXFeedHandle --> call WEventAdapter err :"<<e);
	//		} catch(...){
	//			MCE_WARN("IqGetTask::getXFeedHandle --> beginId is not a number");
	//		}
	//	}

	Ice::Long feedIndex = 0;
	Ice::Long replyIndex = 0;

	xml_attribute feedIndexAttr = _doc->child("iq").child("query").child("xfeed").attribute("begin");
	if (feedIndexAttr) {
		try {
			feedIndex = boost::lexical_cast<Ice::Long>(feedIndexAttr.value());
			MCE_DEBUG("IqGetTask::getXFeedHandle --> call WEventAdapter  jid:"<<jidToString(_jid)<< " begin:"<<feedIndex<<"   iqId:"<<iqId);

		} catch(Ice::Exception& e) {
			MCE_WARN("IqGetTask::getXFeedHandle --> call WEventAdapter err :"<<e);
		} catch(...) {
			MCE_WARN("IqGetTask::getXFeedHandle --> beginId is not a number");
		}
	}

	xml_attribute replyIndexAttr = _doc->child("iq").child("query").child("reply").attribute("begin");
	if (replyIndexAttr) {
		try {
			replyIndex = boost::lexical_cast<Ice::Long>(replyIndexAttr.value());
			MCE_DEBUG("IqGetTask::getXFeedHandle --> call WEventAdapter  jid:"<<jidToString(_jid)<< " begin:"<<replyIndexAttr<<"   iqId:"<<iqId);

		} catch(Ice::Exception& e) {
			MCE_WARN("IqGetTask::getXFeedHandle --> call WEventAdapter err :"<<e);
		} catch(...) {
			MCE_WARN("IqGetTask::getXFeedHandle --> beginId is not a number");
		}
	}
	//WEventAdapter::instance().loadOfflineXFeed(_jid, feedIndex, replyIndex, iqId);
	//if(232626782 != _jid->userId){
		try{
			MCE_INFO("IqGetTask::getXFeedHandle-->id:" << _jid->userId);
			WTransitForIqAdapter::instance().loadOfflineFeed(_jid, feedIndex, replyIndex, iqId);
		}catch(Ice::Exception& e){
			MCE_WARN("IqGetTask::getXFeedHandle-->WTransitForIqAdapter::loadOfflineFeed-->id:" << _jid->userId << " e:" << e);
		}
	//}


	//---------------------------------
	try{
		WTransitAdapter::instance().CallFeedCacheAndContent(_jid, feedIndex, replyIndex, iqId);
	}catch(Ice::Exception& e){
		MCE_WARN("IqGetTask::getXFeedHandle-->WTransitAdapter::CallFeedCacheAndContent--> error :" << e );
	}
	//---------------------------------

}

void IqGetTask::getUniqueRoomName(xml_document& reply) {

}
int IqGetTask::_mucGetRole(string role) {
	//enum Affiliation {Owner,Admin,Member,Outcast,None};
	//enum RoomRole {Moderator,Participant,Visitor,NoneRole};
	if (role == "moderator") {
		return Moderator;
	} else if (role == "participant") {
		return Participant;
	} else if (role == "visitor") {
		return Visitor;
	} else {
		return NoneRole;
	}
}

Permisions IqGetTask::_mucGetPermision(string permision){
	if (permision == "owner") {
		return POwner;
	} else if (permision == "admin") {
		return PAdmin;
	} else if (permision == "member") {
		return PMember;
	} else if (permision == "outcast") {
		return POutcast;
	} else {
		return PNone;
	}
}
int IqGetTask::_mucGetAffiliation(string affiliation) {
	if (affiliation == "owner") {
		return Owner;
	} else if (affiliation == "admin") {
		return Admin;
	} else if (affiliation == "member") {
		return Member;
	} else if (affiliation == "outcast") {
		return Outcast;
	} else {
		return None;
	}
}
//--------------------------------------

void IqSetTask::handle() {
	StatFunc statF("IqSetTask::handle");
	ostringstream os;
	os << "IqSetTask::";
	TimeStat ts;
	xml_document reply;
	xml_node iqNode = reply.append_child();
	iqNode.set_name("iq");
	iqNode.append_attribute("type").set_value("result");
	/*	if (!_doc->child("iq").attribute("from").empty()) {
	 iqNode.append_attribute("to") = _doc->child("iq").attribute("from").value().c_str();
	 }
	 if (!_doc->child("iq").attribute("to").empty()) {
	 iqNode.append_attribute("from") = _doc->child("iq").attribute("to").value().c_str();
	 }*/
	if (!_doc->child("iq").attribute("id").empty()) {
		iqNode.append_attribute("id") = _doc->child("iq").attribute("id").value().c_str();
	} else {
		iqNode.append_attribute("id") = (int)time(NULL);
	}

	string tag = _doc->child("iq").first_child().name();
	string xmlns = _doc->child("iq").first_child().attribute("xmlns").value();

	if (tag == "session") {
		setSessionHandle(reply);
	  try{
	    RestLogicAdapter::instance().getSessionKey(_jid);
	  }catch(Ice::Exception& e){
	    MCE_WARN("IqSetTask::handle-->RestLogicAdapter::getSessionKey-->" << e);
	  }
		os << "session";
	} else if (tag == "query") {
		if (xmlns == "jabber:iq:privacy") {
			setPrivacyHandle(reply);
			os << "privacy";
		} else if (xmlns == "jabber:iq:private") {
			setPrivateHandle(reply);
			os << "private";
		} else if (xmlns == "jabber:iq:roster") {
			setRosterHandle(reply);
			os << "roster";
		} else if (xmlns == "http://jabber.org/protocol/muc#admin") {
			if (_doc->child("iq").child("query").child("item").attribute("role")) {
				//_mucSetPrivilege();
			} else if (_doc->child("iq").child("query").child("item").attribute("affiliation")) {
				_mucSetAffiliation();
				os << "_mucSetAffiliation";
			}
			try{
				TalkProxyAdapter::instance().express(_jid, reply.xml());
			}catch(Ice::Exception& e){
				MCE_WARN("IqSetTask::handle-->TalkProxyAdapter::express-->" << e);
			}
		} else if (xmlns == "http://jabber.org/protocol/muc#owner") {
			if (_doc->child("iq").child("query").child("destroy")) {
				_mucDestroyRoom();
			} else {
				//_mucSetRoomConfig();
			}
			try{
				TalkProxyAdapter::instance().express(_jid, reply.xml());
			}catch(Ice::Exception& e){
				MCE_WARN("IqSetTask::handle-->TalkProxyAdapter::express-->" << e);
			}
		}else if(xmlns == "http://talk.renren.com/removenotify" || xmlns == "http://talk.xiaonei.com/removenotify"){
			setNotifyHandle(reply);
			os << "setNotifyHandle";
		}
	} else {
		try{
			TalkProxyAdapter::instance().express(_jid, reply.xml());
		}catch(Ice::Exception& e){
			MCE_WARN("IqSetTask::handle-->TalkProxyAdapter::express-->" << e);
		}
	}
	FunStatManager::instance().Stat(os.str(), ts.getTime(), (ts.getTime() >= 250.0));
}

void IqSetTask::setNotifyHandle(xml_document& reply){
	StatFunc statF("IqSetTask::setNotifyHandle");
	MCE_DEBUG("IqSetTask::setNotifyHandle --> xml :  " << _doc->xml());
	bool notify_other = false;
/*
	xml_document dispatch_doc;
  xml_node iqNode = dispatch_doc.append_child();
  iqNode.set_name("iq");
  iqNode.append_attribute("type") = "set";
  iqNode.append_attribute("id") = (int)time(NULL);
  xml_node queryNode = iqNode.append_child();
  queryNode.set_name("query");
  queryNode.append_attribute("xmlns") = "http://talk.renren.com/removenotify";
*/
	string removetype = _doc->child("iq").first_child().attribute("type").value();
	MCE_DEBUG("IqSetTask::setNotifyHandle --> removetype = " << removetype);
	if("removebyid" == removetype){
	  notify_other = true;
	  xml_node nid_node = _doc->child("iq").child("query").child("notifyid");
	  Ice::Long notifyid = boost::lexical_cast<Ice::Long>(nid_node.first_child().value());
	  if(notifyid < 0){
	    notifyid = -notifyid;
	  }
	  try{
      MCE_DEBUG("IqSetTask::setNotifyHandle --> NotifyIndexAdapter::instance().removeById(" << _jid->userId <<", " << notifyid<< ")");
      NotifyIndexAdapter::instance().removeById(_jid->userId, notifyid);
    }catch(Ice::Exception& e){
      MCE_WARN("IqSetTask::setNotifyHandle-->NotifyIndexAdapter::removeById-->"<<e);
    }

/*
    queryNode.append_attribute("type") = "removebyid";
    xml_node dispatch_nid_node = queryNode.append_child();
    dispatch_nid_node.set_name("notifyid");
    dispatch_nid_node.append_child(node_pcdata).set_value((boost::lexical_cast<string>(nid_node.first_child().value())).c_str());
*/
	}
	else if("removebysource" == removetype){
	  notify_other = true;
    xml_node type_node = _doc->child("iq").child("query").child("stype");
    int type = boost::lexical_cast<int>(type_node.first_child().value());
    xml_node resource_node = _doc->child("iq").child("query").child("resource");
    Ice::Long resource = boost::lexical_cast<Ice::Long>(resource_node.first_child().value());
    if(type < 0){
      type = -type;
    }
    if(resource < 0){
      resource = -resource;
    }
    try{
      MCE_DEBUG("IqSetTask::setNotifyHandle --> NotifyIndexAdapter::instance().removeBySource(" << _jid->userId <<", "<<type << ", " << resource << ")");
      NotifyIndexAdapter::instance().removeBySource(_jid->userId, type, resource);
    }catch(Ice::Exception & e){
      MCE_WARN("IqSetTask::setNotifyHandle-->NotifyIndexAdapter::removeBySource-->"<<e);
    }

/*
    queryNode.append_attribute("type") = "removebysource";
    xml_node dispatch_stype_node = queryNode.append_child();
    xml_node dispatch_resource_node = queryNode.append_child();
    dispatch_stype_node.set_name("stype");
    dispatch_stype_node.append_child(node_pcdata).set_value((boost::lexical_cast<string>(type_node.first_child().value())).c_str());
    dispatch_resource_node.set_name("resource");
    dispatch_resource_node.append_child(node_pcdata).set_value((boost::lexical_cast<string>(resource_node.first_child().value())).c_str());
*/
	}
	//notify other avatars;
	else{
	  MCE_WARN("IqSetTask::setNotifyHandle --> unknown removetype = " << removetype);
	}
	if(notify_other){
	  MessageSeq mSeq;
		JidSeq self;
		try{
			self = OnlineCenterAdapter::instance().getUserJids(_jid->userId, 4);
		}catch(Ice::Exception& e){
			MCE_WARN("IqSetTask::setNotifyHandle-->OnlineCenterAdapter::getUserJids-->" << e);
		}
	  for(JidSeq::iterator it=self.begin(); it!=self.end(); ++it){
      //if((*it)->index != _jid->index ){
        MessagePtr mPtr = new Message();
        MCE_DEBUG("IqSetTask::setNotifyHandle --> notify self and other avatar from = " << jidToString(_jid) << " to = " << jidToString(*it) << " xml = " << _doc->xml());
        mPtr->to = *it;
        mPtr->type = IQ_SET;
        mPtr->msg = _doc->xml();
        //mPtr->msg = dispatch_doc.xml();
        mSeq.push_back(mPtr);
      //}
    }
	  if(!mSeq.empty()){
	    try{
	      MCE_DEBUG("IqSetTask::setNotifyHandle --> CALL TalkDeliverAdapter.deliver   size = " << mSeq.size());
	      TalkDeliverAdapter::instance().deliver(mSeq);
	    }
	    catch(Ice::Exception & e){
        MCE_WARN("IqSetTask::setNotifyHandle-->TalkDeliverAdapter::deliver-->"<<e);
      }
	  }
	}
}

void IqSetTask::setSessionHandle(xml_document& reply) {
	StatFunc statF("IqSetTask::setSessionHandle");
	xml_node iqNode = _doc->child("iq");
	xml_node sessionNode = iqNode.child("session");
	string xmlns = sessionNode.attribute("xmlns").value();
	MCE_DEBUG("@@@@@@IqSetTask::setSessionHandle-->jid=" << jidToString(_jid) << " reply:" << reply.xml() << " _doc.xml:" << _doc->xml());
	if (xmlns != "http://www.google.com/session") {
		//_session->deliverMsg(reply->xml());
		iqNode.attribute("type").set_value("result");

	MCE_DEBUG("!!!!!!!!IqSetTask::setSessionHandle-->jid=" << jidToString(_jid) << " reply:" << reply.xml() << " _doc.xml:" << _doc->xml());
		MessagePtr msg = new Message();
		msg -> from = _jid;
		msg -> to = _jid;
		msg -> type = IQ_RESULT;
		msg -> msg = _doc->xml();
		try{
		TalkProxyAdapter::instance().deliver(msg);
		}catch(Ice::Exception& e){
			MCE_WARN("IqSetTask::setSessionHandle-->TalkProxyAdapter::deliver-->" << e);
			}
	} else {
		xml_attribute to = iqNode.attribute("to");
		if (to) {
			iqNode.append_attribute("from") = jidToString(_jid).c_str();
			MessagePtr mPtr = new Message();
			mPtr->from = _jid;
			mPtr->to = stringToJid(string(to.value()));
			mPtr->type = IQ_SET;
			mPtr->msg = _doc->xml();

			MessageSeq seq;
			seq.push_back(mPtr);
			try{
			TalkProxyAdapter::instance().deliver(seq);
			}catch(Ice::Exception& e){
				MCE_WARN("IqSetTask::setSessionHandle-->TalkProxyAdapter::deliver-->"<< e);
				}
		}
	}
}

void IqSetTask::setPrivateHandle(xml_document& reply) {
	StatFunc statF("IqSetTask::setPrivateHandle");
	xml_node queryNode = reply.append_child();
	queryNode.set_name("query");
	queryNode.append_attribute("xmlns").set_value("jabber:iq:private");
	try{
	TalkProxyAdapter::instance().express(_jid, reply.xml());
	}catch(Ice::Exception& e){
		MCE_WARN("IqSetTask::setPrivateHandle-->TalkProxyAdapter::express-->" << e);
		}
}

void IqSetTask::setRosterHandle(xml_document& reply) {
	StatFunc statF("IqSetTask::setRosterHandle");
	string id = _doc->child("iq").attribute("id").value();
	xml_node itemN = _doc->child("iq").child("query").child("item");

	if (itemN) {
		string jid = itemN.attribute("jid").value();
		string name = itemN.attribute("name").value();
		string subscription = itemN.attribute("subscription").value();
		if (subscription == "remove") {
			//remove buddy
			//TalkLogicAdapter::instance().removeBuddy(_session->jid()->userId, stringToJid(jid)->userId);

			Friendship f;
			f.from = _jid->userId;
			f.to = stringToJid(jid)->userId;
			MCE_INFO("IqSetTask::setRosterHandle-->remove-->from:"<<f.from<<" to:" <<f.to << " doc:" << _doc->xml());
			try{
				BuddyCoreAdapter::instance().removeFriend(f);
			}catch(Ice::Exception& e){
				MCE_WARN("IqSetTask::setRosterHandle-->BuddyCoreAdapter::removeFriend-->" << e);
			}
		} else if (subscription == "" || subscription == "both") {
			MyUtil::StrSeq groups;
			for (xml_node n = itemN.child("group"); n; n
					= n.next_sibling("group")) {
				groups.push_back(n.first_child().value());
			}

			MCE_DEBUG("IqSetTask::setRosterHandle --> do setBuddyGroups");
			int desc;
			try{
				desc = BuddyGroupAdapter::instance().changeBuddyGroups(_jid->userId, stringToJid(jid)->userId, groups);
			}catch(Ice::Exception& e){
				MCE_WARN("IqSetTask::setRosterHandle-->BuddyGroupAdapter::changeBuddyGroups-->" << e);
			}
			MCE_DEBUG("desc:"<<desc<<"  firend:"<< com::xiaonei::service::buddy::Friend << "  host:"
					<<_jid->userId);
			if (desc == com::xiaonei::service::buddy::Friend) {
				MCE_DEBUG("host:"<<_jid->userId);
				//JidSeq jids = PresenceAdapter::instance().getOnlineUserJidSeq(_jid->userId);
				JidSeq jids;
				try{
					jids = OnlineCenterAdapter::instance().getUserJids(_jid->userId, 4);
				}catch(Ice::Exception& e){
					MCE_WARN("IqSetTask::setRosterHandle-->OnlineCenterAdapter::instance().getUserJids-->" << e)
				}
				MCE_DEBUG("IqSetTask::setRosterHandle --> host:"<<_jid->userId <<"   Jid size:" <<jids.size());

				//MessageSeq seq;
				for (size_t i = 0; i < jids.size(); ++i) {
					if (jids.at(i)->index <=1) {
						continue;
					}
					MessagePtr msg = new Message();
					msg->from = new Jid();
					msg->from->userId = jids.at(i)->userId;
					msg->from->index = -1;

					msg->to = jids.at(i);
					msg->type = IQ_SET;

					xml_document doc;
					xml_node iqNode = doc.append_child();
					iqNode.set_name("iq");
					iqNode.append_attribute("type") = "set";
					iqNode.append_attribute("id") = (int)time(NULL);
					iqNode.append_attribute("to") = jidToString(msg->to).c_str();

					xml_node queryNode = iqNode.append_child();
					queryNode.set_name("query");
					queryNode.append_attribute("xmlns") = "jabber:iq:roster";

					xml_node itemNode = queryNode.append_child();
					itemNode.set_name("item");
					itemNode.append_attribute("jid") = jid.c_str();
					itemNode.append_attribute("name") = name.c_str();
					itemNode.append_attribute("subscription") = "both";

					for (size_t j = 0; j < groups.size(); ++j) {
						xml_node groupNode = itemNode.append_child();
						groupNode.set_name("group");
						groupNode.append_child(node_pcdata).set_value(groups.at(j).c_str());
					}

					msg->msg = doc.xml();
					if (_jid->userId == 128487631) {
						MCE_DEBUG("msg:"<<msg->msg);
					}
					try{
						TalkDeliverAdapter::instance().deliver(msg);
					}catch(Ice::Exception& e){
						MCE_WARN("IqSetTask::setRosterHandle-->TalkDeliverAdapter::deliver-->" << e);
					}
				}

			}

		} else if (subscription == "from" || subscription == "to") {
			// ignore
		}
	}
	MessagePtr msg = new Message();
	msg -> from = _jid;
	msg -> to = _jid;
	msg -> type = IQ_RESULT;
	ostringstream iqMsg;
	iqMsg<<"<iq type='result' id='"<<id<<"'/>";
	msg -> msg = iqMsg.str();
	try{
		TalkProxyAdapter::instance().deliver(msg);
	}catch(Ice::Exception& e){
		MCE_WARN("IqSetTask::setRosterHandle-->TalkProxyAdapter::deliver-->" << e);
	}
}

void IqSetTask::setPrivacyHandle(xml_document& reply) {
	StatFunc statF("IqSetTask::setPrivacyHandle");
	xml_node queryNode = _doc->child("iq").child("query");
	vector<xml_node> listNodes;
	for (xml_node n = queryNode.child("list"); n; n = n.next_sibling("list")) {
		listNodes.push_back(n);
	}
	bool hasErr = false;
	bool removeHidden = true;

	if (listNodes.size() == 1) {
		if (listNodes.at(0).attribute("name").value()=="blocked") {
			MyUtil::IntSeq blockIds;
			MyUtil::StrSeq sysBlocks;
			for (xml_node n = listNodes.at(0).child("item"); n; n = n.next_sibling("item")) {
				if (n.attribute("action").value() != "deny") {
					hasErr = true;
					break;
				}

				if (n.attribute("type") && (n.attribute("type").value() == "jid")) {
					//block user
					string value = n.attribute("value").value();
					if (value[0]>='0' && value[0]<='9') {

						JidPtr jid = stringToJid(value);
						blockIds.push_back(jid->userId);

					} else {
						SysJidPtr sj = stringToSysJid(value);
						if (sj) {
							if (sj->category == "news") {
								sysBlocks.push_back(sj->node);
							}
						}
					}

					continue;
				}
				if ((!n.attribute("type")) && (!n.attribute("value"))
						&& n.child("presence-out")) {
					// hidden user
					removeHidden = false;
					try{
					TalkRightAdapter::instance().addHiddenUser(_jid->userId);
					}catch(Ice::Exception& e){
						MCE_WARN("IqSetTask::setPrivacyHandle-->TalkRightAdapter::instance().addHiddenUser-->" << e);
						}
					continue;
				}
				hasErr = true;
				break;

			}

			if (!hasErr) {
			try{
				BuddyCoreAdapter::instance().changeBlockList(_jid->userId, blockIds);
			}catch(Ice::Exception& e){
				MCE_WARN("IqSetTask::setPrivacyHandle-->BuddyCoreAdapter::changeBlockList-->" << e);
				}
				try{
				TalkRightAdapter::instance().modifySystemMessageBlock(_jid->userId, sysBlocks);
				}catch(Ice::Exception& e){
					MCE_WARN("IqSetTask::setPrivacyHandle-->TalkRightAdapter::modifySystemMessageBlock-->" << e);
					}
				if (removeHidden) {
					try{
					TalkRightAdapter::instance().removeHiddenUser(_jid->userId);
					}catch(Ice::Exception& e){
						MCE_WARN("IqSetTask::setPrivacyHandle-->TalkRightAdapter::removeHiddenUser-->" << e);
						}
				}

				return;
			}
		}
	}

	xml_node rIqNode = reply.child("iq");
	rIqNode.append_attribute("type") = "error";

	xml_node rErrorNode = rIqNode.append_child();
	rErrorNode.set_name("error");
	rErrorNode.append_attribute("type") = "cancel";

	xml_node rFeatureNotImpl = rErrorNode.append_child();
	rFeatureNotImpl.set_name("feature-not-implemented");
	rFeatureNotImpl.append_attribute("xmlns")
			= "urn:ietf:params:xml:ns:xmpp-stanzas";

	xml_node rTextNode = rErrorNode.append_child();
	rTextNode.set_name("text");
	rTextNode.append_attribute("xmlns") = "urn:ietf:params:xml:ns:xmpp-stanzas";
	rTextNode.append_child(node_pcdata).set_value("user can only set privacy into blocked list node , u want to block,in the list");

	MessagePtr msg = new Message();
	msg -> from = _jid;
	msg -> to = _jid;
	msg -> type = IQ_ERROR;
	msg -> msg = reply.xml();
	try{
	TalkProxyAdapter::instance().deliver(msg);
	}catch(Ice::Exception& e){
		MCE_WARN("IqSetTask::setPrivacyHandle-->TalkProxyAdapter::deliver-->" << e);
		}
}

void IqSetTask::setJingleHandle(xml_document& reply) {
	xml_node uniqueNode = reply.append_child();
	uniqueNode.set_name("unique");
	uniqueNode.append_attribute("xmlns").set_value("http://jabber.org/protocol/muc#unique");
	uniqueNode.append_child(node_pcdata).set_value(boost::lexical_cast<string>(_jid->index).c_str());
	try{
	TalkProxyAdapter::instance().express(_jid, reply.xml());
	}catch(Ice::Exception& e){
		MCE_WARN("IqSetTask::setJingleHandle-->TalkProxyAdapter::express-->" << e);
		}
}

void IqSetTask::_mucSetAffiliation() {
	xml_node iqNode = _doc->child("iq");
	string strTo = iqNode.attribute("to").value();
	string strFrom = iqNode.attribute("from").value();
	string id = iqNode.attribute("id").value();

	string strJid = iqNode.child("query").child("item").attribute("jid").value();
	string strAffiliation = iqNode.child("query").child("item").attribute("affiliation").value();

	JidPtr jidTo = stringToJid(strJid);
	if (strJid.empty() || strAffiliation.empty()) {
		MCE_DEBUG("IqSetTask::_mucSetAffiliation --> jid is empty");
		return;
	}
	MessageSeq msgs;
	Permisions permision = Affiliation2Permision(strAffiliation);
	MucRoomIdPtr roomId = stringToMucRoomId(strTo);
	bool setok = false;
	//MCE_INFO("IqSetTask::_mucSetAffiliation --> call MucGate.SetPermisionForIM userid:" << _jid->userId << " roomid:" << mucRoomIdToString(roomId) << " touser:" << jidTo->userId << " permision = " << permision);
	try{
	setok = MucGateAdapter::instance().SetPermisionForIM(_jid->userId, roomId, jidTo->userId, permision);
	}catch(Ice::Exception& e){
		MCE_WARN("IqSetTask::_mucSetAffiliation-->MucGateAdapter::SetPermisionForIM-->" << e);
		}
	if(setok){
		ostringstream os;
		os << "<iq from='"<< strTo << "' to='" << jidToString(_jid) << "' id='" << id << "' type='result'/>";
		MessagePtr msg = new Message();
		msg->from = _jid;
		msg->to = _jid;
		msg->msg = os.str();
		msg->type = IQ_SET;
		msgs.push_back(msg);
		try{
		MucDeliverAdapter::instance().deliver(msgs);
		}catch(Ice::Exception& e){
			MCE_WARN("IqSetTask::_mucSetAffiliation-->MucDeliverAdapter::deliver-->" << e);
			}
		return;
	}
	if (!setok) {
		ostringstream os;
		os << "<iq from='"<< strTo << "' to='" << jidToString(_jid) << "' id='"
				<< id << "' type='error'>"
				<< "<query xmlns='http://jabber.org/protocol/muc#admin'>"
				<< "<item affiliation='" <<strAffiliation<<"' jid='" << strJid
				<< "'/>" << "</query>" << "<error code='405' type='cancel'>"
				<< "<not-allowed xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
				<< "</error>" << "</iq>";
		MessagePtr msg = new Message();
		msg->from = _jid;
		msg->to = _jid;
		msg->msg = os.str();
		msg->type = IQ_SET;
		msgs.push_back(msg);
		try{
		MucDeliverAdapter::instance().deliver(msgs);
		}catch(Ice::Exception& e){
			MCE_WARN("IqSetTask::_mucSetAffiliation-->MucDeliverAdapter::deliver-->" << e);
			}
		return;
	}
/*
	ostringstream os;
	os << "<iq from='"<< strTo << "' to='" << jidToString(_jid) << "' id='"
			<< id << "' type='result'/>";
	MCE_DEBUG("IqSetTask::_mucSetPrivilege --> to Moderator " << os.str());
	MessagePtr msg = new Message();
	msg->from = _jid;
	msg->to = _jid;
	msg->msg = os.str();
	msg->type = IQ_SET;
	msgs.push_back(msg);
	MCE_DEBUG("RoomPresenceTask::handle --> deliver msgs " << msgs.size());
	MucDeliverAdapter::instance().deliver(msgs);

	OccupantSeq oseq;
	oseq = OccupantsAdapter::instance().getOccupants(roomId);
	MCE_DEBUG("IqSetTask::_mucSetAffiliation --> invoke Occupants::getOccupants "
			<< oseq.size());

	JidSeq jseq;
	for (size_t k = 0; k < tmpSeq.size(); ++k) {
		jseq.push_back(tmpSeq.at(k)->fJid);
	}
	//OnlineInfoSeq onlineInfo;
	AvatarSeq avatars;
	try {
		//onlineInfo = PresenceAdapter::instance().getOnlineUsersInfo(jseq);
		avatars = OnlineCenterAdapter::instance().getAvatars(jseq);
	} catch(Ice::Exception& e) {
		MCE_WARN("getOnlineUserInfo err:"<< e);
	}
	for (size_t k = 0; k < tmpSeq.size(); ++k) {
		if (tmpSeq.at(k)->role == NoneRole) {

			ostringstream os;
			os << "<presence from='"<< roomJidToString(tmpSeq.at(k)->rJid)<<"' to='"<<jidToString(tmpSeq.at(k)->fJid) << "' type='unavailable" <<"'>"
					<< "<x xmlns='http://jabber.org/protocol/muc#user'>"
					<< "<item affiliation='" << _mucAffiliationStr(tmpSeq.at(k)->aType) <<"' jid='" << strJid <<"' role='none'/>"
					<< "<status code='307'/>" << "</x>";
			//			if(onlineInfo.at(k) && onlineInfo.at(k)->status != ""){
			//				os << "<show>"<<onlineInfo.at(k)->status <<"</show>";
			//			}
			os << "</presence>";

			MessagePtr msg = new Message();
			msg->from = _jid;
			msg->to = tmpSeq.at(k)->fJid;
			msg->msg = os.str();
			msg->type = PRESENCE_COMMON;
			MCE_DEBUG(" " << os.str());
			msgs.push_back(msg);

		}

		for (size_t i = 0; i < oseq.size(); ++i) {
			//for(size_t j = 0; j < oseq.at(i)->jids.size(); ++j){
			ostringstream os;
			os << "<presence from='"<< roomJidToString(tmpSeq.at(k)->rJid)<<"' to='"<<jidToString(oseq.at(i)->fJid);
			if (tmpSeq.at(k)->role == NoneRole) {
				os << "' type='unavailable";
			}
			os <<"'>";
			if (tmpSeq.at(k)->role > NoneRole) {
				if (avatars.size()> k && avatars.at(k) && onlineStatStr(avatars.at(k)->stat) != "") {
					os << "<show>"<<onlineStatStr(avatars.at(k)->stat) <<"</show>";
				}
			}
			os << "<x xmlns='http://jabber.org/protocol/muc#user'>"
					<< "<item affiliation='" << _mucAffiliationStr(tmpSeq.at(k)->aType) <<"' jid='" << strJid <<"' role='"
					<< _mucRoleStr(tmpSeq.at(k)->role);
			os <<"'/>";
			if (tmpSeq.at(k)->role == NoneRole) {
				os << "<status code='307'/>";
			}
			os << "</x>" << "</presence>";

			MessagePtr msg = new Message();
			msg->from = _jid;
			msg->to = oseq.at(i)->fJid;
			msg->msg = os.str();
			msg->type = PRESENCE_COMMON;
			MCE_DEBUG(" " << os.str());
			msgs.push_back(msg);
			//}
		}
	}

	MCE_DEBUG("RoomPresenceTask::handle --> deliver msgs " << msgs.size());
	MucDeliverAdapter::instance().deliver(msgs);
	*/
}

void IqSetTask::_mucSetPrivilege() {
	/*
	xml_document reply;
	xml_node iqNode = _doc->child("iq");
	string strTo = iqNode.attribute("to").value();
	string strFrom = iqNode.attribute("from").value();
	string id = iqNode.attribute("id").value();

	string nick = iqNode.child("query").child("item").attribute("nick").value();
	string strRole = iqNode.child("query").child("item").attribute("role").value();
	MucRoomIdPtr roomid = stringToMucRoomId(strTo);
	if(!roomid){
		return;
	}
	MessageSeq msgs;
	if (tmpSeq.empty()) {
		MCE_DEBUG("IqSetTask::_mucSetPrivilege --> setting privilege failed ");
		ostringstream os;
		os << "<iq from='"<< strTo << "' to='" << jidToString(_jid) << "' id='"
				<< id << "' type='error'>"
				<< "<query xmlns='http://jabber.org/protocol/muc#admin'>"
				<< "<item nick='" <<nick<<"' role='" << strRole << "'/>"
				<< "</query>" << "<error code='405' type='cancel'>"
				<< "<not-allowed xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>"
				<< "</error>" << "</iq>";
		MessagePtr msg = new Message();
		msg->from = _jid;
		msg->to = _jid;
		msg->msg = os.str();
		msg->type = IQ_SET;
		msgs.push_back(msg);
		MCE_DEBUG("RoomPresenceTask::handle --> deliver msgs " << msgs.size());
		MucDeliverAdapter::instance().deliver(msgs);
		return;
	}

	JidSeq jseq;
	for (size_t i = 0; i < tmpSeq.size(); ++i) {
		jseq.push_back(tmpSeq.at(i)->fJid);
	}
	AvatarSeq avatars;
	//OnlineInfoSeq onlineInfo;
	try {
		//onlineInfo = PresenceAdapter::instance().getOnlineUsersInfo(jseq);
		avatars = OnlineCenterAdapter::instance().getAvatars(jseq);
	} catch(Ice::Exception& e) {
		MCE_WARN("getOnlineUserInfo err:"<<e);
	}

	MCE_DEBUG("IqSetTask::_mucSetPrivilege --> setting privilege succeed ");

	OccupantSeq oseq;
	try {
		oseq = OccupantsAdapter::instance().getOccupants(roomJid->rid);
	} catch(Ice::Exception& e) {
		MCE_WARN("IqSetTask::_mucSetPrivilege --> invoke setPrivilege " << " " <<"Ice::Exception:"<<e);
	} catch(...) {
		MCE_WARN("IqSetTask::_mucSetPrivilege --> invoke setPrivilege err");
	}
	MCE_DEBUG("IqSetTask::_mucSetPrivilege --> invoke Occupants::getOccupants "
			<< oseq.size());

	//Service Informs Moderator of Success
	ostringstream os;
	os << "<iq from='"<< strTo << "' to='" << jidToString(_jid) << "' id='"
			<< id << "' type='result'/>";
	MCE_DEBUG("IqSetTask::_mucSetPrivilege --> to Moderator " << os.str());
	MessagePtr msg = new Message();
	msg->from = _jid;
	msg->to = _jid;
	msg->msg = os.str();
	msg->type = IQ_SET;
	msgs.push_back(msg);

	//Service Sends Notice of Voice to All Occupants
	for (size_t k = 0; k < tmpSeq.size(); ++k) {
		if (strRole == "none") {
			//send to the occupant kicked
			string reason = iqNode.child("query").child("item").child("reason").first_child().value();
			MCE_DEBUG("IqSetTask::_mucSetPrivilege --> kick reason, " << reason);
			ostringstream os;
			os << "<presence from='"<< roomJidToString(tmpSeq.at(k)->rJid)<<"' to='"<<jidToString(tmpSeq.at(k)->fJid) << "' type='unavailable'>"
					<< "<x xmlns='http://jabber.org/protocol/muc#user'>"
					<< "<item affiliation='none' role='none'>" << "<reason>"
					<< reason <<"</reason>" << "</item>"
					<< "<status code='307'/>" << "</x>" << "</presence>";

			MessagePtr msg = new Message();
			msg->from = _jid;
			msg->to = tmpSeq.at(k)->fJid;
			msg->msg = os.str();
			msg->type = PRESENCE_COMMON;
			MCE_DEBUG(" " << os.str());
			msgs.push_back(msg);
		}
		for (size_t i = 0; i < oseq.size(); ++i) {
			//for(size_t j = 0; j < oseq.at(i)->jids.size(); ++j){
			ostringstream os;
			os << "<presence from='"<< roomJidToString(tmpSeq.at(k)->rJid)<<"' to='"<<jidToString(oseq.at(i)->fJid);
			if (strRole == "none") {
				os << "' type='unavailable";
			}
			os <<"'>";

			if (avatars.size()> k && avatars.at(k) && strRole != "none") {
				os << "<show>"<< onlineStatStr(avatars.at(k)->stat) <<"</show>";
			}

			os << "<x xmlns='http://jabber.org/protocol/muc#user'>"
					<< "<item affiliation='" << _mucAffiliationStr(tmpSeq.at(k)->aType) << "' nick='" << nick<<"' role='"<< strRole;
			if (Moderator == oseq.at(i)->role) {
				os << "' jid='" << jidToString(tmpSeq.at(k)->fJid);
			}
			os << "'/>";
			if (strRole == "none") {
				os << "<status code='307'/>";
			}
			os << "</x>" << "</presence>";

			MessagePtr msg = new Message();
			msg->from = _jid;
			msg->to = oseq.at(i)->fJid;
			msg->msg = os.str();
			msg->type = PRESENCE_COMMON;
			MCE_DEBUG(" " << os.str());
			msgs.push_back(msg);
			//}
		}
	}
	MCE_DEBUG("RoomPresenceTask::handle --> deliver msgs " << msgs.size());
	MucDeliverAdapter::instance().deliver(msgs);
	*/
}

void IqSetTask::_mucSetRoomConfig() {
	/*
	MCE_DEBUG("----IqSetTask::_mucSetRoomConfig----");

	string strTo = _doc->child("iq").attribute("to").value();
	RoomIdPtr roomId = stringToRoomId(strTo);

	RoomPtr room = TalkRoomAdapter::instance().getRoom(roomId);
	//room->type = new RoomType;
	xml_node n = _doc->child("iq").child("query").child("x").child("field");
	for (; n; n = n.next_sibling("field")) {
		if (n.attribute("var").value() == "muc#roomconfig_roomname") {
			room->title = n.child("value").first_child().value();
		} else if (n.attribute("var").value() == "muc#roomconfig_roomdesc") {
			room->descr = n.child("value").first_child().value();
		} else if (n.attribute("var").value() == "muc#roomconfig_persistentroom") {
			room->type->persistent = boost::lexical_cast<bool>(n.child("value").first_child().value());
		} else if(n.attribute("var").value() == "muc#roomconfig_publicroom") {
			room->type->hidden = !(boost::lexical_cast<bool>(n.child("value").first_child().value()));
		} else if(n.attribute("var").value() == "muc#roomconfig_passwordprotectedroom") {
			room->type->secured = boost::lexical_cast<bool>(n.child("value").first_child().value());
		} else if(n.attribute("var").value() == "muc#roomconfig_maxusers") {
			room->maxOccupantsCount = boost::lexical_cast<int>(n.child("value").first_child().value());
		} else if(n.attribute("var").value() == "muc#roomconfig_membersonly") {
			room->type->memberOnly = boost::lexical_cast<bool>(n.child("value").first_child().value());
		} else if(n.attribute("var").value() == "muc#roomconfig_moderatedroom") {
			room->type->moderated = boost::lexical_cast<bool>(n.child("value").first_child().value());
		} else if(n.attribute("var").value() == "muc#roomconfig_allowinvites") {
			room->allowUserToInvite = boost::lexical_cast<bool>(n.child("value").first_child().value());
		}
	}

	OccupantSeq tmpSeq = OccupantsAdapter::instance().setRoomConfig(_jid, roomId, room);
	OccupantSeq oseq = OccupantsAdapter::instance().getOccupants(roomId);

	JidSeq jseq;
	for(size_t i = 0; i < tmpSeq.size(); ++i) {
		jseq.push_back(tmpSeq.at(i)->fJid);
	}
	//OnlineInfoSeq onlineInfo;
	AvatarSeq avatars;
	try {
		//onlineInfo = PresenceAdapter::instance().getOnlineUsersInfo(jseq);
		avatars = OnlineCenterAdapter::instance().getAvatars(jseq);
	} catch(Ice::Exception& e) {
		MCE_WARN("getonlineuserinfo err:"<<e);
	}

	MessageSeq msgs;
	for (size_t k = 0; k < tmpSeq.size(); ++k) {
		if (tmpSeq.at(k)->role == NoneRole) {
			//for(size_t j = 0; j < tmpSeq.at(k)->jids.size(); ++j){
			ostringstream os;
			os << "<presence from='"<< roomJidToString(tmpSeq.at(k)->rJid)<<"' to='"<<jidToString(tmpSeq.at(k)->fJid);
			os << "' type='unavailable";
			os <<"'>" << "<x xmlns='http://jabber.org/protocol/muc#user'>"
			<< "<item affiliation='none' jid='" << jidToString(tmpSeq.at(k)->fJid, false) <<"' role='none'/>";
			os << "<status code='307'/>";
			os << "</x>" << "</presence>";

			MessagePtr msg = new Message();
			msg->from = _jid;
			msg->to = tmpSeq.at(k)->fJid;
			msg->msg = os.str();
			msg->type = PRESENCE_COMMON;
			MCE_DEBUG(" " << os.str());
			msgs.push_back(msg);
			//}
		}

		for (size_t i = 0; i < oseq.size(); ++i) {
			//for(size_t j = 0; j < oseq.at(i)->jids.size(); ++j){
			ostringstream os;
			os << "<presence from='"<< roomJidToString(tmpSeq.at(k)->rJid)<<"' to='"<<jidToString(oseq.at(i)->fJid);
			if (tmpSeq.at(k)->role == NoneRole) {
				os << "' type='unavailable";
			}
			os <<"'>";
			if (avatars.size()> k && avatars.at(k) && tmpSeq.at(k)->role> NoneRole) {
				os << "<show>"<<onlineStatStr(avatars.at(k)->stat)<<"</show>";
			}
			os << "<x xmlns='http://jabber.org/protocol/muc#user'>"
			<< "<item affiliation='" << _mucAffiliationStr(tmpSeq.at(k)->aType) <<"' jid='"
			<< jidToString(tmpSeq.at(k)->fJid, false) <<"' role='"
			<< _mucRoleStr(tmpSeq.at(k)->role);
			os <<"'/>";
			if (tmpSeq.at(k)->role == NoneRole) {
				os << "<status code='307'/>";
			}
			os << "</x>" << "</presence>";

			MessagePtr msg = new Message();
			msg->from = _jid;
			msg->to = oseq.at(i)->fJid;
			msg->msg = os.str();
			msg->type = PRESENCE_COMMON;
			MCE_DEBUG(" " << os.str());
			msgs.push_back(msg);
			//}
		}
	}
	MCE_DEBUG(__FUNCTION__ << " --> deliver msgs " << msgs.size());
	MucDeliverAdapter::instance().deliver(msgs);
	*/
}

void IqSetTask::_mucDestroyRoom() {
	MCE_DEBUG("----IqSetTask::_mucDestroyRoom----");
	string rJid = _doc->child("iq").child("query").child("destroy").attribute("jid").value();
	string strTo = _doc->child("iq").attribute("to").value();
	string id = _doc->child("iq").attribute("id").value();
	OccupantSeq oseq;
	try {
		oseq = OccupantsAdapter::instance().destroyRoom(_jid, stringToRoomId(strTo));
	} catch(Ice::Exception& e) {
		MCE_WARN("IqSetTask::_mucDestroyRoom-->OccupantsAdapter::destroyRoom" << " " <<"Ice:Exception:-->"<<e);
	} catch(...) {
		MCE_WARN("IqSetTask::_mucDestroyRoom --> invoke getOccupants err");
	}
	if (oseq.empty()) {
		MCE_DEBUG("IqSetTask::_mucDestroyRoom --> occupant seq is empty");
		return;
	}
	MessageSeq msgs;
	for (size_t i = 0; i < oseq.size(); ++i) {

		if (equal(_jid, oseq.at(i)->fJid)) {
			ostringstream os;
			os << "<iq from='"<< strTo << "' to='" << jidToString(_jid)
					<< "' id='" << id << "' type='result'>"
					<< "<query xmlns='http://jabber.org/protocol/muc#owner'/>"
					<< "</iq>";
			MessagePtr msg = new Message();
			msg->from = _jid;
			msg->to = oseq.at(i)->fJid;
			msg->msg = os.str();
			msg->type = PRESENCE_COMMON;
			MCE_DEBUG(" " << os.str());
			msgs.push_back(msg);
		}
		ostringstream os;
		os << "<presence from='"<< roomJidToString(oseq.at(i)->rJid)<<"' to='" <<jidToString(oseq.at(i)->fJid) << "' type='unavailable'>"
				<< "<x xmlns='http://jabber.org/protocol/muc#user'>"
				<< "<item affiliation='none' role='none'/>" << "<destroy jid='"
				<< rJid << "'/>" << "</x>" << "</presence>";

		MessagePtr msg = new Message();
		msg->from = _jid;
		msg->to = oseq.at(i)->fJid;
		msg->msg = os.str();
		msg->type = PRESENCE_COMMON;
		MCE_DEBUG(" " << os.str());
		msgs.push_back(msg);
	}
	MCE_DEBUG("IqSetTask::_mucDestroyRoom --> deliver msgs " << msgs.size());
	try{
	MucDeliverAdapter::instance().deliver(msgs);
	}catch(Ice::Exception& e){
		MCE_WARN("IqSetTask::_mucDestroyRoom-->MucDeliverAdapter::deliver-->" << e);
		}
}

Permisions IqSetTask::Role2Permision(string role){
	if (role == "moderator") {
		return PAdmin;
	} else if (role == "participant") {
		return PMember;
	} else if (role == "visitor") {
		return PMember;
	} else {
		return PNone;
	}
}
int IqSetTask::_mucGetRole(string role) {
	//enum Affiliation {Owner,Admin,Member,Outcast,None};
	//enum RoomRole {Moderator,Participant,Visitor,NoneRole};
	if (role == "moderator") {
		return Moderator;
	} else if (role == "participant") {
		return Participant;
	} else if (role == "visitor") {
		return Visitor;
	} else {
		return NoneRole;
	}
}
int IqSetTask::_mucGetAffiliation(string affiliation) {
	if (affiliation == "owner") {
		return Owner;
	} else if (affiliation == "admin") {
		return Admin;
	} else if (affiliation == "member") {
		return Member;
	} else if (affiliation == "outcast") {
		return Outcast;
	} else {
		return None;
	}
}
string IqSetTask::_mucRoleStr(int role) {
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
Permisions IqSetTask::Affiliation2Permision(const string& affiliation){
	if ("owner" == affiliation){
		return POwner;
	}
	else if("admin" == affiliation){
		return PAdmin;
	}
	else if("member" == affiliation){
		return PMember;
	}
	else if("none" == affiliation){
		return PNone; 
	}
	else if("outcast" == affiliation){
		return POutcast;
	}
	return PNone;

}
string IqSetTask::_mucAffiliationStr(int affiliation) {
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
//--------------------------------------
void IqResultTask::handle() {
	if (_doc->child("iq").attribute("to")) {
		JidPtr to = stringToJid(_doc->child("iq").attribute("to").value());
		_doc->child("iq").append_attribute("from") = jidToString(_jid).c_str();

		_doc->child("iq").remove_attribute("to");
		try{
		TalkProxyAdapter::instance().express(to, _doc->xml());
		}catch(Ice::Exception& e){
			MCE_WARN("IqResultTask::handle-->TalkProxyAdapter::express-->" << e);
			}
	}
}
//--------------------------------------
void IqErrorTask::handle() {
	if (_doc->child("iq").attribute("to")) {
		JidPtr to = stringToJid(_doc->child("iq").attribute("to").value());
		_doc->child("iq").append_attribute("from") = jidToString(_jid).c_str();

		_doc->child("iq").remove_attribute("to");
		try{
		TalkProxyAdapter::instance().express(to, _doc->xml());
		}catch(Ice::Exception& e){
			MCE_WARN("IqErrorTask::handle-->TalkProxyAdapter::express-->" << e);
			}
	}
}
//-------------------------------------------memcach test


void TalkCacheLoaderIdTask::handle(){
	try{
		MCE_INFO("TalkCacheLoaderIdTask::handle --> call TalkCacheClient.GetUserByIqWithLoad  userid = " << _id);
		TalkUserPtr u = TalkCacheClient::instance().GetUserByIdWithLoad(_id);
		if(!u){
			MCE_INFO("TalkCacheLoaderIdTask::handle --> get from TalkCacheClient is null id = " << _id);
			return;
		}
		MCE_INFO("TalkCacheLoaderIdTask::handle --> check user : id = " << u->id << " name = " << u->name << " statusOriginal = " << u->statusOriginal << " statusShifted = " << u->statusShifted << " headurl = " << u->headurl << " tinyurl = " << u->tinyurl);
	}catch(Ice::Exception& e){
    MCE_WARN("TalkCacheLoaderIdTask::handle-->TalkCacheClient::GetUserByIdWithLoad-->" << e);
  }
}

void TalkCacheLoaderSeqTask::handle(){
	try{
		MCE_INFO("TalkCacheLoaderSeqTask::handle --> call TalkCacheClient.GetUserBySeqWithLoad  userids.size = " << _ids.size());
		TalkUserMap s = TalkCacheClient::instance().GetUserBySeqWithLoad(id, _ids);
		if(s.size() != _ids.size()){
			MCE_INFO("TalkCacheLoaderSeqTask::handle --> get from TalkCacheClient size is equal ids.size = " << _ids.size() << " map.size = " << s.size());
		}
	}catch(Ice::Exception& e){
    MCE_WARN("TalkCacheLoaderSeqTask::handle-->:TalkCacheClient::GetUserBySeqWithLoad-->" << e);
  }
}

