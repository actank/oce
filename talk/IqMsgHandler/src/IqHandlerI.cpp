#include "IqHandlerI.h"
#include "ConnectionQuery.h"
#include "PresenceAdapter.h"
#include "PassportAdapter.h"
#include "TalkCacheAdapter.h"
#include "BuddyGroupAdapter.h"
#include "TalkDeliverAdapter.h"
#include "BuddyCoreAdapter.h"
#include "TalkCache.h"
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "XmppTools.h"
#include "PhotoUrlTools.h"
#include "Date.h"
//#include "MessageRouterAdapter.h"

#include "FeedMemcProxy/client/talk_cache_client.h"

#include "TalkUtil/src/pugixml.hpp"
#include "CountCacheAdapter.h"
#include "OnlineCenterAdapter.h"
#include "BuddyCoreAdapter.h"
#include "MessageAnalyst.h"
#include "MessageType.h"

#include "QueryRunner.h"
#include "UserCacheAdapter.h"
#include "UserCountAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

//#include "OnlineStatDispatchAdapter.h"

//#include "IMCommon.h"
using namespace xce::talk;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::cache;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::talk::common;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::model;
using namespace std;
using namespace com::xiaonei::service::buddy;
using namespace pugi;
using namespace talk::online::adapter;
using namespace xce::usercache;
using namespace xce::usercount::adapter;
//-----------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.getAdapter()->add(&IqHandlerI::instance(), service.createIdentity("M", ""));
	PhotoUrlHelper::instance().StartReloadTimer();
	InitMonitorLogger(ServiceI::instance().getName(), "monitor", "../log/" + ServiceI::instance().getName() + "/monitor/monitor_log", "INFO");


	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Service.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Service.Interval", 5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerIqMsgHandler", &IqHandlerI::instance(), mod, interval, new TalkChannel());
	MCE_INFO("MyUtil::initialize. mod:" << mod << " interval:" << interval);


}

//------------------------------------------------------------------------------

void IqHandlerI::removeBuddyNotify(Ice::Int from, Ice::Int to,
		const Ice::Current&) {
	StatFunc statF("IqHandlerI::removeBuddyNotify");

	//JidSeq toJids = PresenceAdapter::instance().getOnlineUserJidSeq(to);
	//JidSeq fromJids = PresenceAdapter::instance().getOnlineUserJidSeq(from);
	JidSeq toJids;
	JidSeq fromJids;
	try{
		toJids = OnlineCenterAdapter::instance().getUserJids(to, 4);
	}catch(Ice::Exception& e){
		MCE_WARN("IqHandlerI::removeBuddyNotify-->OnlineCenterAdapter::getUserJids-->err" << e);
	}
	try{
		fromJids = OnlineCenterAdapter::instance().getUserJids(from, 4);
	}catch(Ice::Exception& e){
		MCE_WARN("IqHandlerI::removeBuddyNotify-->OnlineCenterAdapter::getUserJids-->err" << e);	
	}
	if(toJids.empty() && fromJids.empty()){
		return;
	}
	MessageSeq seq;
	for (size_t i = 0; i < fromJids.size(); ++i) {

		ostringstream msg;
		msg<<"<iq type='set' id='"<<time(NULL)<<"'>"
				<<"<query xmlns='jabber:iq:roster'>" <<"<item jid='"<<to
				<<"@talk.xiaonei.com' subscription='remove' />"
				<<"</query></iq>";

		//			IqHandlerI::instance().getProxyServerOneway(fromJids.at(i)->endpoint)->deliver(fromJids.at(i), msg.str());
		MessagePtr mPtr = new Message();
		mPtr->from = new Jid();
		mPtr->from->userId = -1;
		mPtr->to = fromJids.at(i);
		mPtr->msg = msg.str();
		mPtr->type = IQ_SET;
		seq.push_back(mPtr);
	}

	for (size_t i = 0; i < toJids.size(); ++i) {

		JidPtr fromJid = new Jid();
		fromJid->userId = from;
		fromJid->endpoint = "talk.xiaonei.com";
		fromJid->index = -1;

		ostringstream msg;
		msg<<"<presence  from='"<<from<<"@talk.xiaonei.com' to='"
				<<jidToString(toJids.at(i))<<"' type='unsubscribed' id='"
				<<time(NULL) <<"'/>";

		MessagePtr mPtr = new Message();
		mPtr -> from = fromJid;
		mPtr -> to = toJids.at(i);
		mPtr -> msg = msg.str();
		mPtr -> type = PRESENCE_COMMON;
		seq.push_back(mPtr);

		msg.str("");
		msg<<"<iq type='set' id='"<<time(NULL)<<"'>"
				<<"<query xmlns='jabber:iq:roster'>" <<"<item jid='"<<from
				<<"@talk.xiaonei.com' subscription='remove' />"
				<<"</query></iq>";

		//			IqHandlerI::instance().getProxyServerOneway(toJids.at(i)->endpoint)->deliver(toJids.at(i), msg.str());
		mPtr = new Message();
		mPtr -> from = fromJid;
		mPtr -> to = toJids.at(i);
		mPtr -> msg = msg.str();
		mPtr -> type = IQ_SET;
		seq.push_back(mPtr);
	}
	try{
		TalkDeliverAdapter::instance().deliver(seq);
	}catch(Ice::Exception& e){
		MCE_WARN("IqHandlerI::removeBuddyNotify-->TalkDeliverAdapter::deliver-->err" << e);
	}
}

