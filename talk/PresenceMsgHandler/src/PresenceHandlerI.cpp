#include "PresenceHandlerI.h"
#include "PresenceTask.h"
#include "PresenceAdapter.h"
#include "BuddyByIdReplicaAdapter.h"
//#include "PassportAdapter.h"
//#include "BuddyGroupAdapter.h"
#include "TalkDeliverAdapter.h"
//#include "BuddyCoreAdapter.h"
#include "OnlineCenterAdapter.h"
#include "MucPresenceAdapter.h"
#include "BuddyFriendsAdapter.h"
#include "TalkCache.h"
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "XmppTools.h"
#include "Date.h"
//#include "MessageRouterAdapter.h"

#include "TalkUtil/src/pugixml.hpp"
//#include "CountCacheAdapter.h"
//#include "OnlineBuddyAdapter.h"
//#include "BuddyCoreAdapter.h"
#include "MessageAnalyst.h"
#include "MessageType.h"
#include "CommonTask.h"
//#include "QueryRunner.h"
#include "UserCacheAdapter.h"
//#include "OnlineStatDispatchAdapter.h"
#include "TalkStatAdapter.h"
//#include "IMCommon.h"
#include "MessageHolderAdapter.h"
#include "OnlineEventMediatorAdapter.h"
#include "UserCacheAdapter.h"

//#include "ActiveUserNotify.h"
#include "IMWindowAdapter.h"
#include "MucGateAdapter.h"
using namespace com::xiaonei::talk::muc;
#include "FeedMemcProxy/client/talk_cache_client.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::talk;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::cache;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::talk::common;
using namespace mop::hi::svc::adapter;
//using namespace com::xiaonei::model;
using namespace std;
//using namespace com::xiaonei::service::buddy;
using namespace pugi;
using namespace talk::online::adapter;
using namespace xce::usercache;
using namespace talk::stat;
using namespace talk::adapter;
using namespace talk::window;
using namespace xce::mediator::onlineevent;
using namespace xce::buddy::adapter;
//-----------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.getAdapter()->add(&PresenceHandlerI::instance(), service.createIdentity("M",
			""));
	TaskManager::instance().scheduleRepeated(&KickUserTimer::instance());
//	TaskManager::instance().scheduleRepeated(&ActiveUserNotify::instance());
	InitMonitorLogger(ServiceI::instance().getName(), "monitor", "../log/" + ServiceI::instance().getName() + "/monitor/monitor_log", "INFO");

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Service.Mod");
//	int index = service.getCommunicator()->getProperties()->getPropertyAsInt("Service.ServerIndex");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Service.Interval", 5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerPresenceMsgHandler", &PresenceHandlerI::instance(), mod, interval, new TalkChannel());
	MCE_INFO("MyUtil::initialize. mod:" << mod << " interval:" << interval<<"   index:"<<index);
//  ActiveUserNotify::instance().setIndex(index);
}


//------------------------------------------------------------------------------

void PresenceHandlerI::presence(const JidPtr& jid, const string& show,
    const string& ver, const string& subver, const string& ext, const Ice::Current&) {
	TaskManager::instance().execute(new PushOnlineBuddyTask(jid, show, ver, subver, ext));
}

