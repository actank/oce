#include "WTransitI.h"
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include "MessageType.h"
#include "TalkDeliverAdapter.h"
#include "XmppTools.h"
#include "TalkUtil/src/pugixml.hpp"
#include "QueryRunner.h"
#include "OnlineCenterAdapter.h"
#include "IceLogger.h"
//#include "NotifyRplAdapter.h"
#include <boost/lexical_cast.hpp>
#include <util/cpp/String.h>
#include "FeedCacheNAdapter.h"
#include "FeedContentAdapter.h"
#include "UgcCommentAdapter.h"
#include "XmppTools.h"
#include "NotifyGateAdapter.h"
#include "TalkProxyAdapter.h"
#include "FeedMemcContentAdapter.h"
#include "FeedMemcLoaderAdapter.h"
#include "MucOnlineCenterAdapter.h"
#include <iostream>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
//#include "client/linux/handler/exception_handler.h"
//#include "FeedInteractionAdapter.h"

//using namespace com::xiaonei::wService::slice;
using namespace com::xiaonei::talk::muc;
using namespace xce::ucgcomment;
using namespace xce::notify;
using namespace talk::wtransit;
using namespace pugi;
using namespace MyUtil;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk::adapter;
using namespace ::xce::feed;
using namespace ::com::xiaonei::xce;
//using namespace mop::hi::svc::dispatch;
using namespace talk::online::adapter;

//bool MyCallback(const char *dump_path, const char *minidump_id, void *context, bool succeeded) {
//  printf("got crash %s %s\n", dump_path, minidump_id);
//  return true;
//}

void MyUtil::initialize() {
  //static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
	ServiceI::instance().getAdapter()->add(&WTransitManagerI::instance(),
			ServiceI::instance().createIdentity("M", ""));
	PipePool::instance().initialize(new PipeFactoryI);
	TaskManager::instance().config(TASK_LEVEL_QUERY, ThreadPoolConfig(50, 100));
	TaskManager::instance().config(TASK_LEVEL_CREATE, ThreadPoolConfig(50, 100));
	TaskManager::instance().config(TASK_LEVEL_NORMAL, ThreadPoolConfig(50, 100));
	TaskManager::instance().config(2011, ThreadPoolConfig(100, 300));
	TaskManager::instance().scheduleRepeated(new FeedPipe());
	TaskManager::instance().scheduleRepeated(new FeedReplyPipe());
	string configFile = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault( "Service." + ServiceI::instance().getName() + ".ReplyConfigFile","/data/xce/IMTest/ReplyView.xml");
	InitMonitorLogger(ServiceI::instance().getName(), "monitor", "../log/" + ServiceI::instance().getName() + "/monitor/monitor_log", "INFO");


/*
	ServiceI& service = ServiceI::instance();
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Service.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Service.Interval", 5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerWTransitForIq", &WTransitManagerI::instance(), mod, interval, new TalkChannel());
	MCE_INFO("MyUtil::initialize. mod:" << mod << " interval:" << interval);
*/

}




void WTransitManagerI::SwapFeedSeq(FeedWithTargetSeq& seq){
 IceUtil::Mutex::Lock lock(_seqmutex);
 seq.swap(_feedseq);
}
void WTransitManagerI::SwapFeedReplySeq(FeedReplyWithTargetSeq& seq){
 IceUtil::Mutex::Lock lock(_seqmutex);
 seq.swap(_feedreplyseq);
}

string WTransitManagerI::GetServerProperties(const string& key){
	IceUtil::Mutex::Lock lock(_promutex);
	map<string, string>::iterator it = _properties.find(key);
	if(it != _properties.end()){
		return it->second;
	}else{
		string val = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault(key, "");
		if(!val.empty())
			_properties.insert(make_pair<string, string>(key, val));
		return val;
	}
}

FeedContentDict WTransitManagerI::GetFeedContents(const MyUtil::LongSeq& feedids){
	int count = 1;
	FeedContentDict ans;
	LongSeq tmpseq;
	for(LongSeq::const_iterator it = feedids.begin(); ; ++it){
		if(it != feedids.end()){
			tmpseq.push_back((*it));
		}
		if((count == 50 || it == feedids.end()) && (!tmpseq.empty())){
			count = 0;
			FeedContentDict tmpans;
			try{
				tmpans = FeedContentAdapter::instance().getFeedDict(tmpseq);
			}catch(Ice::Exception& e){
				MCE_WARN("WTransitManagerI::GetFeedContets-->FeedContentAdapter::FeedContent-->error:" << e);
			}
			if(!tmpans.empty()){
				ans.insert(tmpans.begin(), tmpans.end());
			}
			tmpseq.clear();
		}
		if(it == feedids.end()){
			break;
		}
		++count;
	}
	return ans;
}




////////////////////// LoadFeedTask //////////////////////////////////////////


