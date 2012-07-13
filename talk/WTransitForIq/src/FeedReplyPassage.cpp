#include "FeedReplyPassage.h"
#include "FeedPassage.h"



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
//#include "FeedDBAdapter.h"
#include "FeedContentAdapter.h"
#include "TransitAider.h"
#include "WTransitI.h"

using namespace talk::wtransit;
using namespace pugi;
using namespace MyUtil;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace ::xce::feed;
using namespace ::com::xiaonei::xce;
//using namespace mop::hi::svc::dispatch;
using namespace talk::online::adapter;





/////////////////////////////FeedReplyPipe////////////////////////////////////////////

void FeedReplyPipe::handle(){
	FeedReplyToMessagConverter converter;
	TimeStat ts;
	float totalwait = 0.0;
	FeedReplyWithTargetSeq objs;
	WTransitManagerI::instance().SwapFeedReplySeq(objs);
	if(objs.empty()){
		return;
	}
	for(size_t x = 0; x < objs.size(); ++x){
		//FeedReplyWithTargetPtr reply = FeedReplyWithTargetPtr::dynamicCast(objs.at(x));
		FeedReplyWithTargetPtr reply = objs.at(x);
		totalwait += reply->latets.getTime();
	}
	for (size_t x = 0; x < objs.size(); ++x) {
	  MessageSeq msgs;
		//FeedReplyWithTargetPtr reply = FeedReplyWithTargetPtr::dynamicCast(objs.at(x));
		FeedReplyWithTargetPtr reply = objs.at(x);
		converter.setData(reply->replyData);
		if(reply->replyData->actor == 257876975 || reply->replyData->actor == 238489851){
			//MCE_DEBUG(reply->replyData->actor << " " << reply->replyData->xml);
		}
//		MyUtil::IntSeq target = reply->getTarget();
		MCE_DEBUG(" FeedReplyPipe::handle --> feed->target.size = "<< reply->target.size());
		JidSeqMap jmap;
	
		try {
			jmap = OnlineCenterAdapter::instance().getUsersJids(reply->target);
		} catch (Ice::Exception& e) {
			MCE_WARN("FeedReplyPipe::handle-->OnlineCenterAdapter::getUsersJids-->" << reply->replyData->actor << " " << reply->target.size() << " "<<e);
		} catch (std::exception& e) {
			MCE_WARN("FeedReplyPipe::handle --> online center err: " << reply->target.size() << " "<<e.what());
		}
		if (jmap.empty()) {
			MCE_DEBUG("FeedReplyPipe::handle --> no friend online  ");
			continue;
		}
		MCE_DEBUG("FeedReplyPipe::handle --> jmap.size = "<<jmap.size());
		
		for (size_t i = 0; i < reply->target.size(); ++i) {
			JidSeqMap::const_iterator it = jmap.find(reply->target.at(i));
			if (it == jmap.end()) {
				MCE_DEBUG("FeedReplyPipe::handle --> can not find jmap feed->target(i)="<<reply->target.at(i));
				continue;
			}
			for (size_t j = 0; j < it->second.size(); ++j) {
				MessagePtr msg;
				if ( (!it->second.at(j)->endpoint.empty()) ){
					if( it->second.at(j)->endpoint[0] == 'T' ){
					  //MCE_DEBUG("FeedReplyPipe::handle-->SEND REPLY source=" << reply->replyData->source << " actor=" << reply->replyData->actor << " replyid=" << reply->replyData->replyId << " to userid=" << it->second.at(j)->userId);
						msg = converter.getToTalkMessage(it->second.at(j));
					}
					if(msg){
						MCE_DEBUG("FeedReplyPipe::handle send message  : "<<msg->msg);
						//if(it->second.at(j)->endpoint=="TalkProxy1001" || it->second.at(j)->endpoint=="TalkProxy1002"
						//	|| it->second.at(j)->endpoint=="PTalkProxy1001"){
							//MCE_DEBUG("FeedPipe::handle --> deliver to " << jidToString(it->second.at(j))									<< " ---- " << msg->msg);
							msgs.push_back(msg);
						//}
					}
				}
			}
		}
		if (!msgs.empty()) {
			try{
				TalkDeliverAdapter::instance().deliver(msgs);
			}catch(Ice::Exception& e){
				MCE_WARN("FeedReplyPipe::handle --> TalkDeliverAdapter::deliver--> error : " << e);
			}
			/*
			MCE_DEBUG("FeedReplyPipe::handle --> deliver size:"<<msgs.size() << " onlinesize:" << jmap.size() << " " << reply->target.size() << " actor:" << reply->replyData->actor);
			MessageSeq split_seq;
			int len_msgs = msgs.size();
			int sl = 0;
			int count = 0;
			for(size_t i = 1;i <= len_msgs; ++i)
			{
				split_seq.push_back(msgs[i-1]);
				if(i%200 == 0 || i == len_msgs){
					try {
						count++;
						TalkDeliverAdapter::instance().deliver(split_seq);
						sl += split_seq.size();
					} catch (Ice::Exception& e){
						MCE_WARN("FeedReplyPipe::handle-->TalkDeliverAdapter::deliver-->" << "size:" << split_seq.size() << "onlinesize:" << jmap.size() << " " << reply->target.size() << " actor:" << reply->replyData->actor << " " << e);
					}
					split_seq.clear();
				}
			}
			//MCE_DEBUG("FeedReplyPipe::handle--> len:" << len_msgs << " send len:" << sl << " count:" << count);
			*/
		}
	}
	MONITOR("SignForTimeuse::WTransit::FeedReplyPipe::handle-->" << ts.getTime() << "|" << "count" << objs.size());
}