//这个方法还有待考虑
void IqHandlerI::notifySetPrivacy(const JidPtr& j, const string& msg,
		const Ice::Current&) {

}

void IqHandlerI::message(const JidPtr& jid, const string& msg,
		const Ice::Current&) {
	StatFunc statF("IqHandlerI::message");
	//MCE_DEBUG("IqHandlerI::message --> from:"<<jidToString(jid)<<"   msg:"<< msg);
	if (jid->userId == 128487631) {
	//	MCE_DEBUG("LogicManager::message --> userid:128487631   msg:"<<msg);
	}

	MessageAnalyst::instance().handle(jid, msg);
}

void IqHandlerI::changeBuddyGroupNotify(const BuddyItemPtr& bi, const Ice::Current&) {
  if(!bi){
    return;
  }
	MCE_DEBUG("IqHandlerI::changeBuddyGroupNotify --> host:"<<bi->host);
	MyUtil::IntSeq buddys;

	for (map<int,MyUtil::StrSeq>::iterator it = bi->items.begin(); it != bi->items.end(); ++it) {
		buddys.push_back(it->first);
	}
  TalkUserMap buddyInfo;
  try{
		buddyInfo = TalkCacheClient::instance().GetUserBySeqWithLoad(bi->host, buddys);
		if(buddyInfo.size() != buddys.size()){
			MCE_INFO("IqHandlerI::changeBuddyGroupNotify --> may be warn get from TalkCacheClient size is not equal actor = " << bi->host << " ids.size = " << buddys.size() << " map.size = " << buddyInfo.size());
		}else{
			MCE_INFO("IqHandlerI::changeBuddyGroupNotify --> success get from TalkCacheClient size is equal actor = " << bi->host << " ids.size = " << buddys.size() << " map.size = " << buddyInfo.size());
    }
	}catch(Ice::Exception& e){
    MCE_WARN("IqHandlerI::changeBuddyGroupNotify --> TalkCacheClient err, " << e);
  }
	
  MessageSeq mSeq;
  JidSeq jids;
  try{
	  jids = OnlineCenterAdapter::instance().getUserJids(bi->host, 4);
  }catch(Ice::Exception& e){
    MCE_WARN("IqHandlerI::changeBuddyGroupNotify-->OnlineCenterAdapter::getUserJids-->" << e);
  }
	for (size_t j = 0; j < jids.size(); ++j) {
		for (map<int,MyUtil::StrSeq>::iterator it = bi->items.begin(); it
				!= bi->items.end(); ++it) {
			xml_document doc;
			xml_node iqNode = doc.append_child();
			iqNode.set_name("iq");
			iqNode.append_attribute("type") = "set";
			iqNode.append_attribute("id") = (int)time(NULL);
			iqNode.append_attribute("to") = jidToString(jids.at(j)).c_str();

			xml_node queryNode = iqNode.append_child();
			queryNode.set_name("query");
			queryNode.append_attribute("xmlns") = "jabber:iq:roster";

			xml_node itemNode = queryNode.append_child();
			itemNode.set_name("item");
			ostringstream jos;
			jos << it->first << "@talk.xiaonei.com";
			itemNode.append_attribute("jid") = jos.str().c_str();//jid.c_str();

      TalkUserMap::iterator oit = buddyInfo.find(it->first);
			if (oit != buddyInfo.end()) {
				itemNode.append_attribute("name") = oit->second->name.c_str();
			} else {
				itemNode.append_attribute("name") = boost::lexical_cast<string>(it->first).c_str();
			}

			//itemNode.append_attribute("name") = name.c_str();
			itemNode.append_attribute("subscription") = "both";

			for (size_t k = 0; k < it->second.size(); ++k) {
				xml_node groupNode = itemNode.append_child();
				groupNode.set_name("group");
				groupNode.append_child(node_pcdata).set_value(it->second.at(k).c_str());
			}

			MessagePtr mPtr = new Message();
			//mPtr->from = new Jid();
			mPtr->from = jids.at(j);
			mPtr->to = jids.at(j);
			mPtr->type = IQ_SET;
			mPtr->msg = doc.xml();
			mSeq.push_back(mPtr);
			MCE_DEBUG("IqHandlerI::changeBuddyGroupNotify --> xml:"<<doc.xml());
		}
	}
	try{
		TalkDeliverAdapter::instance().deliver(mSeq);
	}catch(Ice::Exception& e){
		MCE_WARN("IqHandlerI::changeBuddyGroupNotify-->TalkDeliverAdapter::deliver-->err" << e);
	}
}