void LoadFeedTask::handle(){
	TimeStat ts;
	MessageSeq mseqs;
	mseqs.push_back(0);
	//MCE_DEBUG("LoadFeedTask::handle --> userid:" << _jid->userId << " feedid:" << _feedIndex << " replyid:" << _replyIndex);
	Ice::Long maxFeedId = _feedIndex;
	float timeuse_getFeedDict = 0.0;
	FeedItemSeq feeds;
	TimeStat tsget;
	float timeuse_get;
	try {
		feeds = FeedCacheNAdapter::instance().get(_jid->userId);
		timeuse_get = tsget.getTime();
	} catch (Ice::Exception& e) {
		MCE_WARN("LoadFeedTask::handle-->FeedCacheAdapter::get-->"<<_jid->userId<<":"<<e);
	}
	if(!feeds.empty()){
		MyUtil::LongSeq feedIds;
		for( int h=feeds.size() - 1; h>= 0; --h ){
			if(!WTransitManagerI::instance().isDeliver2Client(feeds.at(h).type&0xFFFF)){
				continue;
			}
			//只取100条
			//if(h > 100){
			//	break;
			//}
			if(maxFeedId < WTransitManagerI::baseFeedId){
				//旧版feedid
				Ice::Long id = 6000000000000000000 + ((Ice::Long)feeds.at(h).time)*100000 + ((feeds.at(h).feed & 0xffffffff) % 100000);
				if(id > maxFeedId){
					feedIds.push_back(feeds.at(h).feed);
				}
				//else{
				//	break;
				//}
			}else{
				//新版feedid
				Ice::Long id = maxFeedId - WTransitManagerI::baseFeedId;
				if(feeds.at(h).feed > id){
					feedIds.push_back(feeds.at(h).feed);
				}
				//else{
				//	break;
				//}
			}
		}
		if(!feeds.empty()){
			//maxFeedId = 6000000000000000000 + ((Ice::Long)feeds.at(0).time)*100000 + ((feeds.at(0).feed & 0xffffffff) % 100000);
			maxFeedId = WTransitManagerI::baseFeedId + feeds.at(0).feed;
			//TODO
		}
		FeedContentDict contents;
		TimeStat contentts;

		if(!feedIds.empty()){
			TimeStat tsgetFeedDict;
			TimeStat tsLoadMissFeedContent;
			try {
				contents = WTransitManagerI::instance().GetFeedContents(feedIds);
				timeuse_getFeedDict = tsgetFeedDict.getTime();
				WTransitManagerI::instance().LoadMissFeedContent(contents, feedIds);
			} catch (Ice::Exception& e) {
				MCE_WARN("LoadFeedTask::handle-->FeedContentAdapter::getFeedDict-->:"<<e);
			}
			MCE_INFO("LoadFeedTask::handle --> get content actor=" << _jid->userId << " feedIds.size=" << feedIds.size() << " contents.size="<<contents.size());
			if (!contents.empty()) {
				FeedToMessagConverter converter;
				for (FeedContentDict::iterator it = contents.begin(); it != contents.end(); ++it) {
					if(it->second){
						converter.setData(it->second);
						string feedxml = it->second->data->xml;
						int feedxml_size = feedxml.size();
						MCE_DEBUG("LoadFeedTask::handle --> feedxml = " << feedxml << " size = " << feedxml_size);
						MessagePtr msg = converter.getToTalkMessage(_jid, true);
						if( msg ){
							mseqs.push_back(msg);
						}
					} else{
						MCE_DEBUG("LoadFeedTask::handle --> can not get feed=" << it->first);
					}
				}
			}
		}
	}
	static const Ice::Long maxNoteId = 2000000000;
	static const int updateTime = 1270662628 + 300;

		TimeStat tsgetNotifySeq2;
		float timeuse_getNotifySeq2;
		Ice::Long maxReplyId = _replyIndex;
		StrSeq notifySeq;
	
		try{
			notifySeq = NotifyGateAdapter::instance().getNotifySeq2(_jid->userId, 5, 0, 200);
		}catch(Ice::Exception& e){
			MCE_WARN("LoadFeedTask::handle-->NotifyGateAdapter::getNotifySeq2-->" << _jid->userId << " error:" << e);
		}
		timeuse_getNotifySeq2 = tsgetNotifySeq2.getTime();
		MCE_INFO("LoadFeedTask::handle--> NotifyGateAdapter::getNotifySeq2--> jid:" << _jid->userId << " notifySeq.size:" << notifySeq.size())
		
		int countFromNew = 0;
		int fifteendays = 15 * 24 * 60 * 60;
		time_t now = time(NULL);
		//int countFromOld = noteSeq1.size();
		for (int i = notifySeq.size() - 1; i >= 0; --i) {
			xml_document doc;
			if(!doc.load(notifySeq.at(i).c_str())){
				continue;
			}
			time_t time = 0;
			try{
				time = boost::lexical_cast<int>(doc.child("message").child("xfeed").child("time").first_child().value());
			}catch(...){
				MCE_WARN("LoadFeedTask::handle --> cast time err, time:" << doc.child("message").child("xfeed").child("time").first_child().value() << " data:" << doc.xml());
			}
			if(now - time > fifteendays){ //产品要求 : 超过15天的提醒不推送
				break;
			}
			if(time < updateTime){
				continue;
			}
			Ice::Long replyId = 0;
			try{
				replyId = boost::lexical_cast<Ice::Long>(doc.child("message").child("xfeed").child("id").first_child().value());
			}catch(...){
				MCE_WARN("LoadFeedTask::handle --> cast replyid err, " << doc.child("message").child("xfeed").child("id").first_child().value());
			}
			if((-replyId) <= _replyIndex){
				break;
			}
			if(i==0){
				maxReplyId = -replyId;
			}
			/*if((-replyId) <= (_replyIndex - WTransitManagerI::baseReplyId)){
				break;
			}
			Ice::Long id = -(-replyId + WTransitManagerI::baseReplyId);
			doc.child("message").child("xfeed").remove_child("id");
			xml_node tmp = doc.child("message").child("xfeed").append_child();
			tmp.set_name("id");
			tmp.append_child(node_pcdata).set_value(boost::lexical_cast<string>(id).c_str());
			*/
			countFromNew++;
			MessagePtr msg = new Message();
			JidPtr jfrom = new Jid();
			jfrom->userId = 0;
			jfrom->endpoint = "feed.talk.xiaonei.com";
			jfrom->index = -1;

			doc.child("message").append_attribute("to") = jidToString(_jid).c_str();
			msg->from = jfrom;
			msg->to = _jid;
			msg->type = NOTIFY2_MESSAGE;
			msg->msg = doc.xml();
			mseqs.push_back(msg);
		}

	//build iq msg
	MessagePtr iqMsg = new Message();
	iqMsg->from = _jid;
	iqMsg->to = _jid;
	iqMsg->type = IQ_RESULT;

	ostringstream msg;
	msg << "<iq type='result' id='" << _iqId << "'>"
		<< "<query xmlns='http://talk.xiaonei.com/xfeed'>"
		<< "<xfeed end='" << maxFeedId << "'/>"
		<< "<reply end='" << maxReplyId << "'/>" << "</query>" << "</iq>";

	iqMsg->msg = msg.str();
	mseqs.at(0) = iqMsg;
	if (!mseqs.empty()) {
		try {
			TalkDeliverAdapter::instance().deliver(mseqs);
		} catch (Ice::Exception& e) {
			MCE_WARN("LoadFeedTask::handle-->TalkDeliverAdapter::deliver-->"<< e);
		}
	}
	MONITOR("SignForTimeuse::WTransit::LoadFeedTask::handle-->" << ts.getTime() << "@FeedCacheAdapter::get-->" << timeuse_get << "@FeedContentAdapter::getFeedDict-->" << timeuse_getFeedDict << "@NotifyGateAdapter::getNotifySeq2-->" << timeuse_getNotifySeq2 );
	FunStatManager::instance().Stat("LoadFeedTask::handle", ts.getTime(), (ts.getTime() > 400.0));
	FunStatManager::instance().Stat("FeedCacheAdapter::get", timeuse_get, (timeuse_get > 250.0));
	FunStatManager::instance().Stat("FeedContentAdapter::getFeedDict", timeuse_getFeedDict , (timeuse_getFeedDict > 250.0));
	FunStatManager::instance().Stat("NotifyGateAdapter::getNotifySeq2", timeuse_getNotifySeq2, (timeuse_getNotifySeq2> 250.0));
}





/*void LoadFeedTask::loadReply(const NoteSeq& seq) {
	MessageSeq msgs;
	for (size_t x = 0; x < seq.size(); ++x) {
		NotePtr note = seq.at(x);

		if (!note->unread || note->noteId < _replyIndex) {
			continue;
		}
		ostringstream endpoint;
		endpoint << note->type;
		JidPtr jfrom = new Jid;
		jfrom = new Jid();
		jfrom->userId = note->type;
		jfrom->endpoint = "feed.talk.xiaonei.com";
		jfrom->index = -1;

		ostringstream os;
		os << "<message type='common' from='" << note->type
		   << "@feed.talk.xiaonei.com'>" << "<subject>reply</subject>"
		   << "<body/>" << "<xfeed xmlns='http://talk.xiaonei.com/xfeed'>"
		   << "<id>" << 0 - note->noteId << "</id>" << "<actor>"
		   << note->fromId << "</actor>" << "<time>" << note->timestamp
		   << "</time>" << "<type>0</type>" << "<typen/>"
		   << "<stype>" << 0
			- note->type << "</stype>" << "<stypen/>"
		   << "<resource>"
		   << note->resource << "</resource>" << "<f>" << "<fromid>"
		   << note->fromId << "</fromid>" << "<fromname>" << xmlEncode(
			   note->fromName) << "</fromname>" << "<title>" << xmlEncode(
				   note->title) << "</title>" << "<link>" << xmlEncode(note->link)
		   << "</link>" << "</f>" << ReplyPipe::getTemplate(note)
		   << "</xfeed></message>";

		xml_document doc;
		//MCE_DEBUG("from name:"<<note->fromName<< "  toid:"<<note->toId << " msg:"<<os.str());
		if (!doc.load(os.str().c_str())) {
			continue;
		}
		doc.child("message").append_attribute("to") = jidToString(_jid).c_str();
		MessagePtr msg = new Message();
		msg->from = jfrom;
		msg->to = _jid;
		msg->type = SYSTEM_MESSAGE;
		msg->msg = doc.xml();
		msgs.push_back(msg);
	}

	if (!msgs.empty()) {
		MCE_DEBUG("LoadFeedTask::loadReply --> msg.size:"<<msgs.size()<<"   msg:"
				<<msgs.at(0)->msg);
		TalkDeliverAdapter::instance().deliver(msgs);
	}

}*/


//////////////////////////////////////////////////
//仅推送客户端支持的新鲜事类型。
/*
101        分享好友
102        分享日志
103        分享照片
104        分享相册
106        分享论坛群主题
107        分享链接
110        分享视频
111        分享音乐
502        发表/转发状态
601        发表日志
701        上传单张照片
702        照片被圈
709        上传多张照片
2301        发起投票  kill
2303        参与投票  kill
2401        安装应用 
*/
bool WTransitManagerI::isDeliver2Client(int stype){
	int small = stype & 0xFFFF;
	if(101 == small || 102 == small || 103 == small || 104 == small || 106 == small || 107 == small || 110 == small || 111 == small || 502 == small || 601 == small || 701 == small || 702 == small || 709 == small ||  2401 == small){
		return true;
	}
	return false;
}

void WTransitManagerI::PrintLog(const string& method, Ice::Long objid, const MyUtil::IntSeq& target){
	ostringstream os;
	os << method << "-->id:" << objid << " to:";
	for(IntSeq::const_iterator it = target.begin(); it != target.end(); ++it){
		os << (*it) << ",";
	}
	MCE_INFO(os.str());
}

void WTransitManagerI::deliverFeed(const FeedSeedPtr& feed, const MyUtil::IntSeq& target, const Ice::Current& ){
	/*try{
    FeedInteractionAdapter::instance().AddFeedInteraction(feed->feed, feed->actor, target.size(), feed->time); 
  }catch(Ice::Exception& e){
    MCE_WARN("WTransitManagerI::deliverFeed --> call FeedInteraction err, " << e);
  }*/
  TimeStat ts;
  if(!isDeliver2Client(feed->type) || target.empty()){
    return;
  }
	if(feed->xml.empty()){
		return;
	}
	FeedWithTargetPtr ft = new FeedWithTarget(feed,target);
	{
		IceUtil::Mutex::Lock lock(_seqmutex);
		_feedseq.push_back(ft);
	}
	//PrintLog("WTransitManagerI::deliverFeed", feed->feed, target);
	FunStatManager::instance().Stat("WTransitManagerI::deliverFeed", ts.getTime(), false);
}

