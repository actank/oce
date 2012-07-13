#include "WEventI.h"
#include "MessageType.h"
#include "TalkDeliverAdapter.h"
#include "XmppTools.h"
#include "pugixml.hpp"
#include "QueryRunner.h"
//#include "PresenceAdapter.h"
#include "OnlineCenterAdapter.h"
#include "IceLogger.h"
#include "NotifyRplAdapter.h"
#include <boost/lexical_cast.hpp>
#include "XFeedNewsAdapter.h"
#include <util/cpp/String.h>
#include "FeedDBAdapter.h"

using namespace talk::wevent;
using namespace pugi;
using namespace MyUtil;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace ::xce::xfeed;
using namespace ::com::xiaonei::xce;
using namespace mop::hi::svc::dispatch;
using namespace talk::online::adapter;
//-----------------------------------------------
void MyUtil::initialize() {
	ServiceI::instance().getAdapter()->add(&WEventManagerI::instance(),
			ServiceI::instance().createIdentity("M", ""));
	PipePool::instance().initialize(new PipeFactoryI);
	TaskManager::instance().config(TASK_LEVEL_QUERY, ThreadPoolConfig(0, 10));
	string
			configFile =
					ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault(
							"Service." + ServiceI::instance().getName()
									+ ".ReplyConfigFile",
							"/opt/XNTalk/etc/ReplyView.xml");

	WEventManagerI::instance().loadReplyTemplate(configFile);
}

//-----------------------------------------------

QtfEscape::QtfEscape() {
	for (int i = 0; i < 128; i++)
		iscmd[i] = !(('a' <= i && i <= 'z') || ('A' <= i && i <= 'Z') || ('0'
				<= i && i <= '9'));
	for (int i = 128; i < 256; i++)
		iscmd[i] = false;
	std::string str = ".,;!?%()/<># ";
	for (size_t i = 0; i < str.size(); i++)
		iscmd[(size_t) str[i]] = false;
}

std::string QtfEscape::escape(const std::string& text) {
	std::ostringstream ss;
	for (size_t i = 0; i < text.size(); ++i) {
		if (iscmd[(unsigned char) text[i]])
			ss << '`';
		ss << text[i];
	}
	return ss.str();
}

//-----------------------------------------------
void NotifyPipe::handle(const ObjectSeq& objs) {

	MessageSeq msgs;
	IntSeq ids;
	for (size_t i = 0; i < objs.size(); ++i) {
		MessagePtr m = MessagePtr::dynamicCast(objs.at(i));
		ids.push_back(m->to->userId);
		msgs.push_back(m);
	}
	JidSeqMap jmap = OnlineCenterAdapter::instance().getUsersJids(ids);
	//if (jmap.empty()) {
	//	return;
	//}

	MessageSeq res;
	for (size_t i = 0; i < ids.size(); ++i) {
		
		bool friendApply = false;
		xml_document doc;
		if(!doc.load(msgs.at(i)->msg.c_str())){
			continue;
		}
		string tmp = doc.child("message").child("body").first_child().value();
		//string tmp = node.child("img").attribute("src").value();
		if(msgs.at(i)->to->userId == 238489851){
			MCE_INFO("@@ --> " << 238489851 << " " << tmp);
		}
		if(tmp.find("friends.gif")!=string::npos){
			friendApply = true;
		}
		
		JidSeqMap::iterator it = jmap.find(ids.at(i));
		if (it != jmap.end()) {
			for (size_t j = 0; j < it->second.size(); ++j) {
				if (it->second.at(j)->endpoint.length() > 3) {
					if (it->second.at(j)->endpoint[0] == 'W' && 
						it->second.at(j)->endpoint!="WTalkProxy1001") {

						MessagePtr m = new Message();
						m->from = new Jid();
						m->from->userId = -1;
						m->from->index = -1;
						m->to = it->second.at(j);
						m->msg = msgs.at(i)->msg;
						m->type = msgs.at(i)->type;
						res.push_back(m);
					}
				}
			}
		}else if(friendApply){
			//MCE_INFO("NotifyPipe::handle --> offline notify userid:" << msgs.at(i)->to->userId << " " << msgs.at(i)->msg);
			//Statement sql;
			//sql << "insert into offline_notify(userid, xml) values(" << msgs.at(i)->to->userId << ", " << mysqlpp::quote << msgs.at(i)->msg << ")";
			//QueryRunner("im", CDbWServer).schedule(sql);
		}
	}

	TalkDeliverAdapter::instance().deliver(res);
}