void PresenceHandlerI::presenceWithSecver(const JidPtr& jid, const string& show,
    const string& ver, const string& subver, const string& ext, const string& secver, const Ice::Current&) {
	TaskManager::instance().execute(new PushOnlineBuddyTask(jid, show, ver, subver, ext, secver));
}
//----------------------
void PushOnlineBuddyTask::handle() {
	TimeStat ts, talkcache_ts;
	float vtalkcache;
	//StatFunc statF("PresenceHandlerI::presence");
	MCE_INFO("~~PresenceHandlerI::presence --> " << jidToString(jid)
		<< " show:" << show
		<< " ver:" << ver << " subver:" << subver
		<< " ext:" << ext << "  secver:"<< secver);
	try{
		PresenceAdapter::instance().bind(jid);
	}
	catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presence-->PresenceAdapter::instance().bind-->" << jidToString(jid) << "Ice::Exception : " << e);
	}
  string merged_ver = ver;
  if (secver != "") {
    const size_t point_pos = merged_ver.find_first_of('.');
    if (point_pos != string::npos) {
      const size_t non_zero_pos = merged_ver.find_first_not_of('0', point_pos+1);
      if (non_zero_pos!=string::npos)
        merged_ver.erase(point_pos+1, non_zero_pos - point_pos-1);
    }
    merged_ver += ".";
    merged_ver += secver; 
  }
	try {
		MCE_INFO("PresenceHandlerI::presence-->-    merged_ver:->"<< merged_ver);
		//PresenceAdapter::instance().bind(jid);
		TalkStatAdapter::instance().versionStat(jid->userId, merged_ver, subver);
	//	TalkStatAdapter::instance().versionStat(jid->userId, ver, subver);
	} catch(Ice::Exception& e) {
		MCE_WARN("PresenceHandlerI::presence-->TalkStatAdapter::instance().versionStat-->"<<jidToString(jid)<< "Ice::Exception : " << e);
	}

	AvatarMap avatars;
	//AvatarSeq seq;
	try {

		avatars = OnlineCenterAdapter::instance().getBuddyAvatarsAndSelf(jid->userId);
		//seq = OnlineCenterAdapter::instance().getUserAvatars(jid->userId);
	} catch(Ice::Exception& e) {
		MCE_WARN("PresenceHandlerI::presence-->OnlineBuddyAdapter::getBuddyIds-->userId:"<<jid->userId<<""<<e);
	}

	AvatarMap::iterator tmpIt = avatars.find(jid->userId);
	if(tmpIt!=avatars.end()){
		for(AvatarSeq::iterator seqIt=tmpIt->second.begin(); seqIt!=tmpIt->second.end(); ++seqIt){
			if((*seqIt)->jid->index == jid->index){
				tmpIt->second.erase(seqIt);
				break;
			}
		}
	}

	MyUtil::IntSeq onlineIds;
	for (AvatarMap::iterator it = avatars.begin(); it != avatars.end(); it++) {
		onlineIds.push_back(it->first);
	}
	TalkUserMap usermaps;
	try {
		//MCE_INFO("## get profile " << onlineIds.size());
		talkcache_ts.reset();
		usermaps = TalkCacheClient::instance().GetUserBySeqWithLoad(jid->userId, onlineIds);
		vtalkcache = talkcache_ts.getTime();
		MONITOR("SignForTimeuse::PresenceHandlerI::presence-->TalkCacheClient::GetUserBySeqWithLoad-->" << vtalkcache);
	} catch(Ice::Exception& e) {
		MCE_WARN("PresenceHandlerI::presence->TalkCacheClient::GetUserBySeqWithLoad-->"<<jid->userId<<":"<<e);
	}
	
	map<int, UserCachePtr> userVips;
	try{
		userVips = UserCacheAdapter::instance().GetUserCache(onlineIds);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presence-->UserCacheAdapter::GetUserCache-->userId:"<<jid->userId<<" Ice::Exception:"<<e);
	}

	MessageSeq mSeq;
	for (TalkUserMap::iterator it = usermaps.begin(); it != usermaps.end(); ++it) {
		TalkUserPtr u = it->second;
		int vip = 0;
		UserCachePtr vipInfo; 
		map<int, UserCachePtr>::iterator it = userVips.find(u->id);
		if(it != userVips.end()){
			vipInfo = it->second;
			//MCE_INFO("## vip info " << u->id << " " << vipInfo->level() << " " << vipInfo->vip() << " " << vipInfo->ever_vip());
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

		AvatarMap::iterator eit = avatars.find(u->id);
		if (eit != avatars.end()) {
			MessageSeq tmpSeq;
			int isInvisiable = false;
			for (size_t j = 0; j < eit->second.size(); ++j) {
				if(eit->second.at(j)->stat == INVISIABLE){
					isInvisiable = true;
					break;
				}
				xml_document doc;
				xml_node presenceNode = doc.append_child();
				presenceNode.set_name("presence");
				presenceNode.append_attribute("from") = jidToString(eit->second.at(j)->jid).c_str();
				presenceNode.append_attribute("to") = jidToString(jid).c_str();

				xml_node priorityNode = presenceNode.append_child();
				priorityNode.set_name("priority");
				priorityNode.append_child(node_pcdata).set_value("0");

				xml_node statusNode = presenceNode.append_child();
				statusNode.set_name("status");
				statusNode.append_child(node_pcdata).set_value(u->statusOriginal.c_str());
				string show = onlineStatStr(eit->second.at(j)->stat);
				if (show != "") {
					xml_node showNode = presenceNode.append_child();
					showNode.set_name("show");
					showNode.append_child(node_pcdata).set_value(show.c_str());
				}
				xml_node nickNode = presenceNode.append_child();
				nickNode.set_name("nick");
				nickNode.append_attribute("xmlns")
						= "http://jabber.org/protocol/nick";
				nickNode.append_child(node_pcdata).set_value(u->name.c_str());

				xml_node xNode = presenceNode.append_child();
				xNode.set_name("x");
				xNode.append_attribute("xmlns") = "vcard-temp:x:update";

				xml_node photoNode = xNode.append_child();
				photoNode.set_name("PHOTO");

				xml_node extvalNode = photoNode.append_child();
				extvalNode.set_name("EXTVAL");
				extvalNode.append_child(node_pcdata).set_value(buildHeadUrl(u->headurl).c_str());

				xml_node cNode = presenceNode.append_child();
				cNode.set_name("c");
				cNode.append_attribute("xmlns")
						= "http://jabber.org/protocol/caps";
				cNode.append_attribute("node")
						= "http://talk.xiaonei.com/xmpp/client/caps";
				cNode.append_attribute("ver") = eit->second.at(j)->ver.c_str();
				cNode.append_attribute("ext") = eit->second.at(j)->ext.c_str();


				xml_node embHtmlNode = presenceNode.append_child();
				embHtmlNode.set_name("richstatus");
				embHtmlNode.append_child(node_pcdata).set_value(u->statusShifted.c_str());
				
				xml_node vipNode = presenceNode.append_child();
				vipNode.set_name("vip");
				vipNode.append_child(node_pcdata).set_value(strVip.c_str());


				MessagePtr mPtr = new Message();
				mPtr->from = eit->second.at(j)->jid;

				mPtr->to = jid;
				mPtr->type = PRESENCE_COMMON;
				mPtr->msg = doc.xml();
				tmpSeq.push_back(mPtr);
			}
			if(!isInvisiable){
				mSeq.insert(mSeq.end(), tmpSeq.begin(), tmpSeq.end());
			}
		}

	}
	try{
		TalkDeliverAdapter::instance().deliver(mSeq);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presence-->TalkDeliverAdapter::deliver" << e);
	}
	try{
		MessageHolderAdapter::instance().loadOfflineMessage(jid);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presence-->MessageHolderAdapter::loadOfflineMessage-->" << e);
	}
	try{
	  float v = boost::lexical_cast<float>(ver);
//	  ActiveUserNotify::instance().needNotify(jid->userId,v);
    IMWindowAdapter::instance().NeedNotify(jid->userId,v);
	}catch(Ice::Exception& e){
	  MCE_WARN("call ActionUserNotify err:"<<e << ", line:"<<__LINE__);
	}catch(std::exception& e){
	  MCE_WARN("call ActionUserNotify err:"<<e.what() << ", line:"<<__LINE__);
	}
	FunStatManager::instance().Stat("presence", ts.getTime(), (ts.getTime() >= FUNTIMEOUT));
}