//////////////////////////////FeedReplyWithTarget///////////////////////////////////////////////////



string FeedReplyToMessagConverter::_getType(int stype){
	ServiceI& service = ServiceI::instance();
	ostringstream key;
	key << "Service." << service.getBusiness() << ".XFeed.TypeName." << stype;
	string ret = WTransitManagerI::instance().GetServerProperties(key.str());
	return ret;
	//return service.getCommunicator()->getProperties()->getPropertyWithDefault( key.str(), "");
}


string FeedReplyToMessagConverter::_getSType(int type){
	ServiceI& service = ServiceI::instance();
	ostringstream key;
	key << "Service." << service.getBusiness() << ".XFeed.STypeName." << type;
	string ret = WTransitManagerI::instance().GetServerProperties(key.str());
	return ret;
	//return service.getCommunicator()->getProperties()->getPropertyWithDefault( key.str(), "");
}


string FeedReplyToMessagConverter::_getTemplate(int stype){
	ServiceI& service = ServiceI::instance();
	ostringstream key;
	key << "Service." << service.getBusiness() << ".XFeed.Reply.Template." << stype;
	//string ret = service.getCommunicator()->getProperties()->getPropertyWithDefault(key.str(), "");
	string ret = WTransitManagerI::instance().GetServerProperties(key.str());
	return ret;
}


void FeedReplyToMessagConverter::setData(const ReplyDataPtr& replyData){
	_jfrom = new Jid();
	_jfrom->userId = replyData->stype;
	_jfrom->endpoint = "feed.talk.xiaonei.com";
	_jfrom->index = -1;
	_replyData = replyData;
	_talkMsg.clear();
//	_toTalkMasg.load("");
	//switch(replyData->stype){
	//case 601: _replyType = 602;break;//发日志 601   日志回复 602 
	//case 502: _replyType = 507;break;//修改状态 502 状态回复 506
	//case 701: _replyType = 704;break;//上传照片 701 上传单张照片回复 704
	//}

}
// JidPtr FeedReplyWithTarget::_getJfrom(){
// 	if( !jfrom ){
// 		jfrom = new Jid();
// 		jfrom->userId = replyData->stype;
// 		jfrom->endpoint = "feed.talk.xiaonei.com";
// 		jfrom->index = -1;
// 	}
// 	return jfrom;
// }




// MessagePtr FeedReplyWithTarget::getToWebPagerMessage(  const JidPtr& to ){
// 	return 0;
// }