//-----------------------------------------------
void XFeedPipe::handle(const ObjectSeq& objs) {

	MessageSeq msgs;
	for (size_t x = 0; x < objs.size(); ++x) {
		xce::xfeed::XFeedLetterPtr xftl =
				xce::xfeed::XFeedLetterPtr::dynamicCast(objs.at(x));

		JidSeqMap jmap;
		try {
			jmap = OnlineCenterAdapter::instance().getUsersJids(xftl->target);
		} catch (Ice::Exception& e) {
			MCE_WARN("XFeedPipe::handle --> online center err:"<<e);
		} catch (std::exception& e) {
			MCE_WARN("XFeedPipe::handle --> online center err:"<<e.what());
		}
		if (jmap.empty()) {
			continue;
		}
		ostringstream endpoint;
		endpoint << xftl->stype;
		JidPtr jfrom;
		jfrom = new Jid();
		jfrom->userId = xftl->stype;
		jfrom->endpoint = "feed.talk.xiaonei.com";
		jfrom->index = -1;

		Ice::Long newId = 6000000000000000000 + ((Ice::Long) xftl->time)
				* 100000 + ((xftl->id & 0xffffffff) % 100000);

		ostringstream os;
		os << "<message type='common' from='" << jfrom->endpoint
				<< "@feed.talk.xiaonei.com'>" << "<subject>新鲜事</subject>"
				<< "<body/>" << "<xfeed xmlns='http://talk.xiaonei.com/xfeed'>"
				<< "<id>" << newId << "</id>" << "<actor>" << xftl->actor
				<< "</actor>" << "<time>" << xftl->time << "</time>"
				<< "<type>" << xftl->type << "</type>" << "<typen>" << getType(
				xftl->stype) << "</typen>" << "<stype>" << xftl->stype
				<< "</stype>" << "<stypen>" << getSType(xftl->type)
				<< "</stypen>" << "<resource>" << xftl->resource
				<< "</resource>" << "<merge>" << xftl->prop["merge"]
				<< "</merge>" << xftl->xml << getTemplate(xftl->stype)
				<< "</xfeed></message>";
				MCE_DEBUG("XFeedPipe::handle -> msg:" << os.str());
		xml_document doc;
		if (!doc.load(os.str().c_str())) {
			MCE_WARN("XFeedPipe::handle -> wrong: " << os.str());
			continue;
		}
		if(xftl->stype == 502){
			xml_node titleNode = doc.child("message").child("xfeed").child("f").child("title");
			if(titleNode){
				string tmp = titleNode.first_child().value();
				const char* MASK = "a href=&quot;http://status.renren.com/status?hotWord";
				int pos = 0;
				int limit = 0;
				for(; limit<20; limit++){
					int n = tmp.find(MASK, pos);
					if(n == string::npos){
						break;	
					}
					tmp.at(n) = 'b';
					pos = n + sizeof(MASK);
					int n1 = tmp.find("a&gt;", pos);
					if(n1 != string::npos){
						tmp.at(n1) = 'b';
					}
				}
				doc.child("message").child("xfeed").child("f").remove_child("title");
				titleNode = doc.child("message").child("xfeed").child("f").append_child();
				titleNode.set_name("title");
				titleNode.append_child(node_pcdata).set_value(tmp.c_str());
			}
		}

		xml_document doc1;
		if (xftl->stype == 502) {
			doc1.load(os.str().c_str());
			if (doc1.child("message").child("xfeed").child("tpl")) {
				doc1.child("message").child("xfeed").remove_child("tpl");
			}
		}
			////////////////////////////////
			//for test
			/*xce::feed::FeedItem item;
			item.feed = xftl->id;
			item.merge = xftl->actor;
			item.type = xftl->type;
			item.time = xftl->time;
			item.actor = 0;
			try{
				//MCE_INFO("@@@@ invoke FeedDB " << xftl->target.at(i));
				xce::feed::FeedDBDispatcherAdapter::instance().putBatch(item, xftl->target);
			}catch(Ice::Exception& e){
				MCE_WARN("@@@@ invoke FeedDB err, " << e);
			}*/
			/////////////////////////////////
	
		MCE_DEBUG("XFeedPipe::handle -> actor:"<<xftl->actor << " target.size:"
				<<xftl->target.size() <<" jmap.size:"<<jmap.size());
		if((xftl->stype==107 || xftl->stype==110 || xftl->stype==111)){
			xml_node node = doc.child("message").child("xfeed").child("f");
                	string shareId = node.child("sID").first_child().value();
	                string ownerId = node.child("from").child("fID").first_child().value();
        	        
			string l = string("http://share.renren.com/share/GetShare.do?") + "id=" + shareId + "&owner=" + ownerId;
		
			string name;
			if(xftl->stype==107){
				name = "link";
				node.child(name.c_str()).remove_child("eurl");
				xml_node tmp = node.child(name.c_str()).append_child();
                		tmp.set_name("eurl");
                		tmp.append_child(node_pcdata).set_value(l.c_str());
			
				node.child(name.c_str()).remove_child("url");
				tmp = node.child(name.c_str()).append_child();
                		tmp.set_name("url");
                		tmp.append_child(node_pcdata).set_value(l.c_str());

			}else if(xftl->stype==110){
				node.remove_child("eurl");
				xml_node tmp = node.append_child();
                		tmp.set_name("eurl");
                		tmp.append_child(node_pcdata).set_value(l.c_str());

				name = "video";
				node.child(name.c_str()).remove_child("eurl");
				tmp = node.child(name.c_str()).append_child();
                		tmp.set_name("eurl");
                		tmp.append_child(node_pcdata).set_value(l.c_str());
			
				node.child(name.c_str()).remove_child("url");
				tmp = node.child(name.c_str()).append_child();
                		tmp.set_name("url");
                		tmp.append_child(node_pcdata).set_value(l.c_str());
			}else if(xftl->stype==111){
				name = "music";
				node.child(name.c_str()).remove_child("eurl");
				xml_node tmp = node.child(name.c_str()).append_child();
                		tmp.set_name("eurl");
                		tmp.append_child(node_pcdata).set_value(l.c_str());
			
				node.child(name.c_str()).remove_child("url");
				tmp = node.child(name.c_str()).append_child();
                		tmp.set_name("url");
                		tmp.append_child(node_pcdata).set_value(l.c_str());
			}
		}
		for (size_t i = 0; i < xftl->target.size(); ++i) {
			
			JidSeqMap::iterator it = jmap.find(xftl->target.at(i));
			//MCE_DEBUG("XFeedPipe::handle -> target id:"<< xftl->target.at(i) << " online:"<< (it != jmap.end()));
			if (it == jmap.end()) {
				continue;
			}

			//临时过滤通过publisher上传照片的新鲜事
			if (701 == xftl->stype) {
				xml_node tmpNode = doc.child("message").child("xfeed").child(
						"f").child("photo");
				if (!tmpNode) {
					MCE_INFO("XFeedPipe::handle --> filter feeds with the stype of 701");
					continue;
				}
			}
			//临时测试
			//int userid = xftl->target.at(i);
			//xml_document doc1;
			/*if((xftl->stype == 602 || xftl->stype == 704 || xftl->stype == 705)){
				MCE_INFO("XFeedPipe::handle test@@@@ -> " << xftl->stype << " " << xftl->actor);
				if(userid!=238489851 && userid!=258133247 && userid!=248605995 && userid!=266418865 && userid!=228444877 && userid!=264191122){
					continue;
				}else{
					MCE_INFO("XFeedPipe::handle test@@@@ --> " << doc.xml());
				}
			}*/

			/*if((xftl->stype == 601 || xftl->stype == 701) && userid!=238489851 && userid!=258133247 && userid!=248605995 && userid!=266418865 && userid!=228444877 && userid!=264191122){
 				//doc1.load(os.str().c_str());
				if (doc.child("message").child("xfeed").child("reply_tpl")) {
					doc.child("message").child("xfeed").remove_child("reply_tpl");
				}
			}*/
			if(xftl->stype == 701){
				if(doc.child("message").child("xfeed").child("f").child("mSrc")){	
				}else{
					doc.child("message").child("xfeed").remove_child("stype");
					xml_node tmp = doc.child("message").child("xfeed").append_child();
					tmp.set_name("stype");
					tmp.append_child(node_pcdata).set_value("799");

					doc.child("message").child("xfeed").child("f").remove_child("type");
					tmp = doc.child("message").child("xfeed").child("f").append_child();
					tmp.set_name("type");
					tmp.append_child(node_pcdata).set_value("799");
					if(doc.child("message").child("xfeed").child("f").child("biz")){
						doc.child("message").child("xfeed").child("f").remove_child("biz");
					}
				}
			}
			MCE_INFO("XFeedPipe::handle --> deliver to im, stype:" << xftl->stype << " actor:" << xftl->actor << " to:" << xftl->target.at(i));
			//////////////////////////////////////////////////
			for (size_t j = 0; j < it->second.size(); ++j) {
				//				MCE_DEBUG("XFeedPipe::handle -> endpoint:"<< it->second.at(j)->endpoint << " msgsize:"<<msgs.size());
				if ((!it->second.at(j)->endpoint.empty())
						&& it->second.at(j)->endpoint[0] == 'T') {
					//test
					if(it->second.at(j)->endpoint == "TalkProxy1001" || it->second.at(j)->endpoint == "TalkProxy1002"){
						continue;
					}
					string jstr = jidToString(it->second.at(j));
					doc.child("message").append_attribute("to") = jstr.c_str();
					MessagePtr msg = new Message();
					msg->from = jfrom;
					msg->to = it->second.at(j);
					msg->type = SYSTEM_MESSAGE;
					msg->msg = doc.xml();
					msgs.push_back(msg);
				} else if ((!it->second.at(j)->endpoint.empty())
						&& it->second.at(j)->endpoint[0] == 'W' && xftl->stype
						== 502) {
					//改状态 FOR WEBPAGE
					string jstr = jidToString(it->second.at(j));
					doc1.child("message").append_attribute("to") = jstr.c_str();

					MessagePtr msg = new Message();
					msg->from = jfrom;
					msg->to = it->second.at(j);
					msg->type = FEED_STATUS;
					msg->msg = doc1.xml();
					msgs.push_back(msg);
				}
			}
		}

		//---for phone---
		if (xftl->stype == 102 || xftl->stype == 103 || xftl->stype == 502
				|| xftl->stype == 601 || xftl->stype == 701 || xftl->stype
				== 104) {

			xml_document xml;
			if (!xml.load(xftl->xml.c_str())) {
				continue;
			}
			os.str("");
			os << "<message>" << "<xfeed>" << "<feedid>" << xftl->id
					<< "</feedid>" << "<type>" << xftl->stype << "</type>"
					<< "<time>"
					<< xml.child("f").child("time").first_child().value()
					<< "</time>" << "<from>" << "<id>" << xml.child("f").child(
					"from").child("fID").first_child().value() << "</id>"
					<< "<name>"
					<< xml.child("f").child("from").child("fName").first_child().value()
					<< "</name>" << "</from>";

			if (xftl->stype == 102 || xftl->stype == 103 || xftl->stype == 104) {
				//os << "<sid>"<<xml.child("f").child("sID").first_child().value()<<"</sid>";
				if (xftl->stype == 102) {
					string
							data =
									xml.child("f").child("blog").child("url").first_child().value();
					os << "<id>" << ReplyPipe::getAid(data) << "</id>";
					os << "<title>" << xml.child("f").child("blog").child(
							"title").first_child().value() << "</title>";
					//<< "<friend>" << "<id>"
					//<<xml.child("f").child("friend").child("uID").first_child().value()<<"</id>" << "<name>"<<xml.child("f").child("friend").child("uName").first_child().value()<<"</name>" << "</friend>";
				} else if (xftl->stype == 103) {
					string
							data =
									xml.child("f").child("album").child("aUrl").first_child().value();
					os << "<id>" << ReplyPipe::getAid(data) << "</id>";
					data
							= xml.child("f").child("photo").child("pUrl").first_child().value();
					os << "<picid>" << ReplyPipe::getAid(data) << "</picid>";
					os << "<albumname>" << xml.child("f").child("album").child(
							"aName").first_child().value() << "</albumname>";
				} else if (xftl->stype == 104) {
					//""
					//os << "<picid>"<<ReplyPipe::getAid(pic)<<"</picid>"
					string
							data =
									xml.child("f").child("album").child("aUrl").first_child().value();
					os << "<id>" << ReplyPipe::getAid(data) << "</id>";
					os << "<albumname>" << xml.child("f").child("album").child(
							"title").first_child().value() << "</albumname>";
				}
				os << "<friend>" << "<id>"
						<< xml.child("f").child("friend").child("uID").first_child().value()
						<< "</id>" << "<name>"
						<< xml.child("f").child("friend").child("uName").first_child().value()
						<< "</name>" << "</friend>";

			} else if (xftl->stype == 502) {
				os << "<id>"
						<< xml.child("f").child("dID").first_child().value()
						<< "</id>" << "<content>" << xml.child("f").child(
						"title").first_child().value() << "</content>";
			} else if (xftl->stype == 601) {
				string
						blog =
								xml.child("f").child("blog").child("url").first_child().value();
				os << "<id>" << ReplyPipe::getAid(blog) << "</id>" << "<title>"
						<< xml.child("f").child("blog").child("title").first_child().value()
						<< "</title>";
			} else if (xftl->stype == 701) {

				string
						album =
								xml.child("f").child("album").child("aUrl").first_child().value();
				string
						pic =
								xml.child("f").child("photo").child("url").first_child().value();

				//os << "<album>"
				os << "<id>" << ReplyPipe::getAid(album) << "</id>"
						<< "<albumname>"
						<< xml.child("f").child("album").child("aName").first_child().value()
						<< "</albumname>" << "<picid>"
						<< ReplyPipe::getAid(pic) << "</picid>" << "<count>"
						<< xml.child("f").child("count").first_child().value()
						<< "</count>";
				//<< "</album>";

			}
			os << "</xfeed>" << "</message>";
		} else {
			continue;
		}
		//MCE_INFO("phone:"<< os.str());
		xml_document xml;
		if (!xml.load(os.str().c_str())) {
			continue;
		}
		for (size_t i = 0; i < xftl->target.size(); ++i) {
			JidSeqMap::iterator it = jmap.find(xftl->target.at(i));
			if (it == jmap.end()) {
				continue;
			}

			for (size_t j = 0; j < it->second.size(); ++j) {
				if ((!it->second.at(j)->endpoint.empty())
						&& it->second.at(j)->endpoint[0] == 'P') {
					if(it->second.at(j)->endpoint == "PTalkProxy1001"){
						continue;
					}
					//新鲜事 to phone
					string jstr = jidToString(it->second.at(j));
					//doc1.child("message").append_attribute("to") = jstr.c_str();

					MessagePtr msg = new Message();
					msg->from = jfrom;
					msg->to = it->second.at(j);
					msg->type = PHONE_NOTIFY;
					msg->msg = xml.xml();
					msgs.push_back(msg);
				}
			}
		}
		//---end for phone---

	}
	MCE_DEBUG("XFeedPipe::handle -->  deliver size:"<<msgs.size());
	if (!msgs.empty()) {
		MCE_DEBUG("XFeedPipe::handle --> deliver size:"<<msgs.size());
		TalkDeliverAdapter::instance().deliver(msgs);
	}
}