void PresenceHandlerI::presenceSubscribeWithPostscript(const string& id, Ice::Int from,
		Ice::Int to, const string& status, const string& postscript, const Ice::Current&) {
	StatFunc statF("PresenceHandlerI::presenceSubscribe");
	TimeStat ts;
	//-----------------------------------------------------------------------
	//add by guoqing.liu  当被发送请求的人的好友数量达到2000 上线的时候，就抛弃这条协议
	int to_friend_count = 0;
	try{
		to_friend_count = BuddyByIdCacheAdapter::instance().getFriendCount(to);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceSubscribeWithPostscript --> BuddyByIdCacheAdapter::getFriendCount --> getid = " << to << " err : " << e);
	}
	MCE_INFO("PresenceHandlerI::presenceSubscribeWithPostscript --> id = " << id << " from = " << from << " to = " << to << " status = " << status 
      <<"  postscipt:"<< postscript << " to.friends.size = " << to_friend_count);
	if(to_friend_count >= 2000){
		MCE_INFO("PresenceHandlerI::presenceSubscribeWithPostscript --> cost_time = " << ts.getTime());
		return;
	}
	
	//-----------------------------------------------------------------------
  JidSeq toJids;
	try{
		toJids = OnlineCenterAdapter::instance().getUserJids(to, 4);
	}
	catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceSubscribeWithPostscript-->OnlineCenterAdapter::getUserJids-->" << e);
	}
	JidSeq fromJids;
	try{
		fromJids = OnlineCenterAdapter::instance().getUserJids(from, 4);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceSubscribeWithPostscript-->OnlineCenterAdapter::getUserJids-->" << e);
	}
  MCE_INFO("PresenceHandlerI::presenceSubscribeWithPostscript--> toJids.size():" << toJids.size() <<"   fromJids.size:" << fromJids.size());
	if (!toJids.empty()) {
		TalkUserPtr fromUser;
		try{
			fromUser = TalkCacheClient::instance().GetUserByIdWithLoad(from);
		}catch(Ice::Exception& e){
	  	MCE_WARN("PresenceHandlerI::presenceSubscribeWithPostscript-->TalkCacheClient::GetUserByIdWithLoad-->"<<e << ", line:"<<__LINE__);
		}
    MCE_INFO("PresenceHandlerI::presenceSubscribeWithPostscript-->  tttt");
		if (fromUser) {
			for (size_t i = 0; i < toJids.size(); ++i) {
				xml_document doc;
				xml_node presenceNode = doc.append_child();
				presenceNode.set_name("presence");
				presenceNode.append_attribute("from") = createJid(from).c_str();
				presenceNode.append_attribute("to") = jidToString(toJids.at(i)).c_str();
				presenceNode.append_attribute("type") = "subscribe";
				presenceNode.append_attribute("id") = id.c_str();

				xml_node nickNode = presenceNode.append_child();
				nickNode.set_name("nick");
				nickNode.append_attribute("xmlns")
						= "http://jabber.org/protocol/nick";
				nickNode.append_child(node_pcdata).set_value(fromUser->name.c_str());

				xml_node xNode = presenceNode.append_child();
				xNode.set_name("x");
				xNode.append_attribute("xmlns") = "vcard-temp:x:update";
				xml_node photoNode = xNode.append_child();
				photoNode.set_name("photo");
				xml_node extvalNode = photoNode.append_child();
				extvalNode.set_name("EXTVAL");
				extvalNode.append_child(node_pcdata).set_value(buildHeadUrl(fromUser->headurl).c_str());

				xml_node statusNode = presenceNode.append_child();
				statusNode.set_name("status");
				statusNode.append_child(node_pcdata).set_value(status.c_str());

        xml_node postscriptNode = presenceNode.append_child();
        postscriptNode.set_name("postscript");
			  postscriptNode.append_child(node_pcdata).set_value(postscript.c_str());

        Ice::Int common_friends_count = 0;
        try {
          common_friends_count = BuddyFriendsCacheAdapter::instance().getCommonFriends(from, to).size();
        } catch (Ice::Exception &e) {
          MCE_WARN("BuddyList::load -> get buddies error" << e); 
        }
        string number_of_buddies;
	    	try{
			    number_of_buddies = boost::lexical_cast<string>(common_friends_count);
		    }catch(...){
			    MCE_WARN("PresenceHandlerI::presence --> lexical cast err");
    		}
        xml_node numberOfBuddiesNode = presenceNode.append_child();
        numberOfBuddiesNode.set_name("numberofbuddies");
			  numberOfBuddiesNode.append_child(node_pcdata).set_value(number_of_buddies.c_str());

				ostringstream os;
				doc.print(os);

				MCE_INFO("PresenceHandlerI::presenceSubscribeWithPostscript-->  postscript:" << os.str().c_str());

				MessagePtr msg = new Message();
				msg->from = new Jid();
				msg->from->userId = from;
				msg->from->index = -1;

				msg->to = toJids.at(i);
				msg->msg = os.str();
				msg->type = PRESENCE_COMMON;
				try{
					TalkDeliverAdapter::instance().deliver(msg);
				}catch(Ice::Exception& e){
					MCE_WARN("PresenceHandlerI::presenceSubscribeWithPostscript-->TalkDeliverAdapter::deliver-->" << e);
				}
			}
		}
	}
	TalkUserPtr toUser;
	try{
		toUser = TalkCacheClient::instance().GetUserByIdWithLoad(to);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceSubscribeWithPostscript-->TalkCacheClient::GetUserByIdWithLoad-->" << e);
	}
	for (size_t i = 0; i < fromJids.size(); ++i) {

		xml_document doc;
		xml_node iqNode = doc.append_child();
		iqNode.set_name("iq");
		iqNode.append_attribute("from") = createJid(from).c_str();
		iqNode.append_attribute("to") = jidToString(fromJids.at(i)).c_str();
		iqNode.append_attribute("id") = (int)time(NULL);
		iqNode.append_attribute("type") = "set";

		xml_node queryNode = iqNode.append_child();
		queryNode.set_name("query");
		queryNode.append_attribute("xmlns") = "jabber:iq:roster";

		xml_node itemNode = queryNode.append_child();
		itemNode.set_name("item");
		itemNode.append_attribute("ask") = "subscribe";
		itemNode.append_attribute("subscription") = "none";
		if (toUser) {
			itemNode.append_attribute("name") = toUser->name.c_str();
		}
		itemNode.append_attribute("jid") = createJid(to).c_str();

		ostringstream os;
		doc.print(os);

		MessagePtr msg = new Message();
		msg->from = new Jid();
		msg->from->userId = from;
		//msg->from->endpoint = "";
		msg->from->index = -1;

		msg->to = fromJids.at(i);
		msg->msg = os.str();
		msg->type = IQ_SET;
		try{
			TalkDeliverAdapter::instance().deliver(msg);
		}catch(Ice::Exception& e){
			MCE_WARN("PresenceHandlerI::presenceSubscribeWithPostscript-->TalkDeliverAdapter::deliver-->" << e);
		}
	}
	FunStatManager::instance().Stat("presenceSubscribe", ts.getTime(), (ts.getTime() >= FUNTIMEOUT));
}