MessagePtr FeedReplyToMessagConverter::getToTalkMessage(  const JidPtr& to  ){
	MCE_DEBUG("call FeedReplyToMessagConverter::getToTalkMessage actor="<<_replyData->actor );
	if(!_jfrom){
		MCE_DEBUG("FeedReplyToMessagConverter::getToTalkMessage --> should call setData first" );
		return 0;
	}
	if(!_talkMsg.empty()){
		MessagePtr msg = new Message();
		msg->from = _jfrom;
		msg->to = to;
		msg->type = SYSTEM_MESSAGE;
		msg->msg = _talkMsg;
		return msg;
	}

	MCE_DEBUG("FeedReplyToMessagConverter::getToTalkMessage --> msg="<<_replyData->xml.c_str());
	xml_document xml;
	if (!xml.load(_replyData->xml.c_str())) {
		MCE_DEBUG("FeedReplyToMessagConverter::getToTalkMessage --> load xml error:  xml=\n"<<_replyData->xml.c_str());
		return 0;
	}
	xml_node node;
	xml_node node2;
	ostringstream os1;

	//node = xml.child("f").child("reply").child("from");
	//xml.child("f").append_copy(node);
	node = xml.child("f").append_child();
	node.set_name("from");
	node = node.append_child();
	node.set_name("fID");
	node2 = xml.child("f").child("reply").child("from");
	node.append_child(node_pcdata).set_value(node2.child("id").first_child().value().c_str());

	string logid = node2.child("id").first_child().value();
	if( logid == "" || logid == "0"){
	  //MCE_DEBUG("FeedReplyToMessagConverter::getToTalkMessage --> FROM ID = 0  _replyData->xml = " << _replyData->xml.c_str() << " -------------- xml = " << xml.xml());
	}

	node = xml.child("f").child("from").append_child();
	node.set_name("fName");
	node.append_child(node_pcdata).set_value(node2.child("name").first_child().value().c_str());



	node = xml.child("f").append_child();
	node.set_name("count");
	os1<<_replyData->replyCount;
	node.append_child(node_pcdata).set_value(os1.str().c_str());
	os1.str("");
	
	node = xml.child("f").append_child();
	node.set_name("biz");
	os1<<_replyData->source<<"_"<<_replyData->stype<<"_"<<_replyData->actor;
	node.append_child(node_pcdata).set_value(os1.str().c_str());
	os1.str("");
	
	node = xml.child("f").append_child();
	node.set_name("type");
	os1<<_replyData->stype;
	node.append_child(node_pcdata).set_value(os1.str().c_str());
	os1.str("");

	node = xml.child("f").child("reply");
	node2 = node.append_child();
	node2.set_name("ui");
	os1<<xml.child("f").child("reply").child("from").child("id").first_child().value();
	node2.append_child(node_pcdata).set_value(os1.str().c_str());
	os1.str("");

	node2 = node.append_child();
	node2.set_name("rid");
	os1<<_replyData->replyId;
	node2.append_child(node_pcdata).set_value(os1.str().c_str());
	os1.str("");

 
  string replyString;
  if(_replyData->stype == 502){
	  replyString = xml.child("f").child("reply").child("im").child("body").first_child().value();
  }else{
	  replyString = xml.child("f").child("reply").child("body").first_child().value();
	  if(!replyString.empty()){
		  replyString = replyString.substr(6,replyString.length()-12);
	  }
	  node = xml.child("f").child("reply").child("im");
	  string tmp = node.child("body").first_child().value();
	  node.remove_child("body");
	  node2 = node.append_child();
	  node2.set_name("body");
	  node2.append_child(node_pcdata).set_value( replyString.c_str() );
  } 
	node = xml.child("f").child("reply");
	node2 = node.append_child();
	node2.set_name("hcon");
	node2.append_child(node_pcdata).set_value( replyString.c_str() );
//	xml.child("f").child("reply").child("im").child("body").set_value(replyString.substr(1,replyString.length()-2).c_str());
	
	ostringstream os;
	os << "<message type='common' from='" << _jfrom->endpoint << "@feed.talk.xiaonei.com'>"
	   << "<xfeed xmlns='http://talk.renren.com/xfeed'>"
	   << "<id>" << _replyData->replyId << "</id>"
	   << "<actor>" << _replyData->actor << "</actor>"
	   << "<time>" << _replyData->time << "</time>"
	   << "<type>" << (int)(_replyData->stype/100) << "</type>"
	   <<"<typen/>"
	   << "<stype>" << _replyData->stype << "</stype>"
	   <<"<stypen/>"
	   << "<resource>" << _replyData->source << "</resource>"
	   << xml.xml()
	   << _getTemplate(_replyData->stype)
	   << "</xfeed></message>";
	
	MCE_DEBUG("FeedPipe::buildTalkMessage -> actor "<<_replyData->actor<<" msg:" << os.str());
	xml_document toTalkMasg;
	if (!toTalkMasg.load(os.str().c_str())) {
		MCE_WARN("FeedPipe::buildTalkMessage -> actor="<<_replyData->actor<<" wrong: " << os.str());
		return 0;
	}
	_talkMsg = toTalkMasg.xml();
//	string str = jidToString(to);
	MessagePtr msg = new Message();
	msg->from = _jfrom;
	msg->to = to;
	msg->type = SYSTEM_MESSAGE;
	msg->msg = _talkMsg;
	return msg;
}