/*
void WTransitManagerI::deliverReply(const NotifyContentPtr& content, const Ice::Current& ){
	MCE_DEBUG("call WTransitManagerI::deliverReply");
	PipePool::instance().push("Reply", 0, content);
}*/
void WTransitManagerI::loadOfflineFeed(
	const JidPtr& jid, Ice::Long feedIndex, Ice::Long replyIndex,Ice::Int iqId, const Ice::Current& current){
	MCE_INFO("WTransitManagerI::loadOfflineFeed-->" << current.con->toString());
  TimeStat ts;
	if(!jid){
		return;
	}
  TaskManager::instance().execute(new LoadFeedTask(jid, feedIndex, replyIndex, iqId));
}


void WTransitManagerI::deliverAppNotice(
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
	MCE_DEBUG("WEventManagerI::deliverAppNotice --> "<< "  to:"<< toid
			<<"  msg:"<<doc.xml() );
	PipePool::instance().push("AppNotice", 0, m);

	 // 产品要求先去掉
	 /*string pager_body = "<img class=\"notifyico\" src=\"";
	 pager_body += appIcon;
	 pager_body += "\"/><div class=\"notifybody\">";
	 pager_body += msg;
	 pager_body += "</div>";

	 //pager_body html encoding
	 //MyUtil::StrUtil::string_replace(pager_body, "&", "&amp;");
	 //MyUtil::StrUtil::string_replace(page
	
	 deliverNotify(toid, appName, pager_body);*/
  //MCE_DEBUG("interface WEventManagerI::deliverAppNotice usetime = " << ts.getTime());
}

/*void WTransitManagerI::deliverNotify(Ice::Int userid, const string& title,
			const string& msg, const Ice::Current&){
	MCE_DEBUG("WTransitManagerI::deliverNotify");

	MyUtil::StrUtil::string_replace(const_cast<string&> (title), "&", "&amp;");
	MyUtil::StrUtil::string_replace(const_cast<string&> (msg), "&", "&amp;");

	xml_document doc;
	xml_node messageNode = doc.append_child();
	messageNode.set_name("message");
	messageNode.append_attribute("type") = "headline";
	messageNode.append_attribute("from") = "notify@news.talk.xiaonei.com";

	xml_node subjectNode = messageNode.append_child();
	subjectNode.set_name("subject");
	subjectNode.append_child(node_pcdata).set_value(title.c_str());

	xml_node bodyNode = messageNode.append_child();
	bodyNode.set_name("body");
	bodyNode.append_child(node_pcdata).set_value(msg.c_str());

	MessagePtr m = new Message();
	m->to = new Jid();
	m->to->userId = userid;
	m->msg = doc.xml();
	m->type = NOTIFY_MESSAGE;

	PipePool::instance().push("Notify", 0, m);
}*/



void WTransitManagerI::deliverFeedReply( const ReplyDataPtr& replyData, const MyUtil::IntSeq& target, const Ice::Current& ){
  TimeStat ts;
  if((replyData->stype>=2000 && replyData->stype<2100)){
	//过滤pager产生的回复
		return;
	}
	if(replyData->actor == 257876975 || replyData->actor == 238489851 || replyData->actor == 232626782){
		//MCE_DEBUG("call WTransitManagerI::deliverFeedReply actor="<<replyData->actor<<"  source="<<replyData->source<<" stype="<<replyData->stype<<" " << target.size());
	}
	FeedReplyWithTargetPtr ft = new FeedReplyWithTarget(replyData,target);
	{
		IceUtil::Mutex::Lock lock(_seqmutex);
		_feedreplyseq.push_back(ft);
	}
	//PrintLog("WTransitManagerI::deliverFeedReply", replyData->replyId, target);
	FunStatManager::instance().Stat("WTransitManagerI::deliverFeedReply", ts.getTime(), false);
}

void WTransitManagerI::loadOfflineFeedReply(const JidPtr& jid, int stype, int actor, Ice::Long source, const string& iqId, const Ice::Current&){
  TimeStat ts;
	TaskManager::instance().execute( new loadOfflineFeedReplyTask(jid, stype, actor, source, iqId));
}


// By Guoqing.liu get ContentDict from FeedContentAdapter, miss ids get from FeedMemcLoader 
void WTransitManagerI::LoadMissFeedContent(FeedContentDict& contents, MyUtil::LongSeq& feedids){
	TimeStat ts;
	if(contents.size() == feedids.size()){
		return;
	}
	MyUtil::LongSeq missids;
	FeedContentDict::iterator fit;
	for(MyUtil::LongSeq::iterator idit = feedids.begin(); idit != feedids.end(); ++idit){
		Ice::Long fid = (*idit);
		fit = contents.find(fid);
		if(fit == contents.end()){
			missids.push_back(fid);
		}
	}
	TimeStat tsGetFeedDict;
	float timeuse_GetFeedDict;
	if(!missids.empty()){
		FeedContentDict tmp;
		try{
			tmp = FeedMemcLoaderAdapter::instance().GetFeedDict(missids, 300);
			timeuse_GetFeedDict = tsGetFeedDict.getTime();
			if(!tmp.empty()){
				contents.insert(tmp.begin(), tmp.end());
			}
			//MCE_DEBUG("WTransitManagerI::LoadMissFeedContent --> timestat & usertime:" << ts.getTime() << " feedids:" << feedids.size() << " missids:" << missids.size() << " tmp:" <<tmp.size() << " contents:" << contents.size());
		}catch(Ice::Exception& e){
			MCE_WARN("WTransitManagerI::LoadMissFeedContent --> FeedMemcLoaderAdapter::GetFeedDict--> missids:" << missids.size() << " error:" << e);
		}
		
		/*
		try{
			FeedMemcLoaderAdapter::instance().LoadFeedSeq(missids);
		}catch(Ice::Exception& e){
			MCE_WARN("WTransitManagerI::LoadMissFeedContent --> FeedMemcLoaderAdapter::GetFeedDict --> missids: " << missids.size() << " error :" << e);
		}
		*/
	}
	MONITOR("SignForTimeuse::WTransit::WTransitManagerI::LoadMissFeedContent-->" << ts.getTime() << "@FeedMemcLoaderAdapter::GetFeedDict-->" << timeuse_GetFeedDict);		
}




// BY YunFeng.Ma     For IM loadOfflineFeedReply Encode
void WTransitManagerI::ClientEncode(string& body_string){
//----------------------@需求转义字符串:-----------------------------------
	try{
		//MCE_DEBUG("boost regex   body_string = " << body_string);
		boost::regex expression("@[^@]{1,}\\(\\d{1,10}\\)");
		//按照 @name(id) 做正则匹配

		//需要注意一些 用户的特殊回复 :    我印象中gmail总是和老kou(@顾烨辰(250944453) )联系在一起的  代码需要严谨
		std::string::const_iterator start, end, head, tail; 
		boost::match_results<std::string::const_iterator> result; 
		start = body_string.begin(); 
		end = body_string.end(); 
		int search_count = 0;
		while(regex_search(start, end, result, expression)) { 
			++search_count;
			//拼接profile链接
			head = result[0].first;
			tail = result[0].second; 
			size_t pos = head - body_string.begin();
			size_t size = tail - head;
			//MCE_DEBUG("WTransitManagerI::ClientEncode --> start = " << (*(result[0].first)) << " pos = " << pos << " size = " << size);
			size_t left_found = body_string.find_first_of("@", pos);
			size_t right_found = body_string.find_first_of("(", pos);
			if(right_found <= left_found){
				break;
			}
			string name = body_string.substr(left_found + 1, (right_found - left_found) - 1);
			left_found = body_string.find_first_of("(", right_found);
			right_found = body_string.find_first_of(")", right_found);
			if(right_found <= left_found){
				break;
			}
			string id = body_string.substr(left_found + 1, (right_found - left_found) - 1);
			body_string.replace(pos, size, "<a href='www.renren.com/g/" + id + "' target='_blank'>@" + name +"</a> ");
			//MCE_DEBUG("bodystring = " << body_string);

			//更新search的起始和终止位置
			start = body_string.begin(); 
			end = body_string.end(); 
			if(search_count > 20){
				MCE_WARN("WTransitManagerI::ClientEncode --> error!!!!!! serch_count has bigger than 20 may be run forever.... it will not lager than 10 buddy_string:" << body_string);
				break;
			}
		}	
	}catch(...){
		MCE_WARN("WTransitManagerI::ClientEncode --> error  body_string = " << body_string);
	}
//----------------------取离线2次转义特殊字符:-----------------------------------
	try{
		size_t pos = body_string.find("&");
		if(string::npos != pos){
			body_string = boost::regex_replace(body_string, boost::regex("&"), string("&amp;"));
		}
		pos = body_string.find("<");
		if(string::npos != pos){
			body_string = boost::regex_replace(body_string, boost::regex("<"), string("&lt;"));
		}
		pos = body_string.find(">");
		if(string::npos != pos){
			body_string = boost::regex_replace(body_string, boost::regex(">"), string("&gt;"));
		}
		pos = body_string.find("\"");
		if(string::npos != pos){
			body_string = boost::regex_replace(body_string, boost::regex("\""), string("&apos;"));
		}
	}catch(...){
		MCE_WARN("WTransitManagerI::ClientEncode --> error  body_string = " << body_string);
	}
	
}