void PresenceHandlerI::presenceSubscribe(const string& id, Ice::Int from,
		Ice::Int to, const string& status, const Ice::Current&) {
	StatFunc statF("PresenceHandlerI::presenceSubscribe");
	TimeStat ts;
	//-----------------------------------------------------------------------
	//add by guoqing.liu  当被发送请求的人的好友数量达到2000 上线的时候，就抛弃这条协议
	int to_friend_count = 0;
	try{
		to_friend_count = BuddyByIdCacheAdapter::instance().getFriendCount(to);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceSubscribe --> BuddyByIdCacheAdapter::getFriendCount --> getid = " << to << " err : " << e);
	}
	MCE_INFO("PresenceHandlerI::presenceSubscribe --> id = " << id << " from = " << from << " to = " << to << " status = " << status << " to.friends.size = " << to_friend_count);
	if(to_friend_count >= 2000){
		MCE_INFO("PresenceHandlerI::presenceSubscribe --> cost_time = " << ts.getTime());
		return;
	}
	
	//-----------------------------------------------------------------------
  JidSeq toJids;
	try{
		toJids = OnlineCenterAdapter::instance().getUserJids(to, 4);
	}
	catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceSubscribe-->OnlineCenterAdapter::getUserJids-->" << e);
	}
	JidSeq fromJids;
	try{
		fromJids = OnlineCenterAdapter::instance().getUserJids(from, 4);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceSubscribe-->OnlineCenterAdapter::getUserJids-->" << e);
	}
	if (!toJids.empty()) {
		TalkUserPtr fromUser;
		try{
			fromUser = TalkCacheClient::instance().GetUserByIdWithLoad(from);
		}catch(Ice::Exception& e){
	  	MCE_WARN("PresenceHandlerI::presenceSubscribe-->TalkCacheClient::GetUserByIdWithLoad-->"<<e << ", line:"<<__LINE__);
		}
		if (fromUser) {
			for (size_t i = 0; i < toJids.size(); ++i) {

				xml_document doc;
				xml_node presenceNode = doc.append_child();
				presenceNode.set_name("presence");
				presenceNode.append_attribute("from") = createJid(from).c_str();
				presenceNode.append_attribute("to") = jidToString(toJids.at(i)).c_str();
				presenceNode.append_attribute("type") = "subscribe";
				presenceNode.append_attribute("id") = id.c_str();

				xml_node nickNode = presenceNode.append_child();
				nickNode.set_name("nick");
				nickNode.append_attribute("xmlns")
						= "http://jabber.org/protocol/nick";
				nickNode.append_child(node_pcdata).set_value(fromUser->name.c_str());

				xml_node xNode = presenceNode.append_child();
				xNode.set_name("x");
				xNode.append_attribute("xmlns") = "vcard-temp:x:update";
				xml_node photoNode = xNode.append_child();
				photoNode.set_name("photo");
				xml_node extvalNode = photoNode.append_child();
				extvalNode.set_name("EXTVAL");
				extvalNode.append_child(node_pcdata).set_value(buildHeadUrl(fromUser->headurl).c_str());

				xml_node statusNode = presenceNode.append_child();
				statusNode.set_name("status");
				statusNode.append_child(node_pcdata).set_value(status.c_str());

				ostringstream os;
				doc.print(os);

				MessagePtr msg = new Message();
				msg->from = new Jid();
				msg->from->userId = from;
				msg->from->index = -1;

				msg->to = toJids.at(i);
				msg->msg = os.str();
				msg->type = PRESENCE_COMMON;
				try{
					TalkDeliverAdapter::instance().deliver(msg);
				}catch(Ice::Exception& e){
					MCE_WARN("PresenceHandlerI::presenceSubscribe-->TalkDeliverAdapter::deliver-->" << e);
				}
			}
		}
	}
	TalkUserPtr toUser;
	try{
		toUser = TalkCacheClient::instance().GetUserByIdWithLoad(to);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceSubscribe-->TalkCacheClient::GetUserByIdWithLoad-->" << e);
	}
	for (size_t i = 0; i < fromJids.size(); ++i) {

		xml_document doc;
		xml_node iqNode = doc.append_child();
		iqNode.set_name("iq");
		iqNode.append_attribute("from") = createJid(from).c_str();
		iqNode.append_attribute("to") = jidToString(fromJids.at(i)).c_str();
		iqNode.append_attribute("id") = (int)time(NULL);
		iqNode.append_attribute("type") = "set";

		xml_node queryNode = iqNode.append_child();
		queryNode.set_name("query");
		queryNode.append_attribute("xmlns") = "jabber:iq:roster";

		xml_node itemNode = queryNode.append_child();
		itemNode.set_name("item");
		itemNode.append_attribute("ask") = "subscribe";
		itemNode.append_attribute("subscription") = "none";
		if (toUser) {
			itemNode.append_attribute("name") = toUser->name.c_str();
		}
		itemNode.append_attribute("jid") = createJid(to).c_str();

		ostringstream os;
		doc.print(os);

		MessagePtr msg = new Message();
		msg->from = new Jid();
		msg->from->userId = from;
		//msg->from->endpoint = "";
		msg->from->index = -1;

		msg->to = fromJids.at(i);
		msg->msg = os.str();
		msg->type = IQ_SET;
		try{
			TalkDeliverAdapter::instance().deliver(msg);
		}catch(Ice::Exception& e){
			MCE_WARN("PresenceHandlerI::presenceSubscribe-->TalkDeliverAdapter::deliver-->" << e);
		}
	}
	FunStatManager::instance().Stat("presenceSubscribe", ts.getTime(), (ts.getTime() >= FUNTIMEOUT));
}