string XFeedPipe::getType(int stype) {
	ServiceI& service = ServiceI::instance();
	ostringstream key;
	key << "Service." << service.getBusiness() << ".XFeed.TypeName." << stype;
	return service.getCommunicator()->getProperties()->getPropertyWithDefault(
			key.str(), "");
}

string XFeedPipe::getSType(int type) {
	ServiceI& service = ServiceI::instance();
	ostringstream key;
	key << "Service." << service.getBusiness() << ".XFeed.STypeName." << type;
	return service.getCommunicator()->getProperties()->getPropertyWithDefault(
			key.str(), "");
}

string XFeedPipe::getTemplate(int stype) {
	ServiceI& service = ServiceI::instance();
	ostringstream key;
	key << "Service." << service.getBusiness() << ".XFeed.Template." << stype;
	return service.getCommunicator()->getProperties()->getPropertyWithDefault(
			key.str(), "");
}
//-----------------------------------------------

void ReplyPipe::handle(const ObjectSeq& seq) {

	MessageSeq msgs;
	for (size_t x = 0; x < seq.size(); ++x) {
		NotePtr note = NotePtr::dynamicCast(seq.at(x));

		JidSeq jseq = OnlineCenterAdapter::instance().getUserJids(note->toId);
		//MCE_DEBUG("from name:"<<note->fromName<< "  toid:"<<note->toId << " jid size:"<<jseq.size());
		if (jseq.empty()) {
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
				<< "</time>" << "<type>0</type>" << "<typen>" << getType()
				<< "</typen>" << "<stype>" << 0 - note->type << "</stype>"
				<< "<stypen>" << getSType(note) << "</stypen>" << "<resource>"
				<< note->resource << "</resource>" << "<f>" << "<fromid>"
				<< note->fromId << "</fromid>" << "<fromname>" << xmlEncode(
				note->fromName) << "</fromname>" << "<title>" << xmlEncode(
				note->title) << "</title>" << "<link>" << xmlEncode(note->link)
				<< "</link>" << "</f>" << getTemplate(note)
				<< "</xfeed></message>";

		xml_document doc;
		//MCE_DEBUG("from name:"<<note->fromName<< "  toid:"<<note->toId << " msg:"<<os.str());
		if (!doc.load(os.str().c_str())) {
			continue;
		}
		//MCE_DEBUG("ReplyPipe::handle -> toid:"<<note->toId<< "   msg:" << doc.xml());

		MCE_INFO("ReplyPipe::handle --> deliver to im, stype:" << note->type << " actor:" << note->fromId);
		if(note->toId == 257876975){
			MCE_INFO("@@@ -------> " << os.str());
		}
		for (size_t i = 0; i < jseq.size(); ++i) {
			if (jseq.at(i)->endpoint[0] == 'T'
				&& jseq.at(i)->endpoint != "TalkProxy1001") {
				doc.child("message").append_attribute("to") = jidToString(
						jseq.at(i)).c_str();
				MessagePtr msg = new Message();
				msg->from = jfrom;
				msg->to = jseq.at(i);
				msg->type = SYSTEM_MESSAGE;
				msg->msg = doc.xml();
				msgs.push_back(msg);
			}
		}

		//for phone
		os.str("");
		if (note->type == 1 || note->type == 4 || note->type == 16
				|| note->type == 32 || note->type == 128 || note->type == 512
				|| note->type == 65549 || note->type == 65552) {
			//获取主人id
			string ownerid;
			string par;
			if(note->type == 1 || note->type == 4 || note->type == 128 || note->type == 32){
				par = "owner=";
			}else if(note->type == 16 || note->type == 512 || note->type == 65549 || note->type == 65552){
				par = "id=";
			}else{
				ownerid = boost::lexical_cast<string>(note->ownerId);
			}
			if(!par.empty()){
				ownerid = getId(note->link, par);
			}
			//获取资源id（照片id 相册id 日志id）
			par = "";
			string source;
			if(note->type == 1 || note->type == 4 || note->type == 128 || note->type == 32){
				par = "id=";
			}else if(note->type == 65549 || note->type == 65552){
				par = "doingId=";
			}else{
				source = boost::lexical_cast<string>(note->resource);
			}
			if(!par.empty()){
				source = getId(note->link, par);
			}

			os << "<message ver=\"0.1\">" << "<reply>" << "<source>" << source
					<< "</source>" << "<fromid>" << note->fromId << "</fromid>"
					<< "<fromname>" << xmlEncode(note->fromName) << "</fromname>"
					<< "<type>" << note->type << "</type>" << "<time>"
					<< note->timestamp << "</time>" << "<ownerid>"
					<< ownerid << "</ownerid>" << "<title>" << xmlEncode(
					note->title) << "</title>";
			/*if (note->type == 1 || note->type == 4 || note->type == 32
					|| note->type == 128 || note->type == 65549 || note->type
					== 65552) {
				os << "<psource>" << getAid(note->link) << "</psource>";
				//os << "<url>"<<note->link<<"</url>";
			}*/
			os << "</reply>" << "</message>";

			//MCE_INFO("phone:"<< os.str());

			xml_document doc1;
			if (!doc1.load(os.str().c_str())) {
				continue;
			}

			for (size_t i = 0; i < jseq.size(); ++i) {
				if (jseq.at(i)->endpoint[0] == 'P') {
					if(jseq.at(i)->endpoint == "PTalkProxy1001"){
						continue;
					}
					MCE_INFO("ReplyPipe::handle --> deliver to " << jidToString(jseq.at(i)));
					doc.child("message").append_attribute("to") = jidToString(
							jseq.at(i)).c_str();
					MessagePtr msg = new Message();
					msg->from = jfrom;
					msg->to = jseq.at(i);
					msg->type = PHONE_NOTIFY;
					msg->msg = doc1.xml();
					msgs.push_back(msg);
				}
			}
		}
		//----end phone -----


	}

	if (!msgs.empty()) {
		MCE_DEBUG("ReplyPipe::handle --> msg.size:"<<msgs.size()<<"   msg:"
				<<msgs.at(0)->msg);
		TalkDeliverAdapter::instance().deliver(msgs);
	}

}

string ReplyPipe::getAid(const string& url) {
	if (url.empty()) {
		return "";
	}
	size_t begin = url.find("id=");
	if (begin != string::npos && begin + 3 < url.length()) {
		size_t end = url.find('&', begin + 3);
		if (end != string::npos) {
			return url.substr(begin + 3, end - begin - 3);
		}
	}
	return "";
}

string ReplyPipe::getId(const string& url, const string& name){
	if (url.empty()) {
		return "";
	}
	ostringstream os;
	size_t begin = url.find(name);
	if (begin != string::npos && begin + name.length() < url.length()) {
		//size_t end = url.find('&', begin + name.length());
		for(size_t i = begin+name.length(); i < url.length(); i++){
			if(url[i]>='0' && url[i]<='9'){
				os << url[i];
			}else{
				break;
			}
		}
	}
	return os.str();
}

string ReplyPipe::getType() {
	ServiceI& service = ServiceI::instance();
	ostringstream key;
	key << "Service." << service.getBusiness() << ".Reply.TypeName." << 0;
	return service.getCommunicator()->getProperties()->getPropertyWithDefault(
			key.str(), "");
}

string ReplyPipe::getSType(const mop::hi::svc::dispatch::NotePtr& note) {
	ServiceI& service = ServiceI::instance();
	ostringstream key;
	key << "Service." << service.getBusiness() << ".Reply.STypeName."
			<< note->type;
	return service.getCommunicator()->getProperties()->getPropertyWithDefault(
			key.str(), "");
}

string ReplyPipe::getTemplate(const mop::hi::svc::dispatch::NotePtr& note) {
	ReplyTemplatePtr r =
			WEventManagerI::instance().getReplyTemplate(note->type);
	if (!r) {
		return "<tpl/>";
	}

	ostringstream temp;
	temp << "<tpl>"
			<< "<icon>http://xnimg.cn/imgpro/icons/wall_post.gif</icon>"
			<< "<src>";
	if (r->title == "") {
		temp
				<< "[r100 [^http://xiaonei.com/getuser.do?id={{fromid:x-q}}^ {{fromname:x-h}}] "
				<< QtfEscape::instance().escape(r->prefix)
				<< " [^{{link:x-u}}^ {{title:x-h}}] "
				<< QtfEscape::instance().escape(r->sufix) << "]";
	} else {
		temp
				<< "[r100 [^http://xiaonei.com/getuser.do?id={{fromid:x-q}}^ {{fromname:x-h}}] "
				<< QtfEscape::instance().escape(r->prefix)
				<< " [^{{link:x-u}}^ "
				<< QtfEscape::instance().escape(r->title) << "] "
				<< QtfEscape::instance().escape(r->sufix) << "]";
	}
	temp << "</src>" << "</tpl>";
	return temp.str();
}

//-----------------------------------------------

void NoticePipe::handle(const ObjectSeq& objs) {

	MessageSeq msgs;
	IntSeq ids;

	for (size_t i = 0; i < objs.size(); ++i) {
		MessagePtr m = MessagePtr::dynamicCast(objs.at(i));
		msgs.push_back(m);
		ids.push_back(m->to->userId);
	}
	JidSeqMap jmap = OnlineCenterAdapter::instance().getUsersJids(ids);

	MessageSeq res;
	for (size_t i = 0; i < msgs.size(); ++i) {
		JidSeqMap::iterator it = jmap.find(msgs.at(i)->to->userId);
		if (it == jmap.end()) {
			continue;
		}
		for (size_t j = 0; j < it->second.size(); ++j) {
			if (it->second.at(j)->endpoint.size() > 1
					&& it->second.at(j)->endpoint[0] == 'T') {
				MessagePtr m = new Message();
				m->from = new Jid();
				m->from->userId = -1;
				m->from->endpoint = "appnotice.talk.xiaonei.com";
				m->from->index = -1;
				m->to = it->second.at(j);
				m->type = msgs.at(i)->type;
				m->msg = msgs.at(i)->msg;

				res.push_back(m);
			}

		}
	}

	if (!res.empty()) {
		MCE_DEBUG("Notice size:"<<res.size());
		TalkDeliverAdapter::instance().deliver(res);
	}
}

//-----------------------------------------------
void WEventManagerI::deliverNotify(Ice::Int userid, const string& title,
		const string& msg, const Ice::Current&) {
	MCE_INFO("WEventManagerI::deliverNotify");
	return;
	if(userid == 238489851){
		MCE_INFO("WEventManagerI::deliverNotify --> " << userid << " " << msg);
	}
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
}

void WEventManagerI::deliverXFeed(const xce::xfeed::XFeedLetterPtr& xftl,
		const Ice::Current&) {
	MCE_INFO("WEventManagerI::deliverXFeed --> userid:");
	return;
	MCE_DEBUG("WEventManagerI::deliverXFeed --> actor:"<<xftl->actor);
	// 把新鲜事发给他自己
	xftl->target.push_back(xftl->actor);

	PipePool::instance().push("XFeed", 0, xftl);
}

void WEventManagerI::loadOfflineXFeed(const JidPtr& jid, Ice::Long feedIndex,
		Ice::Long replyIndex, Ice::Int iqId, const Ice::Current&) {
	MCE_INFO("WEventManagerI::loadOfflineXFeed --> userid:"<<jid->userId);
	TaskManager::instance().execute(new LoadXFeedTask(jid, feedIndex,
			replyIndex, iqId));
	//TaskManager::instance().execute(new LoadReplyTask(jid->userId));
}

void WEventManagerI::deliverReply(const NotePtr& note, const Ice::Current&) {
	MCE_INFO("WEventManagerI::deliverReply --> userid:");
	return;
	PipePool::instance().push("Reply", 0, note);
}

void WEventManagerI::loadReplyTemplate(const string& configFile) {

	xml_document doc;

	if (doc.load_file(configFile.c_str())) {
		xml_node replysNode = doc.child("fangles").child("replys");
		for (xml_node reply = replysNode.child("reply"); reply; reply
				= reply.next_sibling("reply")) {
			ReplyTemplatePtr r = new ReplyTemplate();
			r->prefix = reply.attribute("prefix").value();
			r->title = reply.attribute("title").value();
			r->sufix = reply.attribute("sufix").value();
			string id = reply.attribute("id").value();
			try {
				int type = boost::lexical_cast<int>(id);
				_replyTempMap[type] = r;
			} catch (...) {
				MCE_WARN("WEventManagerI::loadReplyTemplate err");
				continue;
			}
		}
	} else {
		MCE_WARN("Load Reply Template File err");
	}
}

ReplyTemplatePtr WEventManagerI::getReplyTemplate(int type) {
	map<int, ReplyTemplatePtr>::iterator it = _replyTempMap.find(type);
	if (it == _replyTempMap.end()) {
		return 0;
	}
	return it->second;
}

void WEventManagerI::deliverAppNotice(Ice::Int toid, Ice::Int appid,
		const string& appName, const string& appIcon, const string& msg,
		const Ice::Current&) {
	MCE_INFO("WEventManagerI::deliverAppNotice --> userid:");
	//app通知过多，过滤掉
	return;
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

	/*
	 // 产品要求先去掉
	 string pager_body = "<img class=\"notifyico\" src=\"";
	 pager_body += appIcon;
	 pager_body += "\"/><div class=\"notifybody\">";
	 pager_body += msg;
	 pager_body += "</div>";

	 //pager_body html encoding
	 //MyUtil::StrUtil::string_replace(pager_body, "&", "&amp;");
	 //MyUtil::StrUtil::string_replace(pager_body, "<", "&lt;");
	 //MyUtil::StrUtil::string_replace(pager_body, ">", "&gt;");

	 deliverNotify(toid, appName, pager_body);
	 */
}

//--------------------------------------------

void LoadXFeedTask::handle() {
	MCE_INFO("LoadXFeedTask::handle --> userid:"<<_jid->userId);

	MessageSeq mseqs;
	mseqs.push_back(0);

	Ice::Long maxFeedId = _feedIndex;
	XFeedQueryPtr xq;
	try {
		xq = XFeedNewsAdapter::instance().getXFeedSeqByType(_jid->userId,
				IntSeq(), true, 0, 100);
	} catch (Ice::Exception& e) {
		MCE_WARN("LoadXFeedTask::handle --> call XFeedNewsAdapter getXFeed err:"<<e);
	}
	if (xq) {
		map<Ice::Long, XFeedContentPtr> sortmap;

		for (size_t i = 0; i < xq->data.size(); ++i) {
			Ice::Long newId = 6000000000000000000
					+ ((Ice::Long) xq->data.at(i)->time) * 100000
					+ ((xq->data.at(i)->id & 0xffffffff) % 100000);
			if (newId > _feedIndex) {
				sortmap[newId] = xq->data.at(i);
			}
		}
		if (!sortmap.empty()) {
			maxFeedId = sortmap.rbegin()->first;
		}
		for (map<Ice::Long, XFeedContentPtr>::iterator it = sortmap.begin(); it
				!= sortmap.end(); ++it) {

			JidPtr jfrom = new Jid;
			jfrom->userId = it->second->stype;
			jfrom->endpoint = "feed.talk.xiaonei.com";
			jfrom->index = -1;

			//maxFeedId = it->second->id > maxFeedId ? it->second->id : maxFeedId;


			ostringstream os;
			os << "<message type='common' from='" << jfrom->userId
					<< "@feed.talk.xiaonei.com'>" << "<subject>新鲜事</subject>"
					<< "<body/>"
					<< "<xfeed xmlns='http://talk.xiaonei.com/xfeed'>"
					<< "<id>" << it->first << "</id>" << "<actor>1</actor>"
					<< "<time>" << it->second->time << "</time>" << "<type>"
					<< it->second->type << "</type>" << "<typen>"
					<< XFeedPipe::getType(it->second->stype) << "</typen>"
					<< "<stype>" << it->second->stype << "</stype>"
					<< "<stypen>" << XFeedPipe::getSType(it->second->type)
					<< "</stypen>" << "<resource>null</resource>"
					<< it->second->xml << XFeedPipe::getTemplate(
					it->second->stype) << "</xfeed></message>";

			xml_document doc;
			if (!doc.load(os.str().c_str())) {
				MCE_WARN("LoadXFeedTask::handle --> msg is not a xml :"
						<< os.str());
				continue;
			}
			if((it->second->stype==107 || it->second->stype==110 || it->second->stype==111)){
				xml_node node = doc.child("message").child("xfeed").child("f");
                		string shareId = node.child("sID").first_child().value();
	                	string ownerId = node.child("from").child("fID").first_child().value();
        	        
				string l = string("http://share.renren.com/share/GetShare.do?") + "id=" + shareId + "&owner=" + ownerId;
		
				string name;
				if(it->second->stype==107){
					name = "link";
					node.child(name.c_str()).remove_child("eurl");
					xml_node tmp = node.child(name.c_str()).append_child();
                			tmp.set_name("eurl");
                			tmp.append_child(node_pcdata).set_value(l.c_str());
			
					node.child(name.c_str()).remove_child("url");
					tmp = node.child(name.c_str()).append_child();
                			tmp.set_name("url");
                			tmp.append_child(node_pcdata).set_value(l.c_str());

				}else if(it->second->stype==110){
					node.remove_child("eurl");
					xml_node tmp = node.append_child();
	                		tmp.set_name("eurl");
	                		tmp.append_child(node_pcdata).set_value(l.c_str());
	
					name = "video";
					node.child(name.c_str()).remove_child("eurl");
					tmp = node.child(name.c_str()).append_child();
	                		tmp.set_name("eurl");
	                		tmp.append_child(node_pcdata).set_value(l.c_str());
				
					node.child(name.c_str()).remove_child("url");
					tmp = node.child(name.c_str()).append_child();
	                		tmp.set_name("url");
	                		tmp.append_child(node_pcdata).set_value(l.c_str());
				}else if(it->second->stype==111){
					name = "music";
					node.child(name.c_str()).remove_child("eurl");
					xml_node tmp = node.child(name.c_str()).append_child();
	                		tmp.set_name("eurl");
	                		tmp.append_child(node_pcdata).set_value(l.c_str());
				
					node.child(name.c_str()).remove_child("url");
					tmp = node.child(name.c_str()).append_child();
	                		tmp.set_name("url");
	                		tmp.append_child(node_pcdata).set_value(l.c_str());
				}
			}
			if(it->second->stype == 502){
				xml_node titleNode = doc.child("message").child("xfeed").child("f").child("title");
				if(titleNode){
					string tmp = titleNode.first_child().value();
					const char* MASK = "a href=&quot;http://status.renren.com/status?hotWord";
					int pos = 0;
					int limit = 0;
					for(; limit<20; limit++){
						int n = tmp.find(MASK, pos);
						if(n == string::npos){
							break;	
						}
						tmp.at(n) = 'b';
						pos = n + sizeof(MASK);
						int n1 = tmp.find("a&gt;", pos);
						if(n1 != string::npos){
							tmp.at(n1) = 'b';
						}
					}
					doc.child("message").child("xfeed").child("f").remove_child("title");
					titleNode = doc.child("message").child("xfeed").child("f").append_child();
					titleNode.set_name("title");
					titleNode.append_child(node_pcdata).set_value(tmp.c_str());
				}
			}

			if (701 == it->second->stype) {
				xml_node tmpNode = doc.child("message").child("xfeed").child(
						"f").child("photo");
				if (!tmpNode) {
					MCE_INFO("XFeedPipe::handle --> filter feeds with the stype of 701");
					continue;
				}
			}
			if(701 == it->second->stype){
				if(doc.child("message").child("xfeed").child("f").child("mSrc")){	
				}else{
					doc.child("message").child("xfeed").remove_child("stype");
					xml_node tmp = doc.child("message").child("xfeed").append_child();
					tmp.set_name("stype");
					tmp.append_child(node_pcdata).set_value("799");

					doc.child("message").child("xfeed").child("f").remove_child("type");
					tmp = doc.child("message").child("xfeed").child("f").append_child();
					tmp.set_name("type");
					tmp.append_child(node_pcdata).set_value("799");
					if(doc.child("message").child("xfeed").child("f").child("biz")){
						doc.child("message").child("xfeed").child("f").remove_child("biz");
					}
				}
			}
	
			MessagePtr msg = new Message();
			msg->from = jfrom;
			msg->to = _jid;
			msg->type = SYSTEM_MESSAGE;
			msg->msg = doc.xml();
			//MCE_DEBUG("LoadXFeedTask::handle --> msg:"<< msg->msg);
			mseqs.push_back(msg);
		}
	}

	//=========================
	// relpy
	NoteSeq noteSeq;
	try {
		noteSeq = NotifyRplAdapter::instance().getAll(_jid->userId, -1, true,
				0, -1);
	} catch (Ice::Exception& e) {
		MCE_WARN("LoadXFeedTask::handle --> call NotifyRplAdapter err:"<< e);
	}

	MCE_INFO("LoadXFeedTask::handle --> userid:"<<_jid->userId
			<< "   reply size:" << noteSeq.size());
	Ice::Long maxReplyId = _replyIndex;
	for (size_t i = 0; i < noteSeq.size(); ++i) {
		maxReplyId = maxReplyId > noteSeq.at(i)->noteId ? maxReplyId
				: noteSeq.at(i)->noteId;
	}

	//=========================


	MessagePtr iqMsg = new Message();
	iqMsg->from = _jid;
	iqMsg->to = _jid;
	iqMsg->type = IQ_RESULT;

	ostringstream msg;
	msg << "<iq type='result' id='" << _iqId << "'>"
			<< "<query xmlns='http://talk.xiaonei.com/xfeed'>";
	if ((!xq) || xq->data.empty()) {
		msg << "<xfeed end='" << _feedIndex << "'/>";
	} else {
		msg << "<xfeed end='" << maxFeedId << "'/>";
	}
	msg << "<reply end='" << maxReplyId << "'/>" << "</query>" << "</iq>";

	iqMsg->msg = msg.str();
	mseqs.at(0) = iqMsg;
	MCE_DEBUG("LoadXFeedTask::handle --> iq msg:"<<msg.str());

	if (!mseqs.empty()) {
		MCE_INFO("LoadXFeedTask --> userid:"<<_jid->userId<<" to deliver size:"
				<<mseqs.size());
		TalkDeliverAdapter::instance().deliver(mseqs);
	}
	//========================
	//  reply
	//for (size_t i = 0; i < noteSeq.size(); ++i) {
	MCE_INFO("LoadXFeedTask --> userid:"<<_jid->userId<<" to pipe size:"
			<<noteSeq.size());
	//PipePool::instance().push("Reply", 0, noteSeq.at(i));
	loadReply(noteSeq);
	//}
}

void LoadXFeedTask::loadReply(const NoteSeq& seq) {
	MessageSeq msgs;
	for (size_t x = 0; x < seq.size(); ++x) {
		NotePtr note = seq.at(x);

		//JidSeq jseq = OnlineCenterAdapter::instance().getUserJids(note->toId);
		//MCE_DEBUG("from name:"<<note->fromName<< "  toid:"<<note->toId << " jid size:"<<jseq.size());
		//if (jseq.empty()) {
		//	continue;
		//}
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
				<< "</time>" << "<type>0</type>" << "<typen>"
				<< ReplyPipe::getType() << "</typen>" << "<stype>" << 0
				- note->type << "</stype>" << "<stypen>"
				<< ReplyPipe::getSType(note) << "</stypen>" << "<resource>"
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
		//MCE_DEBUG("ReplyPipe::handle -> toid:"<<note->toId<< "   msg:" << doc.xml());

		//for (size_t i = 0; i < jseq.size(); ++i) {
		//if (jseq.at(i)->endpoint[0] == 'W') {
		//					if (xftl->actor == 80578957) {
		//						MCE_DEBUG("XFeedPipe::handle -> ignore WTalkProxy: "
		//								<< xftl->target.at(i) << ", " << it->second.at(j)->endpoint << ", msg: " << doc.xml());
		//					}
		//} else if (jseq.at(i)->endpoint[0] == 'T') {
		doc.child("message").append_attribute("to") = jidToString(_jid).c_str();
		MessagePtr msg = new Message();
		msg->from = jfrom;
		msg->to = _jid;
		msg->type = SYSTEM_MESSAGE;
		msg->msg = doc.xml();
		msgs.push_back(msg);
		//					if (xftl->actor == 80578957) {
		//						MCE_DEBUG("XFeedPipe::handle -> deliver: " << msg->to
		//								<< ", msg: " << msg->msg);
		//					}
		//}
		//}

	}

	if (!msgs.empty()) {
		MCE_DEBUG("ReplyPipe::handle --> msg.size:"<<msgs.size()<<"   msg:"
				<<msgs.at(0)->msg);
		TalkDeliverAdapter::instance().deliver(msgs);
	}

}
//void LoadReplyTask::handle() {
//	NoteSeq seq = NotifyRplAdapter::instance().getAll(_userId, -1, true, 0, -1);
//	MCE_DEBUG("LoadReplyTask::handle --> userid:"<<_userId << "   reply size:"
//			<< seq.size());
//	for (size_t i = 0; i < seq.size(); ++i) {
//		PipePool::instance().push("Reply", 0, seq.at(i));
//	}
//}