void loadOfflineFeedReplyTask::handle(){
  TimeStat ts;
	MessageSeq msgs;
	CommentSeq seq;
	ReplyType replyType;
	if(_stype==502){
		replyType = STATUS_REPLY;
	}else if(_stype==701 || _stype==708){
		replyType = PHOTO_REPLY;
	}else if(_stype==601){
		replyType = BLOG_REPLY;
	}else if(_stype==709){
		replyType = ALBUM_REPLY;
	}else if((_stype == 103) || (_stype == 110) || (_stype == 102) || (_stype == 104) ||
		(_stype == 107) || (_stype == 101) || (_stype == 111) || (_stype == 106)){
		replyType = SHARE_REPLY;
	}
	//MCE_DEBUG("loadOfflineFeedReplyTask::handle --> user:" << _jid->userId << " stype:" << _stype << " actor:" << _actor << " source:" << _source << " replyType:" << replyType);
	/*int actualType;
	switch(_stype){
		case 601: actualType = 602;break;//发日志 601   日志回复 602
		case 502: actualType = 507;break;//修改状态 502 状态回复 506
		case 701: actualType = 704;break;//上传照片 701 上传单张照片回复 704
	}*/
	TimeStat tsgetReplySeq;
	float timeuse_getReplySeq;

	bool isreturn = false;
	for(int calltimes = 0; calltimes < 3; ++calltimes){
		try{
			seq = UgcCommentAdapter::instance().getReplySeq(_jid->userId, _actor, replyType, _source, 0, 50);
			timeuse_getReplySeq=tsgetReplySeq.getTime();
			isreturn = false;
			if(calltimes){
				MCE_INFO("loadOfflineFeedReplyTask::handle--> call UgcComment success at " << calltimes + 1 << " and get " << seq.size() << " comments !");
			}
			break;
		}catch(UGCCommentNewException& e){
			if(calltimes == 3){
				MCE_WARN("loadOfflineFeedReplyTask::handle-->UgcCommentAdapter::getReplySeq-->" << " userid:" << _jid->userId << " " << replyType << " " << e.msg);
			}
			isreturn = true;
			continue;
		}catch(Ice::ConnectTimeoutException& e){
			MCE_WARN("loadOfflineFeedReplyTask::handle -->UgcCommentAdapter::getReplySeq--> " << " userid:" << _jid->userId << " " << replyType << " " << e);
			isreturn = true;
			break;
		}catch (Ice::TimeoutException& e) {
			if(calltimes == 3){
				MCE_WARN("loadOfflineFeedReplyTask::handle -->UgcCommentAdapter::getReplySeq--> " << " userid:" << _jid->userId << " " << replyType << " " << e);
			}
			isreturn = true;
			continue;
		}
		catch(Ice::Exception& e){
			MCE_WARN("loadOfflineFeedReplyTask::handle -->UgcCommentAdapter::getReplySeq--> " << " userid:" << _jid->userId << " " << replyType << " " << e);
			isreturn = true;
			break;
		}
	}
	if(isreturn){
		return;
	}
	MCE_INFO("loadOfflineFeedReplyTask::handle --> user:" << _jid->userId << " comments:" << seq.size());

	ostringstream os;
	os << "<iq type='result' id='" << _iqId << "'>"
	<< "<query xmlns='http://talk.renren.com/xfeed/reply'>"
	<< "<feedreply id='" << _source << "_" << _stype << "_" << _actor << "' count='" << seq.size() << "'/>"
	<< "</query></iq>";
	//MCE_DEBUG("loadOfflineFeedReplyTask::handle --> @@@ " << os.str());
	try{
		TalkProxyAdapter::instance().express(_jid, os.str());
	}catch(Ice::Exception& e){
		MCE_WARN("loadOfflineFeedReplyTask::handle-->TalkProxyAdapter::express-->" << e);
	}

	for(int i=0; i<(int)seq.size(); i++){
		UGCCommentNewPtr comment = seq.at(i);
		if(comment->whisper == 1){
			MCE_INFO("loadOfflineFeedReplyTask::handle--> get whisper userid:" << _jid->userId << " actor:" << _actor << " replyType:" << replyType);
			continue;
		}
		ostringstream os;
		os << "<message type='common' from='feed.talk.renren.com@feed.talk.renren.com'>"
		<< "<xfeed xmlns='http://talk.renren.com/xfeed'>"
		<< "<id>" << comment->id << "</id>" << "<actor>" << comment->owner << "</actor>"
		<< "<time>" << comment->time/1000 << "</time>" << "<type>" << _stype/100 << "</type>"
		<< "<stypen />" << "<resource>" << comment->sourceId << "</resource>"
		<< "<f v='1.0' type='Reply'>"
		<< "<reply><id>" << comment->id << "</id>" << "<type>0</type>";
		MyUtil::Date t = Date::seconds(comment->time/1000);
		os << "<time>" << t.str("%Y-%m-%d %H:%M") << "</time>"
		//<< "<body>" << comment->body << "</body>"
		//<< "<im><body>" << comment->body << "</body></im>"
		<< "<from><id>" << comment->fromId << "</id>"
		<< "<name>" << comment->fromName << "</name>"
		<< "<tinyimg>" << comment->fromHeadUrl << "</tinyimg></from>"
		<< "<ui>" << comment->fromId << "</ui>" << "<rid>" << comment->id << "</rid>"
		<< "</reply>"
		<< "<from><fID>" << comment->fromId << "</fID>"
		<< "<fName>" << comment->fromName << "</fName></from>"
		<< "<count>" << seq.size() << "</count>"
		<< "<biz>" << comment->sourceId << "_" << _stype << "_" << comment->owner << "</biz>"
		<< "<type>" << _stype << "</type>"
		<< "</f>"
		<< FeedReplyToMessagConverter::_getTemplate(_stype)
		<< "</xfeed></message>";

    WTransitManagerI::instance().ClientEncode(comment->body);

    xml_document doc;
    if(!doc.load(os.str().c_str())){
      MCE_WARN("LoadOfflineFeedReplyTask::handle --> xml err, " << _jid->userId);
      return;
    }
    xml_node n = doc.child("message").child("xfeed").child("f").child("reply").append_child();
		n.set_name("body");
    n.append_child(node_pcdata).set_value(comment->body.c_str());
    n = doc.child("message").child("xfeed").child("f").child("reply").append_child();
    n.set_name("im");
    n = n.append_child();
    n.set_name("body");
    n.append_child(node_pcdata).set_value(comment->body.c_str());

    //MCE_DEBUG("loadOfflineFeedReplyTask::handle userid = " << _jid->userId << " XML is --> " << doc.xml());
    MessagePtr msg = new Message();
		JidPtr jfrom = new Jid();
		jfrom->userId = comment->type;
		jfrom->endpoint = "feed.talk.xiaonei.com";
		jfrom->index = -1;
		msg->from = jfrom;
		msg->to = _jid;
		msg->type = SYSTEM_MESSAGE;
		msg->msg = doc.xml();
		msgs.push_back(msg);
	}
	try {	
		TalkDeliverAdapter::instance().deliver(msgs);
	} catch (Ice::Exception& e) {
		MCE_WARN("WTransitManagerI::ClientEncode-->TalkDeliverAdapter::deliver-->" << e);
	}
  //MCE_DEBUG("loadOfflineFeedReplyTask::handle --> OFFLINEREPLY user : " << _jid->userId << " stype = " << _stype << " actor = " << _actor << " source = " << _source << " replytype = " << replyType << " timeuse = " << ts.getTime());
	MONITOR("SignForTimeuse::WTransit::loadOfflineFeedReplyTask::handle-->" << ts.getTime() << "@UgcCommentAdapter::getReplySeq-->" << timeuse_getReplySeq << " useid:" << _jid->userId << " seq.size:" << seq.size() << " replytype:" << replyType);
	FunStatManager::instance().Stat("WTransitManagerI::loadOfflineFeedReply", ts.getTime(), false);
}