void IqHandlerI::RemoveNotifyBySource(int userId, int type, Ice::Long resource, const Ice::Current&){
	JidSeq jids;
	if(type > 0){
	  type = -type;
	}
	MCE_DEBUG("IqHandlerI::RemoveNotifyBySource --> userId : "<< userId << " type = " << type << " source = " << resource);
	try {
		jids = OnlineCenterAdapter::instance().getUserJids(userId, 4);
	}catch(Ice::Exception & e){
		MCE_WARN("IqHandlerI::RemoveNotifyBySource-->OnlineCenterAdapter::getUserJids-->"<<e);
	}
	if(!jids.empty()){
		for (JidSeq::iterator it = jids.begin(); it != jids.end() ; ++it){
			xml_document doc;
			xml_node iqNode = doc.append_child();
			iqNode.set_name("iq");
			iqNode.append_attribute("type") = "set";
			iqNode.append_attribute("id") = (int)time(NULL);
			xml_node queryNode = iqNode.append_child();
			queryNode.set_name("query");
			queryNode.append_attribute("xmlns") = "http://talk.renren.com/removenotify";
			queryNode.append_attribute("type") = "removebysource";

			xml_node type_node = queryNode.append_child();
			type_node.set_name("stype");
			string str_type = boost::lexical_cast<string>(type);
			type_node.append_child(node_pcdata).set_value(str_type.c_str());

			xml_node resource_node = queryNode.append_child();
			resource_node.set_name("resource");
      string str_resource = boost::lexical_cast<string>(resource);
      resource_node.append_child(node_pcdata).set_value(str_resource.c_str());

			MessagePtr mPtr = new Message();
			mPtr->to = *it;
			mPtr->type = IQ_SET;
			mPtr->msg = doc.xml();
			MCE_DEBUG("IqHandlerI::RemoveNotifyBySource --> to : "<< (*it)->userId <<" xml:"<<doc.xml());
			try {
				TalkDeliverAdapter::instance().deliver(mPtr);
			}catch(Ice::Exception & e){
				MCE_WARN("IqHandlerI::RemoveNotifyBySource-->TalkDeliverAdapter::deliver-->"<<e);
			}
		}
	}
}