void PresenceHandlerI::presenceSubscribed(const string& id, Ice::Int from,
		Ice::Int to, const Ice::Current&) {
	TimeStat ts;
	MCE_INFO("PresenceHandlerI::presenceSubscribed --> id = " << id << " from = " << from  << " to = " << to);
	StatFunc statF("PresenceHandlerI::presenceSubscribed");

	//iq both
	//JidSeq fromJids = PresenceAdapter::instance().getOnlineUserJidSeq(from);
	//JidSeq toJids = PresenceAdapter::instance().getOnlineUserJidSeq(to);
	JidSeq toJids;
	JidSeq fromJids;
	try{
		toJids = OnlineCenterAdapter::instance().getUserJids(to, 4);
	}
	catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceSubscribed-->OnlineCenterAdapter::getUserJids-->" << e);
	}
	try{
		fromJids = OnlineCenterAdapter::instance().getUserJids(from, 4);
	}
	catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceSubscribed-->OnlineCenterAdapter::getUserJids-->" << e);
	}

	TalkUserPtr fromUser;
	TalkUserPtr toUser;
	try{
		fromUser = TalkCacheClient::instance().GetUserByIdWithLoad(from);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceSubscribed-->TalkCacheClient::GetUserByIdWithLoad-->" << e);
	}
	try{
		toUser = TalkCacheClient::instance().GetUserByIdWithLoad(to);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceSubscribed-->TalkCacheClient::GetUserByIdWithLoad-->" << e);
	}
	if(!fromUser || !toUser){
		MCE_INFO("PresenceHandlerI::presenceSubscribed-->cost_time = " << ts.getTime());
		return;
	}
	for (size_t i = 0; i < fromJids.size(); ++i) {

		xml_document doc;
		xml_node iqNode = doc.append_child();
		iqNode.set_name("iq");
		iqNode.append_attribute("from") = createJid(from).c_str();
		iqNode.append_attribute("to") = jidToString(fromJids.at(i)).c_str();
		iqNode.append_attribute("id") = (int)time(NULL);
		iqNode.append_attribute("type") = "set";

		xml_node queryNode = iqNode.append_child();
		queryNode.set_name("query");
		queryNode.append_attribute("xmlns") = "jabber:iq:roster";

		xml_node itemNode = queryNode.append_child();
		itemNode.set_name("item");
		itemNode.append_attribute("subscription") = "both";
		itemNode.append_attribute("name") = toUser->name.c_str();
		itemNode.append_attribute("jid") = createJid(to).c_str();

		ostringstream os;
		doc.print(os);

		MessagePtr mPtr = new Message();
		mPtr->from = new Jid();
		mPtr->from->userId = from;
		mPtr->from->endpoint = "talk.xiaonei.com";
		mPtr->from->index = -1;

		mPtr->to = fromJids.at(i);
		mPtr->type = IQ_SET;
		mPtr->msg = os.str();
		try{
			TalkDeliverAdapter::instance().deliver(mPtr);
		}
		catch(Ice::Exception& e){
			MCE_WARN("PresenceHandlerI::presenceSubscribed-->TalkDeliverAdapter::deliver" << e);
		}
		//			PresenceHandlerI::instance().getProxyServerOneway(fromJids.at(i)->endpoint)->deliver(fromJids.at(i), iqMsg.str());
	}
	for (size_t i = 0; i < toJids.size(); ++i) {

		xml_document doc;
		xml_node presenceNode = doc.append_child();
		presenceNode.set_name("presence");
		presenceNode.append_attribute("from") = createJid(from).c_str();
		presenceNode.append_attribute("to") = jidToString(toJids.at(i)).c_str();
		presenceNode.append_attribute("type") = "subscribed";
		presenceNode.append_attribute("id") = id.c_str();

		xml_node nickNode = presenceNode.append_child();
		nickNode.set_name("nick");
		nickNode.append_attribute("xmlns") = "http://jabber.org/protocol/nick";
		nickNode.append_child(node_pcdata).set_value(fromUser->name.c_str());

		xml_node xNode = presenceNode.append_child();
		xNode.set_name("x");
		xNode.append_attribute("xmlns") = "vcard-temp:x:update";

		xml_node photoNode = xNode.append_child();
		photoNode.set_name("photo");
		xml_node extvalNode = photoNode.append_child();
		extvalNode.set_name("EXTVAL");
		extvalNode.append_child(node_pcdata).set_value(buildHeadUrl(fromUser->headurl).c_str());

		ostringstream os;
		doc.print(os);

		MessagePtr mPtr = new Message();
		mPtr->from = new Jid();
		mPtr->from->userId = from;
		//mPtr->from->endpoint = "talk.xiaonei.com";
		mPtr->from->index = -1;

		mPtr->to = toJids.at(i);
		mPtr->type = PRESENCE_COMMON;
		mPtr->msg = os.str();
		try{
			TalkDeliverAdapter::instance().deliver(mPtr);
		}
		catch(Ice::Exception& e){
			MCE_WARN("PresenceHandlerI::presenceSubscribed-->TalkDeliverAdapter::deliver" << e);
		}

		xml_document doc1;

		xml_node iqNode = doc1.append_child();
		iqNode.set_name("iq");
		iqNode.append_attribute("from") = createJid(to).c_str();
		iqNode.append_attribute("to") = jidToString(toJids.at(i)).c_str();
		iqNode.append_attribute("id") = (int)time(NULL);
		iqNode.append_attribute("type") = "set";

		xml_node queryNode = iqNode.append_child();
		queryNode.set_name("query");
		queryNode.append_attribute("xmlns") = "jabber:iq:roster";

		xml_node itemNode = queryNode.append_child();
		itemNode.set_name("item");
		itemNode.append_attribute("subscription") = "both";
		itemNode.append_attribute("name") = fromUser->name.c_str();
		itemNode.append_attribute("jid") = createJid(from).c_str();

		ostringstream os1;
		doc1.print(os1);

		MessagePtr mPtr1 = new Message();
		mPtr1->from = new Jid();
		mPtr1->from->userId = to;
		mPtr1->from->endpoint = "";
		mPtr1->from->index = -1;

		mPtr1->to = toJids.at(i);
		mPtr1->type = IQ_SET;
		mPtr1->msg = os1.str();
		try{
			TalkDeliverAdapter::instance().deliver(mPtr1);
		}catch(Ice::Exception& e){
			MCE_WARN("PresenceHandlerI::presenceSubscribed-->TalkDeliverAdapter::deliver" << e);
		}

	}

	for (size_t i = 0; i < fromJids.size(); ++i) {
		for (size_t j = 0; j < toJids.size(); ++j) {
			ostringstream pMsg;
			pMsg<<"<presence from='"<<jidToString(fromJids.at(i)) <<"' to='"
					<<jidToString(toJids.at(j))<<"' id='" <<time(NULL)<<"'>"
					<<"<priority>50</priority>" <<"</presence>";

			MessagePtr mPtr = new Message();
			mPtr->from = fromJids.at(i);
			mPtr->to = toJids.at(j);
			mPtr->type = PRESENCE_COMMON;
			mPtr->msg = pMsg.str();
			try{
				TalkDeliverAdapter::instance().deliver(mPtr);
			}
			catch(Ice::Exception& e){
				MCE_WARN("PresenceHandlerI::presenceSubscribed-->TalkDeliverAdapter::deliver" << e);
			}


			pMsg.str("");
			pMsg<<"<presence to='"<<jidToString(fromJids.at(i)) <<"' from='"
					<<jidToString(toJids.at(j))<<"' id='" <<time(NULL)<<"'>"
					<<"<priority>50</priority>" <<"</presence>";

			mPtr = new Message();
			mPtr->to = fromJids.at(i);
			mPtr->from = toJids.at(j);
			mPtr->type = PRESENCE_COMMON;
			mPtr->msg = pMsg.str();
			try{
				TalkDeliverAdapter::instance().deliver(mPtr);
			}
			catch(Ice::Exception& e){
				MCE_WARN("PresenceHandlerI::presenceSubscribed-->TalkDeliverAdapter::deliver" << e);
			}


		}
	}
	FunStatManager::instance().Stat("presenceSubscribed", ts.getTime(), (ts.getTime() >= FUNTIMEOUT));
}