map<Ice::Long, int> WTransitManagerI::getFeedReplyCount(const LongSeq& feeds, const Ice::Current&){
  TimeStat ts;
	LongSeq realFeeds;

	if(feeds.empty()){
		return map<Ice::Long, int>();
	}

	for(int i=0; i<(int)feeds.size(); i++){
		Ice::Long id = feeds.at(i)-baseFeedId;
		if(id > 0){
			realFeeds.push_back(id);
		}
	}
	FeedContentDict fdict;
	TimeStat tsgetFeedDict;
	float timeuse_getFeedDict;
	TimeStat tsLoadMissFeedContent;
	try{
		//fdict = FeedContentAdapter::instance().getFeedDict(realFeeds);
		fdict = GetFeedContents(realFeeds);
		timeuse_getFeedDict = tsgetFeedDict.getTime();
		LoadMissFeedContent(fdict, realFeeds);
	}catch(Ice::Exception& e){
		MCE_WARN("WTransitManagerI::getFeedReplyCount-->FeedContentAdapter::getFeedDict-->" << feeds.size() << ", " << e);
		return map<Ice::Long, int>();
	}
	//MCE_DEBUG("WTransitManagerI::loadReplyCount --> feed size:" << feeds.size() << " res size:" <<  fdict.size());
	map<Ice::Long, int> res;
	map<Ice::Long, FeedContentPtr>::iterator it = fdict.begin();
	for(; it!=fdict.end(); ++it){
		int count = 0;
    if(it->second){
      if(it->second->reply){
			  count = it->second->reply->count;
      }
		  res[it->first + baseFeedId] = count;
		}
	}
  //MCE_DEBUG("interface WTransitManagerI::getFeedReplyCount usetime = " << ts.getTime() << " feed size:" << feeds.size() << " res size:" << fdict.size());
	MONITOR("SignForTimeuse::WTransit::WTransitManagerI::getFeedReplyCount-->" << ts.getTime() << "@FeedContentAdapter::getFeedDict-->" << timeuse_getFeedDict);
	return res;
}





void loadFeedReplyCountTask::handle(){
	TimeStat ts;
	LongSeq realFeeds;
	for(int i=0; i<(int)_feeds.size(); i++){
		Ice::Long id = _feeds.at(i)-WTransitManagerI::baseFeedId;
		if(id > 0){
			realFeeds.push_back(id);
		}
	}
	FeedContentDict fdict;
	TimeStat tsgetFeedDict;
	float timeuse_getFeedDict;
	TimeStat tsLoadMissFeedContent;
	try{
		//fdict = FeedContentAdapter::instance().getFeedDict(realFeeds);
		fdict = WTransitManagerI::instance().GetFeedContents(realFeeds);
		timeuse_getFeedDict=tsgetFeedDict.getTime();
		WTransitManagerI::instance().LoadMissFeedContent(fdict, realFeeds);
	}catch(Ice::Exception& e){
		MCE_WARN("loadFeedReplyCountTask::handle-->FeedContentAdapter::getFeedDict-->:" << _feeds.size() << ", " << e);
		return;
	}
	//MCE_DEBUG("loadFeedReplyCountTask::handle --> feed size:" << _feeds.size() << " res size:" <<  fdict.size());
	map<Ice::Long, int> res;
	map<Ice::Long, FeedContentPtr>::iterator it = fdict.begin();
	for(; it!=fdict.end(); ++it){
		int count = 0;
    if(it->second){
      if(it->second->reply){
			  count = it->second->reply->count;
      }
		  res[it->first + WTransitManagerI::baseFeedId] = count;
		}
	}
  stringstream os;
  os << "<iq type='result' id='" << _iqid << "'>" 
    << "<query xmlns='" << _xmlns << "'>";
  map<Ice::Long, int>::iterator rit = res.begin();
  for(; rit!=res.end(); ++rit){
    os << "<feedreply id='" << rit->first << "' count='" << rit->second << "'/>";
  }
  os << "</query></iq>";
  
  MessageSeq seq;
  MessagePtr msg = new Message;
  msg->to = _jid;
  msg->from = _jid;
  msg->type = IQ_RESULT;
  msg->msg = os.str();
  seq.push_back(msg);
  try{ 
    //MCE_DEBUG("loadFeedReplyCountTask::handle --> deliver " << jidToString(_jid) << " msg:" << os.str());
    TalkProxyAdapter::instance().express(_jid, os.str());
		TalkDeliverAdapter::instance().deliver(seq);
  }catch(Ice::Exception& e){
    MCE_WARN("loadFeedReplyCountTask::handle-->TalkDeliverAdapter::deliver--> " << e);
  }
	MONITOR("SignForTimeuse::WTransit::loadFeedReplyCountTask::handle-->" << ts.getTime() << "@FeedContentAdapter::getFeedDict-->" << timeuse_getFeedDict);
	FunStatManager::instance().Stat("WTransitManagerI::deliverFeedReplyCount", ts.getTime(), false);

}

void WTransitManagerI::deliverFeedReplyCount(const JidPtr& jid, const LongSeq& feeds, const string& iqid, const string& xmlns, const Ice::Current& current){
  TimeStat ts;
  TaskManager::instance().execute(new loadFeedReplyCountTask(jid, feeds, iqid, xmlns)); 
  //MCE_DEBUG("interface deliverFeedReplyCount::handle usetime = " << ts.getTime());
}



void WTransitManagerI::deliverNotifyWebpager(const MyUtil::IntSeq& toids, const string& content, const Ice::Current& current){
	TimeStat ts;
	TaskManager::instance().execute(new deliverNotifyWebpagerTask(toids, content));
  //MCE_DEBUG("interface deliverNotifyWebpager::handle usetime = " << ts.getTime());	
}

void deliverNotifyWebpagerTask::handle(){
	TimeStat ts;
	MessageSeq msgs;
  bool is_feed_actor = false;
	Ice::Long nid = 0;
	if(_content.find("ugc_content") != string::npos){
		//MCE_DEBUG("WTransitManagerI::deliverNotifyWebpager --> deliver status or photo data to httpserver size : " << _toids.size());
		is_feed_actor = true;
	}
	size_t npos = _content.find("\"nid\":\"");
	if(npos != string::npos){
		size_t ipos = _content.find_first_of("\"", npos + 7);
		if(ipos != string::npos){
			string snid = _content.substr(npos+7, ipos - (npos+7));
			try{
				nid = boost::lexical_cast<Ice::Long>(snid);
			}catch(...){
				MCE_WARN("WTransitManagerI::deliverNotifyWebpager--> boost cast nid error nid = " << nid);
			}
		}
	}
	//WTransitManagerI::instance().PrintLog("WTransitManagerI::deliverNotifyWebpager", nid, _toids);
	for(int i=0; i<(int)_toids.size(); i++){
		JidSeq jseq;
		try{
			jseq = OnlineCenterAdapter::instance().getUserJids(_toids.at(i),2);
		}catch(Ice::Exception& e){
			MCE_WARN("WTransitManagerI::deliverNotifyWebpager-->OnlineCenterAdapter::getUserJids-->" << e);
		}
		for(int j=0; j<(int)jseq.size(); j++){
			if(jseq.at(j)->endpoint[0] == 'W'){
				MessagePtr msg = new Message();
				JidPtr jfrom = new Jid();
				jfrom->userId = 0;
				jfrom->endpoint = "feed.talk.xiaonei.com";
				jfrom->index = -1;
				ostringstream os;
				os << "<message type='common' from='" << jidToString(jfrom) << "' to='" << jidToString(jseq.at(j)) << "'>";
				os << "</message>";
				xml_document doc;
				if(!doc.load(os.str().c_str())){
					continue;
				}
				xml_node body = doc.child("message").append_child();
				body.set_name("body");
				body.append_child(node_pcdata).set_value(_content.c_str());
				msg->from = jfrom;
				msg->to = jseq.at(j);
				msg->type = NOTIFY2_MESSAGE;
				msg->msg = doc.xml();
				msgs.push_back(msg);
			}
		}
	}
	if(msgs.empty()){
		return;
	}
	ostringstream os;
	for(IntSeq::iterator idit = _toids.begin(); idit != _toids.end(); ++idit){
		os << (*idit) << " ";
	}
	try{
		MCE_INFO("WTransitManagerI::deliverNotifyWebpager--> is_feed_actor = " << is_feed_actor << " nid : " << nid  << " toids.size :" << _toids.size() << " toids : " << os.str());
		TalkDeliverAdapter::instance().deliver(msgs);
	}catch(Ice::Exception& e){
		MCE_WARN("WTransitManagerI::deliverNotifyWebpager-->TalkDeliverAdapter::deliver--> " << e);
	}
	MONITOR("SignForTimeuse::WTransit::deliverNotifyWebpagerTask::handle-->" << ts.getTime());
	FunStatManager::instance().Stat("WTransitManagerI::deliverNotifyWebpager", ts.getTime(), false);
}

void WTransitManagerI::deliverNotifyClient(const MyUtil::IntSeq& toids, const string& content, const Ice::Current& current){
	TimeStat ts;
	TaskManager::instance().execute(new deliverNotifyClientTask(toids, content));
}


