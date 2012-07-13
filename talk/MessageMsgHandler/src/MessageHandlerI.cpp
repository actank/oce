#include "MessageHandlerI.h"
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
#include "Date.h"
//#include "MessageRouterAdapter.h"
#include "MessageTask.h"
#include "TalkUtil/src/pugixml.hpp"
#include "CountCacheAdapter.h"
#include "OnlineCenterAdapter.h"
#include "BuddyCoreAdapter.h"
#include "MessageAnalyst.h"
#include "MessageType.h"
#include "CommonTask.h"
#include "QueryRunner.h"
#include "UserCacheAdapter.h"
//#include "OnlineStatDispatchAdapter.h"


//#include "IMCommon.h"
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
using namespace xce::log;

//-----------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.getAdapter()->add(&MessageHandlerI::instance(), service.createIdentity("M", ""));
  PipePool::instance().initialize(new PipeFactoryI);
	int index = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Service."+ServiceI::instance().getName()+".Index", -1);
	if(index == -1){
		MCE_WARN("Config err --> index = -1");
		MessageHandlerI::instance().setIndex(0);
	}else{
		MessageHandlerI::instance().setIndex(index);
	}
	ActiveTrack::instance().initialize();

	TaskManager::instance().scheduleRepeated(& FirstSend2WTimer::instance());
	InitMonitorLogger(ServiceI::instance().getName(), "monitor", "../log/" + ServiceI::instance().getName() + "/monitor/monitor_log", "INFO");
}

//------------------------------------------------------------------------------

void MessageHandlerI::addLeaveword(LeavewordPtr& leaveword) {
	IceUtil::Mutex::Lock lock(_leavewordMutex);
	_leavewordSeq.push_back(leaveword);
}

void MessageHandlerI::swapLeaveword(LeavewordSeq& seq) {
	IceUtil::Mutex::Lock lock(_leavewordMutex);
	_leavewordSeq.swap(seq);
}


//----------------------

void MessageHandlerI::deliverAppNotice(
		Ice::Int toid,Ice::Int appid,const string& appName,
		const string& appIcon, const string& msg, const Ice::Current&){
	TimeStat ts;
	xml_document doc;
	xml_node msgNode = doc.append_child();
	msgNode.set_name("message");
	ostringstream from;
	from << appid << "@appnotice.talk.xiaonei.com";
	msgNode.append_attribute("from") = from.str().c_str();
	msgNode.append_attribute("type") = "common";
	xml_node subNode = msgNode.append_child();
	subNode.set_name("subject");
	subNode.append_child(node_pcdata).set_value("app notice");
	xml_node bodyNode = msgNode.append_child();
	bodyNode.set_name("body");
	bodyNode.append_child(node_pcdata).set_value(msg.c_str());
	xml_node appNode = msgNode.append_child();
	appNode.set_name("app");
	appNode.append_attribute("xmlns") = "http://talk.xiaonei.com/appnotice";
	xml_node idNode = appNode.append_child();
	idNode.set_name("id");
	idNode.append_child(node_pcdata).set_value(boost::lexical_cast<string>(
				appid).c_str());
	xml_node nameNode = appNode.append_child();
	nameNode.set_name("name");
	nameNode.append_child(node_pcdata).set_value(appName.c_str());
	xml_node iconNode = appNode.append_child();
	iconNode.set_name("icon");
	iconNode.append_child(node_pcdata).set_value(appIcon.c_str());
	xml_node mNode = appNode.append_child();
	mNode.set_name("msg");
	mNode.append_child(node_pcdata).set_value(msg.c_str());
	MessagePtr m = new Message();
	m->to = new Jid();
	m->to->userId = toid;
	m->msg = doc.xml();
	m->type = NOTIFY_MESSAGE;
	//MCE_DEBUG("WEventManagerI::deliverAppNotice --> "<< "  to:"<< toid <<"  msg:"<<doc.xml() );
	//PipePool::instance().push("AppNotice", 0, m);
	// 产品要求先去掉
	/*string pager_body = "<img class=\"notifyico\" src=\"";
		pager_body += appIcon;
		pager_body += "\"/><div class=\"notifybody\">";
		pager_body += msg;
		pager_body += "</div>";
	//pager_body html encoding
	//MyUtil::StrUtil::string_replace(pager_body, "&", "&amp;");
	//MyUtil::StrUtil::string_replace(pager_body, "<", "&lt;");
	//MyUtil::StrUtil::string_replace(pager_body, ">", "&gt;");
	deliverNotify(toid, appName, pager_body);*/
	PipePool::instance().push("AppNotice", 0, m);
	MCE_INFO("interface MessageMsgHandler::deliverAppNotice to id = " << toid << " usetime = " << ts.getTime());

};