void PresenceHandlerI::presenceUnsubscribed(const string& id, Ice::Int from,
		Ice::Int to, const Ice::Current&) {
	StatFunc statF("PresenceHandlerI::presenceUnsubscribed");
	TimeStat ts;
	//JidSeq toJids = PresenceAdapter::instance().getOnlineUserJidSeq(to);
	JidSeq toJids;
	try{
		toJids = OnlineCenterAdapter::instance().getUserJids(to, 4);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presenceUnsubscribed-->OnlineCenterAdapter::getUserJids-->" << e);
	}
	for (size_t i = 0; i < toJids.size(); ++i) {
		ostringstream msg;
		msg <<"<presence from='"<<from<<"@talk.xiaonei.com' to='"
				<<jidToString(toJids.at(i))<<"' type='unsubscribe'/>";

		MessagePtr mPtr = new Message();
		mPtr->from = new Jid();
		mPtr->from->userId = from;
		mPtr->from->endpoint = "talk.xiaonei.com";
		mPtr->from->index = -1;

		mPtr->to = toJids.at(i);
		mPtr->type = PRESENCE_COMMON;
		mPtr->msg = msg.str();
		try{
			TalkDeliverAdapter::instance().deliver(mPtr);
		}
		catch(Ice::Exception& e){
			MCE_WARN("PresenceHandlerI::presenceUnsubscribed-->TalkDeliverAdapter::deliver-->" << e);
		}

		msg.str("");
		msg <<"<iq from='"<<to<<"@talk.xiaonei.com' to='"
				<<jidToString(toJids.at(i))<<"' id='"<<time(NULL)
				<<"' type='set'>" <<"<query xmlns='jabber:iq:roster'>"
				<<"<item subscription='remove' jid='"<<from
				<<"@talk.xiaonei.com'/>" <<"</query></iq>";

		mPtr = new Message();
		mPtr->from = new Jid();
		mPtr->from->userId = to;
		mPtr->from->endpoint = "";
		mPtr->from->index = -1;

		mPtr->to = toJids.at(i);
		mPtr->type = IQ_SET;
		mPtr->msg = msg.str();
		try{
			TalkDeliverAdapter::instance().deliver(mPtr);
		}
		catch(Ice::Exception& e){
			MCE_WARN("PresenceHandlerI::presenceUnsubscribed-->TalkDeliverAdapter::deliver-->" << e);
		}
	}
	FunStatManager::instance().Stat("presenceUnsubscribed", ts.getTime(), (ts.getTime() >= FUNTIMEOUT));
	/*
	 }*/
}