void deliverNotifyClientTask::handle(){
	TimeStat ts;
	xml_document doc;
	if(!doc.load(_content.c_str())){
		MCE_WARN("WTransitManagerI::deliverNotifyClient --> xml err, " <<_content);
		return;
	}

	Ice::Long nid = 0;
	size_t npos = _content.find("\"nid\":\"");
	if(npos != string::npos){
		size_t ipos = _content.find_first_of("\"", npos + 7);
		if(ipos != string::npos){
			string snid = _content.substr(npos+7, ipos - (npos+7));
			try{
				nid = boost::lexical_cast<Ice::Long>(snid);
			}catch(...){
				MCE_WARN("WTransitManagerI::deliverNotifyClient--> boost cast nid error nid = " << nid);
			}
		}
	}
	//WTransitManagerI::instance().PrintLog("WTransitManagerI::deliverNotifyClient", nid, _toids);
//	Ice::Long replyId = 0;
//	try{
//		replyId = boost::lexical_cast<Ice::Long>(doc.child("message").child("xfeed").child("id").first_child().value());
//	}catch(...){
//		MCE_WARN("LoadFeedTask::handle --> cast replyid err");
//	}
//	Ice::Long id = -(-replyId + baseReplyId);
//	doc.child("message").child("xfeed").remove_child("id");
//	xml_node tmp = doc.child("message").child("xfeed").append_child();
//	tmp.set_name("id");
//	tmp.append_child(node_pcdata).set_value(boost::lexical_cast<string>(id).c_str());
	
	MessageSeq msgs;
	for(int i=0; i<(int)_toids.size(); i++){
		JidSeq jseq;
		try{
			jseq = OnlineCenterAdapter::instance().getUserJids(_toids.at(i),4);
		}catch(Ice::Exception& e){
			MCE_WARN("WTransitManagerI::deliverNotifyClient-->OnlineCenterAdapter::getUserJids-->" << e);
		}
		MCE_INFO("WTransitManagerI::deliverNotifyClient-->jseq:" << jseq.size() << " uid:" << _toids.at(i));
		for(int j=0; j<(int)jseq.size(); j++){
			if(jseq.at(j)->endpoint[0] == 'T'){
				MessagePtr msg = new Message();
				JidPtr jfrom = new Jid();
				jfrom->userId = 0;
				jfrom->endpoint = "feed.talk.xiaonei.com";
				jfrom->index = -1;

				doc.child("message").append_attribute("to") = jidToString(jseq.at(j)).c_str();
				//doc.child("message").append_attribute("from") = jidToString(jfrom).c_str();

				msg->from = jfrom;
				msg->to = jseq.at(j);
				msg->type = NOTIFY2_MESSAGE;
				msg->msg = doc.xml();
				msgs.push_back(msg);
			}
		}
	}
	if(msgs.empty()){
		MCE_INFO("WTransitManagerI::deliverNotifyClient-->message is empty!");
		return;
	}
	try{
		TalkDeliverAdapter::instance().deliver(msgs);
	}catch(Ice::Exception& e){
		MCE_WARN("WTransitManagerI::deliverNotifyClient-->TalkDeliverAdapter::deliver-->" << e);
	}
	MONITOR("SignForTimeuse::WTransit::deliverNotifyClientTask::handle-->" << ts.getTime());
	FunStatManager::instance().Stat("WTransitManagerI::deliverNotifyClient", ts.getTime(), false);
}


void WTransitManagerI::deliverNotifyPhone(const MyUtil::IntSeq& toids, const string& content, const Ice::Current& current){
	TimeStat ts;
	TaskManager::instance().execute(new deliverNotifyPhoneTask(toids, content));
}
void deliverNotifyPhoneTask::handle(){
	TimeStat ts;
	xml_document doc;
	if(!doc.load(_content.c_str())){
		MCE_WARN("WTransitManagerI::deliverNotifyPhone --> xml err, " <<_content);
		return;
	}
	Ice::Long nid = 0;
	size_t npos = _content.find("\"nid\":\"");
	if(npos != string::npos){
		size_t ipos = _content.find_first_of("\"", npos + 7);
		if(ipos != string::npos){
			string snid = _content.substr(npos+7, ipos - (npos+7));
			try{
				nid = boost::lexical_cast<Ice::Long>(snid);
			}catch(...){
				MCE_WARN("WTransitManagerI::deliverNotifyClient--> boost cast nid error nid = " << nid);
			}
		}
	}
	//WTransitManagerI::instance().PrintLog("WTransitManagerI::deliverNotifyPhone", nid, _toids);
//	int type = 0;
//	int replied_id = 0;
//	try{
//		type = lexical_cast<int>(doc.child("message").child("reply").child("type").first_child().value());
//		replied_id = lexical_cast<int>(doc.child("message").child("reply").child("replied_id").first_child().value());
//	}catch(...){
//		MCE_WARN("WTransitManagerI::deliverNotifyPhone --> cast err");
//	}
//	MCE_DEBUG("WTransitManagerI::deliverNotifyPhone --> type:" << type << " replied_id:" << replied_id);
//
	MessageSeq msgs;
	for(int i=0; i<(int)_toids.size(); i++){
		JidSeq jseq;
		try{
			jseq = OnlineCenterAdapter::instance().getUserJids(_toids.at(i),8);
		}catch(Ice::Exception& e){
			MCE_WARN("WTransitManagerI::deliverNotifyPhone-->OnlineCenterAdapter::getUserJids--> " << e);
		}
		//MCE_DEBUG("WTransitManagerI::deliverNotifyPhone --> userid:" << _toids.at(i) << " online size:" << jseq.size());
		for(int j=0; j<(int)jseq.size(); j++){
			if(jseq.at(j)->endpoint[0] == 'P'){
				MessagePtr msg = new Message();
				JidPtr jfrom = new Jid();
				jfrom->userId = 0;
				jfrom->endpoint = "feed.talk.xiaonei.com";
				jfrom->index = -1;

				doc.child("message").append_attribute("to") = jidToString(jseq.at(j)).c_str();
				//doc.child("message").append_attribute("from") = jidToString(jfrom).c_str();

				msg->from = jfrom;
				msg->to = jseq.at(j);
				msg->type = PHONE_NOTIFY;
				msg->msg = doc.xml();
				msgs.push_back(msg);
			}
		}
	}
	if(msgs.empty()){
		return;
	}
	try{
		TalkDeliverAdapter::instance().deliver(msgs);
	}catch(Ice::Exception& e){
		MCE_WARN("WTransitManagerI::deliverNotifyPhone-->TalkDeliverAdapter::deliver-->" << e);
	}
	MONITOR("SignForTimeuse::WTransit::deliverNotifyPhoneTask::handle-->" << ts.getTime());
	FunStatManager::instance().Stat("WTransitManagerI::deliverNotifyPhone", ts.getTime(), false);

}

void WTransitManagerI::deliverGroupNotifyPager(const MyUtil::IntSeq& groupids, const MyUtil::IntSeq& touids, const MyUtil::IntSeq& blockuids, const string& content, const Ice::Current&){
	MCE_INFO("WTransitManagerI::deliverGroupNotifyPager--> groupids.size:" << groupids.size() << " blockuids.size:" << blockuids.size());
	TaskManager::instance().execute(new deliverNotifyGroupTask(groupids, touids, blockuids, content, 'W'));
}
void WTransitManagerI::deliverGroupNotifyClient(const MyUtil::IntSeq& groupids, const MyUtil::IntSeq& touids, const MyUtil::IntSeq& blockuids, const string& content, const Ice::Current&){
	MCE_INFO("WTransitManagerI::deliverGroupNotifyClient--> groupids.size:" << groupids.size() << " blockuids.size:" << blockuids.size());
	TaskManager::instance().execute(new deliverNotifyGroupTask(groupids, touids, blockuids, content, 'T'));
}
void WTransitManagerI::deliverGroupNotifyPhone(const MyUtil::IntSeq& groupids, const MyUtil::IntSeq& touids, const MyUtil::IntSeq& blockuids, const string& content, const Ice::Current&){
	MCE_INFO("WTransitManagerI::deliverGroupNotifyPhone--> groupids.size:" << groupids.size() << " blockuids.size:" << blockuids.size());
	TaskManager::instance().execute(new deliverNotifyGroupTask(groupids, touids, blockuids, content, 'P'));
}