void MessageHandlerI::deliverFeed(const string& msg, const string& type,
		const MyUtil::IntSeq& ids, const Ice::Current&) {
	StatFunc statF("MessageHandlerI::deliverFeed");
	//MCE_DEBUG("call MessageHandlerI::deliverFeed");
	ostringstream os;
	os<< "<message type='headline' from='"<<type<<"@news.talk.xiaonei.com'>"
			<< msg << "</message>";

	xml_document doc;
	if (!doc.load(os.str().c_str())) {
		MCE_WARN("Parser xml err !!!");
		return;
	}

	//TargetHandlerSeq res;
	JidSeqMap jmap;
	try{
	  jmap = OnlineCenterAdapter::instance().getUsersJids(ids);
	}catch(Ice::Exception& e){
		MCE_WARN("MessageHandlerI::deliverFeed-->OnlineCenterAdapter::getUsersJids-->" << e);
		}

	MessageSeq seq;
	for (JidSeqMap::iterator it = jmap.begin(); it != jmap.end(); ++it) {
		for (size_t i = 0; i < it->second.size(); ++i) {
			if (it->second.at(i)->index <= 1) {
				// this user is old im client or web client, ignore this feed
				continue;
			}

			MessagePtr mPtr = new Message();
			mPtr->from = new Jid();
			mPtr->from->userId = -1;
			mPtr->from->endpoint = type;
			mPtr->from->index = -1;
			mPtr->to = it->second.at(i);
			mPtr->type = SYSTEM_MESSAGE;

			doc.child("message").append_attribute("to") = jidToString(it->second.at(i),false).c_str();

			mPtr->msg = doc.xml();
			seq.push_back(mPtr);
			if (it->second.at(i)->userId == 128487631) {
				MCE_DEBUG("NEWS:"<<mPtr->msg);
			}
		}
	}
	if (!seq.empty()) {
		//NotifyTaskManager::instance().addTargetHandler(res);
		MCE_DEBUG("send news to Dispatch list");
		try{
		TalkDeliverAdapter::instance().deliver(seq);
		}catch(Ice::Exception& e){
			MCE_WARN("MessageHandlerI::deliverFeed-->TalkDeliverAdapter::deliver-->" << e);
			}
	}
	//NotifyTaskManager::instance().addNotify(new Notify< std::pair<string,MyUtil::IntSeq>,FeedNotifyHandler>(std::make_pair(msg,ids)));
}

//这个方法还有待考虑

void MessageHandlerI::message(const JidPtr& jid, const string& msg,
		const Ice::Current& current) {
	StatFunc statF("MessageHandlerI::message");
	string ip = "";
	Ice::Context::const_iterator it = current.ctx.find("ip");
	if(it != current.ctx.end()){
		ip = it->second;
	}
	MessageAnalyst::instance().handle(jid, msg, ip);
}

void MessageHandlerI::sendMessage(int from, int to, const string& msg, const Ice::Current&){
  JidSeq jseq;
  ostringstream os;
  os << "<message type=\"chat\" to=\"" << to << "@talk.renren.com\"></message>";
  xml_document doc;
  if(!doc.load(os.str().c_str())){
    return;
  }
  xml_node node = doc.child("message").append_child();
  node.set_name("body");
	node.append_child(node_pcdata).set_value(msg.c_str());
  JidPtr jid = new Jid;
  jid->userId = from;
  jid->endpoint = "Gossip";
  message(jid, doc.xml());
}

/*
void MessageHandlerI::loadOfflineMessage(const JidPtr& jid, const Ice::Current&) {
	MCE_DEBUG("call MessageHandlerI::loadOfflineMessage -->"<< jid->userId);
	StatFunc statF("MessageHandlerI::loadOfflineMessage");
	TaskManager::instance().execute(new LoadOfflineMessageTask(jid));
}*/