void IqHandlerI::RemoveNotifyById(int userId, Ice::Long notifyId, const Ice::Current&){
  JidSeq jids;
  if(notifyId > 0){
    notifyId = -notifyId;
  }
  MCE_DEBUG("IqHandlerI::RemoveNotifyById --> userId : "<< userId << " notifyId = " << notifyId);
  try {
    jids = OnlineCenterAdapter::instance().getUserJids(userId, 4);
  }catch(Ice::Exception & e){
    MCE_WARN("IqHandlerI::RemoveNotifyById-->OnlineCenterAdapter::getUserJids-->"<<e);
  }
  if(!jids.empty()){
    for (JidSeq::iterator it = jids.begin(); it != jids.end() ; ++it){
      xml_document doc;
      xml_node iqNode = doc.append_child();
      iqNode.set_name("iq");
      iqNode.append_attribute("type") = "set";
      iqNode.append_attribute("id") = (int)time(NULL);
      xml_node queryNode = iqNode.append_child();
      queryNode.set_name("query");
      queryNode.append_attribute("xmlns") = "http://talk.renren.com/removenotify";
      queryNode.append_attribute("type") = "removebyid";

      xml_node nid_node = queryNode.append_child();
      nid_node.set_name("notifyid");
      string str_nid = boost::lexical_cast<string>(notifyId);
      nid_node.append_child(node_pcdata).set_value(str_nid.c_str());

      MessagePtr mPtr = new Message();
      mPtr->to = *it;
      mPtr->type = IQ_SET;
      mPtr->msg = doc.xml();
      MCE_DEBUG("IqHandlerI::RemoveNotifyById --> to : "<< (*it)->userId <<" xml:"<<doc.xml());
      try {
        TalkDeliverAdapter::instance().deliver(mPtr);
      }catch(Ice::Exception & e){
        MCE_WARN("IqHandlerI::RemoveNotifyById-->TalkDeliverAdapter::deliver-->"<<e);
      }
    }
  }
}


void IqHandlerI::NotifyBuddyApplyChange(int userId, const Ice::Current&){
	JidSeq jids;
	MCE_DEBUG("IqHandlerI::NotifyBuddyApplyChange --> userId : "<< userId);
	try {
		jids = OnlineCenterAdapter::instance().getUserJids(userId, 4);
	}catch(Ice::Exception & e){
		MCE_WARN("IqHandlerI::NotifyBuddyApplyChange-->OnlineCenterAdapter::getUserJids-->"<<e);
	}
	if(!jids.empty()){
		for (JidSeq::iterator it = jids.begin(); it != jids.end() ; ++it){
			GetBuddyApply(*it);
		}
	}
}

void IqHandlerI::GetBuddyApply(const JidPtr& jid){
	MCE_DEBUG("IqHandlerI::NotifyBuddyApplyChange --> jid:"<< jidToString(jid));
	int tmpcount = 0;
	try {
		tmpcount = UserCountAdapter::instance().get(jid->userId, 3);
	}catch(Ice::Exception & e){
		MCE_WARN("IqHandlerI::GetBuddyApply-->UserCountAdapter::get-->"<<e);
	}
	string budyapplycount = boost::lexical_cast<string>(tmpcount);

	xml_document doc;
	xml_node iqNode = doc.append_child();
	iqNode.set_name("iq");
	iqNode.append_attribute("type") = "set";
	iqNode.append_attribute("id") = (int)time(NULL);

	xml_node queryNode = iqNode.append_child();
	queryNode.set_name("query");
	queryNode.append_attribute("xmlns") = "http://talk.renren.com/buddyapplycount";

	xml_node countNode = queryNode.append_child();
	countNode.set_name("applycount");
	countNode.append_child(node_pcdata).set_value(budyapplycount.c_str());
	MessagePtr mPtr = new Message();

	mPtr->to = jid;
	mPtr->type = IQ_SET;
	mPtr->msg = doc.xml();

	//MCE_INFO("IqHandlerI::NotifyBuddyApplyChange --> xml:"<<doc.xml());
	try {
		TalkDeliverAdapter::instance().deliver(mPtr);
	}catch(Ice::Exception & e){
		MCE_WARN("IqHandlerI::GetBuddyApply-->TalkDeliverAdapter::deliver-->"<<e);
	}
}