void deliverNotifyGroupTask::handle(){
	TimeStat ts;
	xml_document doc;
	if('W' != _endpoint){
		if(!doc.load(_content.c_str())){
			MCE_WARN("WTransitManagerI::deliverNotifyGroupTask --> xml err, " <<_content);
			return;
		}
	}
	set<JidPtr> jset;
	set<int> uidset;
	set<int> blocks;
	for(IntSeq::const_iterator bit = _blockuids.begin(); bit != _blockuids.end(); ++bit){
		blocks.insert((*bit));
	}
	for(IntSeq::const_iterator uit = _touids.begin(); uit != _touids.end(); ++uit){
		if(blocks.count(*uit) <= 0){
			uidset.insert(*uit);
		}
	}
	for(IntSeq::const_iterator git = _groupids.begin(); git != _groupids.end(); ++git){
		int groupid = (*git);
		MucRoomIdPtr roomid = new MucRoomId();
		roomid->roomname = boost::lexical_cast<string>(groupid);
		roomid->domain = "group.talk.xiaonei.com";
		JidSeq ans;
		try{
			ans = MucOnlineCenterAdapter::instance().GetActiveJids(roomid);
		}catch(Ice::Exception& e){
			MCE_WARN("WTransitManagerI::deliverGroupNotifyPager-->MucGateAdapter::GetRoomUserList-->groupid:" << groupid << " error:" << e);
		}
		MCE_INFO("WTransitManagerI::deliverGroupNotifyPager--> groupid:" << groupid << " ans.size():" << ans.size());
		for(JidSeq::iterator jit = ans.begin(); jit != ans.end(); ++jit){
			if(blocks.count((*jit)->userId) <= 0 && (_endpoint == (*jit)->endpoint[0])){
				jset.insert((*jit));
				if(uidset.count((*jit)->userId) > 0){
					uidset.erase((*jit)->userId);
				}
			}
		}
	}	
	MessageSeq msgs;
	for(set<JidPtr>::iterator sit = jset.begin(); sit != jset.end(); ++sit){
		MessagePtr msg = new Message();
		JidPtr jfrom = new Jid();
		jfrom->userId = 0;
		jfrom->endpoint = "feed.talk.xiaonei.com";
		jfrom->index = -1;

		if('W' == _endpoint){
			ostringstream os;
			os << "<message type='common' from='" << jidToString(jfrom) << "' to='" << jidToString(*sit) << "'>";
			os << "</message>";
			xml_document doc1;
			if(!doc1.load(os.str().c_str())){
				continue;
			}
			xml_node body = doc1.child("message").append_child();
			body.set_name("body");
			body.append_child(node_pcdata).set_value(_content.c_str());
			msg->from = jfrom;
			msg->to = (*sit); 
			msg->type = NOTIFY2_MESSAGE;
			msg->msg = doc1.xml();
			msgs.push_back(msg);
		}else{
			doc.child("message").append_attribute("to") = jidToString(*sit).c_str();
			msg->from = jfrom;
			msg->to = (*sit);
			if('P' == _endpoint){
				msg->type = PHONE_NOTIFY;
			}else{
				msg->type = NOTIFY2_MESSAGE;
			}
			msg->msg = doc.xml();
			msgs.push_back(msg);
		}
	}
	if(!msgs.empty()){
		MCE_INFO("WTransitManagerI::deliverGroupNotifyPager-->jids.size:" << jset.size() << " msgs.size:" << msgs.size());
		try{
			TalkDeliverAdapter::instance().deliver(msgs);
		}catch(Ice::Exception& e){
			MCE_WARN("WTransitManagerI::deliverNotifyGroupTask-->TalkDeliverAdapter::deliver-->" << e);
		}
	}
	
	vector<int> uidseq;
	for(set<int>::iterator uit = uidset.begin(); uit != uidset.end(); ++uit){
		uidseq.push_back((*uit));
	}
	if(!uidseq.empty()){	
		if('P' == _endpoint){
			FunStatManager::instance().Stat("WTransitManagerI::deliverGroupNotifyPhone", ts.getTime(), false);
			WTransitManagerI::instance().deliverNotifyPhone(uidseq, _content);
		}else if('W' == _endpoint){
			FunStatManager::instance().Stat("WTransitManagerI::deliverGroupNotifyPager", ts.getTime(), false);
			WTransitManagerI::instance().deliverNotifyWebpager(uidseq, _content);
		}else if('T' == _endpoint){
			FunStatManager::instance().Stat("WTransitManagerI::deliverGroupNotifyClient", ts.getTime(), false);
			WTransitManagerI::instance().deliverNotifyClient(uidseq, _content);
		}
	}
}


void WTransitManagerI::CallFeedCacheAndContent(const JidPtr& jid, Ice::Long feedIndex, Ice::Long replyIndex, int iqId, const Ice::Current&){
	if(!jid){
		return;
	}
  TaskManager::instance().execute(new CallFeedCacheAndContentTask(jid, feedIndex, replyIndex, iqId));
}

void WTransitManagerI::CallFeedCache(const JidPtr& jid, Ice::Long feedIndex, Ice::Long replyIndex, int iqId, const Ice::Current&){
	if(!jid){
		return;
	}
  TaskManager::instance().execute(new CallFeedCacheTask(jid, feedIndex, replyIndex, iqId));
}
void WTransitManagerI::CallNotifyGate(const JidPtr& jid, Ice::Long feedIndex, Ice::Long replyIndex, int iqId, const Ice::Current&){
  TaskManager::instance().execute(new CallNotifyGateTask(jid, feedIndex, replyIndex, iqId));
}

void CallNotifyGateTask::handle(){
	Ice::Long maxNoteId = 2000000000;
	int updateTime = 1270662628 + 300;

	TimeStat tsgetNotifySeq2;
	float timeuse_getNotifySeq2;
	Ice::Long maxReplyId = _replyIndex;
	StrSeq notifySeq;

	try{
		notifySeq = NotifyGateAdapter::instance().getNotifySeq2(_jid->userId, 5, 0, 200);
	}catch(Ice::Exception& e){
		MCE_WARN("CallNotifyGateTask::handle-->NotifyGateAdapter::getNotifySeq2-->" << _jid->userId << " error:" << e);
	}
	timeuse_getNotifySeq2 = tsgetNotifySeq2.getTime();
	MCE_INFO("CallNotifyGateTask::handle--> NotifyGateAdapter::getNotifySeq2--> jid:" << _jid->userId << " notifySeq.size:" << notifySeq.size())

		int countFromNew = 0;
	int fifteendays = 15 * 24 * 60 * 60;
	time_t now = time(NULL);
	//int countFromOld = noteSeq1.size();
	for (int i = notifySeq.size() - 1; i >= 0; --i) {
		xml_document doc;
		if(!doc.load(notifySeq.at(i).c_str())){
			continue;
		}
		time_t time = 0;
		try{
			time = boost::lexical_cast<int>(doc.child("message").child("xfeed").child("time").first_child().value());
		}catch(...){
			MCE_WARN("CallNotifyGateTask::handle --> cast time err, time:" << doc.child("message").child("xfeed").child("time").first_child().value() << " data:" << doc.xml());
		}
		if(now - time > fifteendays){ //产品要求 : 超过15天的提醒不推送
			break;
		}
		if(time < updateTime){
			continue;
		}
		Ice::Long replyId = 0;
		try{
			replyId = boost::lexical_cast<Ice::Long>(doc.child("message").child("xfeed").child("id").first_child().value());
		}catch(...){
			MCE_WARN("CallNotifyGateTask::handle --> cast replyid err, " << doc.child("message").child("xfeed").child("id").first_child().value());
		}
		if((-replyId) <= _replyIndex){
			break;
		}
		if(i==0){
			maxReplyId = -replyId;
		}
		/*if((-replyId) <= (_replyIndex - WTransitManagerI::baseReplyId)){
			break;
			}
			Ice::Long id = -(-replyId + WTransitManagerI::baseReplyId);
			doc.child("message").child("xfeed").remove_child("id");
			xml_node tmp = doc.child("message").child("xfeed").append_child();
			tmp.set_name("id");
			tmp.append_child(node_pcdata).set_value(boost::lexical_cast<string>(id).c_str());
		 */
		countFromNew++;
		MessagePtr msg = new Message();
		JidPtr jfrom = new Jid();
		jfrom->userId = 0;
		jfrom->endpoint = "feed.talk.xiaonei.com";
		jfrom->index = -1;

		doc.child("message").append_attribute("to") = jidToString(_jid).c_str();
		msg->from = jfrom;
		msg->to = _jid;
		msg->type = NOTIFY2_MESSAGE;
		msg->msg = doc.xml();
	}
}