void PresenceHandlerI::message(const JidPtr& jid, const string& msg,
		const Ice::Current&) {
	//StatFunc statF("PresenceHandlerI::message");
	TimeStat ts;
	MCE_DEBUG("PresenceHandlerI::message --> from:"<<jidToString(jid)<<"  msg:"
			<<msg);
	if (jid->userId == 128487631) {
		MCE_DEBUG("LogicManager::message --> userid:128487631   msg:"<<msg);
	}

	MessageAnalyst::instance().handle(jid, msg);
	FunStatManager::instance().Stat("message", ts.getTime(), (ts.getTime() >= FUNTIMEOUT));
}

void PresenceHandlerI::offlineNotify(const JidSeq& seq, const Ice::Current&) {

	//NotifyTaskManager::instance().addNotify(new Notify<JidSeq,OfflineNotifyHandler>(seq));
	StatFunc statF("PresenceHandlerI::offlineNotify");
	TimeStat ts;
	TaskManager::instance().execute(new BatchOfflineNotifyTask(seq));

/*	JidSeq jSeq;
	for (size_t i = 0; i < seq.size(); ++i) {
		if (seq.at(i)->endpoint != "" && seq.at(i)->endpoint[0] == 'T') {
			jSeq.push_back(seq.at(i));
		}
	}
	if (!jSeq.empty()) {
		TaskManager::instance().execute(new ExitAllRoomTask(jSeq));
	}
*/
/*	try {
		OnlineStatSyncAdapter::instance().offline(seq);
	} catch(Ice::Exception& e) {
		MCE_WARN("PresenceHandlerI::offlineNotify --> offlineBatch err:"<<e);
	}*/
	FunStatManager::instance().Stat("offlineNotify", 0, 0);
}

void PresenceHandlerI::webPresence(Ice::Int userId, const Ice::Current& current) {
	StatFunc statF("PresenceHandlerI::webPresence");
	TimeStat ts;
	MCE_DEBUG("call PresenceHandlerI::webPresence");

	//TalkCacheAdapter::instance().updateUser(userId);

	Ice::Context::const_iterator it = current.ctx.find("source");
	if (it != current.ctx.end()) {
		if (it->second == "xntalk") {
			return;
		}
	}
	try{
		JidSeq seq = OnlineCenterAdapter::instance().getUserJids(userId);
		if(seq.empty()){
			return;
		}
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::webPresence-->OnlineCenterAdapter::getUserJids-->" << e);
	}

	JidPtr jid = new Jid();
	jid->userId = userId;
	jid->endpoint = "web";
	jid->index = 1;
	// not complate
	//PresenceAdapter::instance().bind(jid);
	try{
		OnlineCenterAdapter::instance().presence(jid,onlineStat(""),"","", Str2StrMap());
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::webPresence-->OnlineCenterAdapter::presence" << e);
	}
	FunStatManager::instance().Stat("webPresence", ts.getTime(), (ts.getTime() >= FUNTIMEOUT));
	/*try {
		OnlineStatDispatchAdapter::instance().online(jid);
	} catch(Ice::Exception& e) {
		MCE_WARN("PresenceHandlerI::webPresence --> online err:"<<e);
	}*/
/*
	UserPtr u = TalkCacheAdapter::instance().getUser(userId,1000);
	if (!u) {
		MCE_DEBUG("PresenceHandlerI::webPresence --> getUser null userid:"
				<<userId);
		return;
	}

	xml_document_ptr doc = new xml_document();
	xml_node presenceNode = doc->append_child();
	presenceNode.set_name("presence");
	presenceNode.append_attribute("from") = jidToString(jid).c_str();

	if (u->status != "") {
		xml_node statNode = presenceNode.append_child();
		statNode.set_name("status");
		statNode.append_child(node_pcdata).set_value(u->status.c_str());
	}
	if (u->name != "") {
		xml_node nickNode = presenceNode.append_child();
		nickNode.set_name("nick");
		nickNode.append_attribute("xmlns") = "http://jabber.org/protocol/nick";
		nickNode.append_child(node_pcdata).set_value(u->name.c_str());
	}
	if (u->headurl != "") {
		xml_node xNode = presenceNode.append_child();
		xNode.set_name("x");
		xNode.append_attribute("xmlns") = "vcard-temp:x:update";

		xml_node photoNode = xNode.append_child();
		photoNode.set_name("photo");

		xml_node extvalNode = photoNode.append_child();
		extvalNode.set_name("EXTVAL");
		extvalNode.append_child(node_pcdata).set_value(buildHeadUrl(u->headurl).c_str());

	}*/
	//TaskManager::instance().execute(new PresenceNotifyTask(jid,PRESENCE_COMMON,doc));

}

void PresenceHandlerI::nodifyBuddiesOnlineStat(const AvatarSeq& activeAvatars,
		const AvatarSeq& selfOtherAvatars, const AvatarMap& buddies, NotifyType type, const MyUtil::Str2StrMap& paras, const Ice::Current&){
	//StatFunc statF("PresenceHandlerI::nodifyBuddiesOfOnlineStat");
	MCE_DEBUG("PresenceHandlerI::nodifyBuddiesOfOnlineStat --> active size="
			<< activeAvatars.size() <<" self other size=" << selfOtherAvatars.size() << " buddies=" << buddies.size());

	if(activeAvatars.empty() || (selfOtherAvatars.empty()&&buddies.empty())){
		MCE_DEBUG("PresenceHandlerI::nodifyBuddiesOfOnlineStat --> need not notify");
		return;
	}
	TaskManager::instance().schedule(new PresenceNotifyTask(activeAvatars, selfOtherAvatars, buddies, type, paras));
	FunStatManager::instance().Stat("nodifyBuddiesOnlineStat", 0, 0);
	/*
	if(activeAvatars.at(0)->stat == OFFLINE){
		JidSeq seq;
		for(size_t i=0; i<activeAvatars.size(); i++){
			if (!activeAvatars.at(i)->jid->endpoint.empty() && activeAvatars.at(i)->jid->endpoint[0] == 'T') {
				seq.push_back(activeAvatars.at(i)->jid);
			}
		}
		try{
			if(!seq.empty()){
				MCE_INFO("****** mucOfflineNotify --> " << seq.size());
				MucPresenceAdapter::instance().mucOfflineNotify(seq);
			}
		}catch(Ice::Exception& e){
			MCE_WARN("mucOfflineNotify --> " << e);
		}
	}*/
	/*	if(activeAvatars.at(0)->stat == OFFLINE){
		JidSeq seq;
		for(size_t i=0; i<activeAvatars.size(); i++){
			if (!activeAvatars.at(i)->jid->endpoint.empty() && activeAvatars.at(i)->jid->endpoint[0] == 'T') {
				seq.push_back(activeAvatars.at(i)->jid);
			}
		}
		if (!seq.empty()) {
			TaskManager::instance().execute(new ExitAllRoomTask(seq));
		}
	}*/
}

void PresenceHandlerI::mucOfflineNotify(const JidSeq& jids, const Ice::Current&){
	TaskManager::instance().execute(new ExitAllRoomTask(jids));
}

void PresenceHandlerI::offline(const JidPtr& jid, const Ice::Current&){
	TimeStat ts;
	if(jid){
		MCE_INFO("~~PresenceHandlerI::offline --> " << jidToString(jid));
	}

	try{
		OnlineCenterAdapter::instance().offline(jid);
		//PresenceAdapter::instance().unbind(jid);
		//OnlineEventMediatorAdapter::instance().sendOnlineEvent(jid->userId,0);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::mucofflineNotify-->OnlineCenterAdapter::offline-->" << jidToString(jid) << "err" << e);
	}

	try{
		MucGateAdapter::instance().UserOffline(jid);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::offline-->MucGateAdapter::UserOffline-->" << jidToString(jid) << "err" << e);
	}
	try{
		PresenceAdapter::instance().unbind(jid);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::offline-->PresenceAdapter::unbind-->" << jidToString(jid) << "err" << e);
	}

	try{
		OnlineEventMediatorAdapter::instance().sendOnlineEvent(jid->userId,0);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::offline-->OnlineEventMediatorAdapter::sendOnlineEvent-->" << jidToString(jid) << "error" << e);
	}
	FunStatManager::instance().Stat("offline", ts.getTime(), (ts.getTime() >= FUNTIMEOUT));
}

void PresenceHandlerI::userStatusChanged(int userId, int userStatus, const Ice::Current&){
	TimeStat ts;
	if(userStatus == 0){
		return;
	}
	MCE_INFO("PresenceHandlerI::userStatusChanged --> " << userId << " " << userStatus);
	KickUserTimer::instance().push(userId);	
	FunStatManager::instance().Stat("userStatusChanged", ts.getTime(), (ts.getTime() >= FUNTIMEOUT));
	/*JidSeq jids;
	try{
		MCE_INFO("PresenceHandlerI::userStatusChanged -- > " << userId << " " << userStatus);
		jids = OnlineCenterAdapter::instance().getUserJids(userId);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::kick -- > get jids err, " << e);
	}
	if(jids.empty()){
		return;
	}
	MCE_INFO("PresenceHandlerI::userStatusChanged -- > kick " << userId << " " << userStatus);
	for(int i=0; i<jids.size(); i++){
		try{
			MCE_INFO("PresenceHandlerI::kick -- > offline " << jidToString(jids.at(i)));
			OnlineCenterAdapter::instance().offline(jids.at(i));
		}catch(Ice::Exception& e){
			MCE_WARN("PresenceHandlerI::kick -- > invoke offline err, " << e << " " << jidToString(jids.at(i)));
		}
		try{
			MCE_INFO("PresenceHandlerI::kick -- > close " << jidToString(jids.at(i)));
			TalkProxyAdapter::instance().kick(jids.at(i));
		}catch(Ice::Exception& e){
			MCE_WARN("PresenceHandlerI::kick -- > invoke talkproxy kick err, " << e << " " << jidToString(jids.at(i)));
		}
	}*/
}