void CallFeedCacheAndContentTask::handle(){
	TimeStat ts;
	MessageSeq mseqs;
	mseqs.push_back(0);
	//MCE_DEBUG("LoadFeedTask::handle --> userid:" << _jid->userId << " feedid:" << _feedIndex << " replyid:" << _replyIndex);
	Ice::Long maxFeedId = _feedIndex;
	float timeuse_getFeedDict = 0.0;
	FeedItemSeq feeds;      
	TimeStat tsget;             
	float timeuse_get;            
	try {                               
		feeds = FeedCacheNAdapter::instance().get(_jid->userId);
		timeuse_get = tsget.getTime();        
	} catch (Ice::Exception& e) {                   
		MCE_WARN("CallFeedCacheAndContent::handle-->FeedCacheAdapter::get-->"<<_jid->userId<<":"<<e);
	}                                                               
	if(!feeds.empty()){                                                   
		MyUtil::LongSeq feedIds;                                                  
		for( int h=feeds.size() - 1; h>= 0; --h ){                                        
			if(!WTransitManagerI::instance().isDeliver2Client(feeds.at(h).type&0xFFFF)){          
				continue;                                                                                 
			}                                                                                                     
			//只取100条
			//if(h > 100){
			//  break;
			//}
			if(maxFeedId < WTransitManagerI::baseFeedId){
				//旧版feedid
				Ice::Long id = 6000000000000000000 + ((Ice::Long)feeds.at(h).time)*100000 + ((feeds.at(h).feed & 0xffffffff) % 100000);
				if(id > maxFeedId){
					feedIds.push_back(feeds.at(h).feed);
				}
				//else{
				//  break;
				//}
			}else{
				//新版feedid
				Ice::Long id = maxFeedId - WTransitManagerI::baseFeedId;
				if(feeds.at(h).feed > id){
					feedIds.push_back(feeds.at(h).feed);
				}
				//else{
				//  break;
				//}
			}
		}
		if(!feeds.empty()){
			//maxFeedId = 6000000000000000000 + ((Ice::Long)feeds.at(0).time)*100000 + ((feeds.at(0).feed & 0xffffffff) % 100000);
			maxFeedId = WTransitManagerI::baseFeedId + feeds.at(0).feed;
			//TODO
		}
		FeedContentDict contents;
		TimeStat contentts;
		
		if(!feedIds.empty()){
			TimeStat tsgetFeedDict;
			TimeStat tsLoadMissFeedContent;
			try {
				contents = WTransitManagerI::instance().GetFeedContents(feedIds);
				timeuse_getFeedDict = tsgetFeedDict.getTime();
				WTransitManagerI::instance().LoadMissFeedContent(contents, feedIds);
			} catch (Ice::Exception& e) {
				MCE_WARN("CallFeedCacheAndContent::handle-->FeedContentAdapter::getFeedDict-->:"<<e);
			}
			MCE_INFO("CallFeedCacheAndContent::handle --> get content actor=" << _jid->userId << " feedIds.size=" << feedIds.size() << " contents.size="<<contents.size());
			
			if (!contents.empty()) {
				FeedToMessagConverter converter;
				for (FeedContentDict::iterator it = contents.begin(); it != contents.end(); ++it) {
					if(it->second){
						converter.setData(it->second);
						string feedxml = it->second->data->xml;
						int feedxml_size = feedxml.size();
						MCE_DEBUG("CallFeedCacheContent::handle --> feedxml = " << feedxml << " size = " << feedxml_size);
						
						//MessagePtr msg = converter.getToTalkMessage(_jid, true);
						//if( msg ){
						//	mseqs.push_back(msg);
						//}
						
					} else{
						MCE_DEBUG("CallFeedCacheContent::handle --> can not get feed=" << it->first);
					}
				}
			}
			
		}
	}
	Ice::Long maxNoteId = 2000000000;
	int updateTime = 1270662628 + 300;

	TimeStat tsgetNotifySeq2;
	float timeuse_getNotifySeq2;
	Ice::Long maxReplyId = _replyIndex;
	StrSeq notifySeq;

	try{
		notifySeq = NotifyGateAdapter::instance().getNotifySeq2(_jid->userId, 5, 0, 200);
	}catch(Ice::Exception& e){
		MCE_WARN("CallNotifyGateTask::handle-->NotifyGateAdapter::getNotifySeq2-->" << _jid->userId << " error:" << e);
	}
	timeuse_getNotifySeq2 = tsgetNotifySeq2.getTime();
	MCE_INFO("CallNotifyGateTask::handle--> NotifyGateAdapter::getNotifySeq2--> jid:" << _jid->userId << " notifySeq.size:" << notifySeq.size())

		int countFromNew = 0;
	int fifteendays = 15 * 24 * 60 * 60;
	time_t now = time(NULL);
	//int countFromOld = noteSeq1.size();
	for (int i = notifySeq.size() - 1; i >= 0; --i) {
		xml_document doc;
		if(!doc.load(notifySeq.at(i).c_str())){
			continue;
		}
		time_t time = 0;
		try{
			time = boost::lexical_cast<int>(doc.child("message").child("xfeed").child("time").first_child().value());
		}catch(...){
		}
		if(now - time > fifteendays){ //产品要求 : 超过15天的提醒不推送
			break;
		}
		if(time < updateTime){
			continue;
		}
		Ice::Long replyId = 0;
		try{
			replyId = boost::lexical_cast<Ice::Long>(doc.child("message").child("xfeed").child("id").first_child().value());
		}catch(...){
			MCE_WARN("CallNotifyGateTask::handle --> cast replyid err, " << doc.child("message").child("xfeed").child("id").first_child().value());
		}
		if((-replyId) <= _replyIndex){
			break;
		}
		if(i==0){
			maxReplyId = -replyId;
		}
		/*if((-replyId) <= (_replyIndex - WTransitManagerI::baseReplyId)){
			break;
			}
			Ice::Long id = -(-replyId + WTransitManagerI::baseReplyId);
			doc.child("message").child("xfeed").remove_child("id");
			xml_node tmp = doc.child("message").child("xfeed").append_child();
			tmp.set_name("id");
			tmp.append_child(node_pcdata).set_value(boost::lexical_cast<string>(id).c_str());
		 */
		countFromNew++;
		MessagePtr msg = new Message();
		JidPtr jfrom = new Jid();
		jfrom->userId = 0;
		jfrom->endpoint = "feed.talk.xiaonei.com";
		jfrom->index = -1;

		doc.child("message").append_attribute("to") = jidToString(_jid).c_str();
		msg->from = jfrom;
		msg->to = _jid;
		msg->type = NOTIFY2_MESSAGE;
		msg->msg = doc.xml();
	}
}

void CallFeedCacheTask::handle(){
	TimeStat ts;
	MessageSeq mseqs;
	mseqs.push_back(0);
	//MCE_DEBUG("LoadFeedTask::handle --> userid:" << _jid->userId << " feedid:" << _feedIndex << " replyid:" << _replyIndex);
	Ice::Long maxFeedId = _feedIndex;
	float timeuse_getFeedDict = 0.0;
	FeedItemSeq feeds;
	TimeStat tsget;
	float timeuse_get;
	try {
		feeds = FeedCacheNAdapter::instance().get(_jid->userId);
		timeuse_get = tsget.getTime();
	} catch (Ice::Exception& e) {
		MCE_WARN("CallFeedCache::handle-->FeedCacheAdapter::get-->"<<_jid->userId<<":"<<e);
	}
	if(!feeds.empty()){
		MyUtil::LongSeq feedIds;
		for( int h=feeds.size() - 1; h>= 0; --h ){
			if(!WTransitManagerI::instance().isDeliver2Client(feeds.at(h).type&0xFFFF)){
				continue;
			}
			//只取100条
			//if(h > 100){
			//  break;
			//}
			if(maxFeedId < WTransitManagerI::baseFeedId){
				//旧版feedid
				Ice::Long id = 6000000000000000000 + ((Ice::Long)feeds.at(h).time)*100000 + ((feeds.at(h).feed & 0xffffffff) % 100000);
				if(id > maxFeedId){
					feedIds.push_back(feeds.at(h).feed);
				}
				//else{
				//  break;
				//}
			}else{
				//新版feedid
				Ice::Long id = maxFeedId - WTransitManagerI::baseFeedId;
				if(feeds.at(h).feed > id){
					feedIds.push_back(feeds.at(h).feed);
				}
				//else{
				//  break;
				//}
			}
		}
		if(!feeds.empty()){
			//maxFeedId = 6000000000000000000 + ((Ice::Long)feeds.at(0).time)*100000 + ((feeds.at(0).feed & 0xffffffff) % 100000);
			maxFeedId = WTransitManagerI::baseFeedId + feeds.at(0).feed;
			//TODO
		}
		FeedContentDict contents;
		TimeStat contentts;
		MCE_INFO("CallFeedCache::handle-->" << feedIds.size());
		/*
		if(!feedIds.empty()){
			TimeStat tsgetFeedDict;
			TimeStat tsLoadMissFeedContent;
			try {
				contents = WTransitManagerI::instance().GetFeedContents(feedIds);
				timeuse_getFeedDict = tsgetFeedDict.getTime();
				WTransitManagerI::instance().LoadMissFeedContent(contents, feedIds);
			} catch (Ice::Exception& e) {
				MCE_WARN("CallFeedCache::handle-->FeedContentAdapter::getFeedDict-->:"<<e);
			}
			MCE_INFO("CallFeedCache::handle --> get content actor=" << _jid->userId << " feedIds.size=" << feedIds.size() << " contents.size="<<contents.size());
			if (!contents.empty()) {
				FeedToMessagConverter converter;
				for (FeedContentDict::iterator it = contents.begin(); it != contents.end(); ++it) {
					if(it->second){
						converter.setData(it->second);
						string feedxml = it->second->data->xml;
						int feedxml_size = feedxml.size();
						MCE_DEBUG("CallFeedCache::handle --> feedxml = " << feedxml << " size = " << feedxml_size);
						MessagePtr msg = converter.getToTalkMessage(_jid, true);
						if( msg ){
							mseqs.push_back(msg);
						}
					} else{
						MCE_DEBUG("CallFeedCache::handle --> can not get feed=" << it->first);
					}
				}
			}
		}
		*/
	}

}
