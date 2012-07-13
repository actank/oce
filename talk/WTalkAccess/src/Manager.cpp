#include "Manager.h"
#include "Server.h"

#include "ConnectionQuery.h"
#include "XmppTools.h"
#include <sys/resource.h>
#include "Action.h"
#include "Date.h"
#include "MessageType.h"
#include "ServiceI.h"
#include "util/cpp/String.h"
#include "XmppTools.h"
#include "MessageHolderAdapter.h"

using namespace ::talk::adapter;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::talk::util;
using namespace MyUtil;

void Manager::deliver(const common::MessageSeq& mseq, const Ice::Current&) {
	MCE_DEBUG("call Manager::batchDeliver  msg seq size:"<<mseq.size());

	for (common::MessageSeq::const_iterator it = mseq.begin(); it != mseq.end(); ++it) {

		if (((*it)->type == MESSAGE) || ((*it)->type == OFFLINE_MESSAGE) || ((*it)->type == NOTIFY_MESSAGE) || ((*it)->type == FEED_STATUS)) {

			xml_document doc;
			if (doc.load((*it)->msg.c_str())) {

				if ((*it)->type == MESSAGE) {

					xml_node msgNode = doc.child("message");
					xml_node bodyNode = msgNode.child("body");
					if (bodyNode.empty()) {
						return;
					}

					string fromname = ServiceI::instance().locateObject<ActiveUserPtr>(0,(*it)->from->userId)->name;
					StrUtil::string_replace(fromname, "&", "&amp;");
					//fromname = xmlEncode(fromname);
					string content = bodyNode.child_value();
					//content = xmlEncode(content);
					StrUtil::string_replace(content, "&", "&amp;");
					MCE_DEBUG("content : "<< content);
					ostringstream msg;
					msg << "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
							<<"<hi cmd='msg'>" << "<fromid>"<<(*it)->from->userId<<"</fromid>" << "<fromname>"
							<<fromname<<"</fromname>" << "<fromnick />"
							<< "<toid>"<<(*it)->to->userId<<"</toid>" << "<toname />"
							<< "<tonick />" << "<content>" <<content
							<<"</content>" << "<sendtime>"<<time(NULL)
							<<"</sendtime>" << "</hi>";
					string m = msg.str();
					MCE_INFO("Recv msg :"<<m);
					m.push_back('\0');

					MCE_DEBUG("start to send msg");
					if (Server::instance().deliver((*it)->to, m)) {
						MCE_DEBUG("sent msg");
						try {
							MessageHolderAdapter::instance().send((*it)->to->userId, (*it)->msgKey);
						} catch(...) {
							// nothing
						}
					};

				} else if ((*it)->type == OFFLINE_MESSAGE) {
					xml_node msgNode = doc.child("message");
					xml_node bodyNode = msgNode.child("body");
					if (bodyNode.empty()) {
						return;
					}
					xml_node delayNode = msgNode.child("delay");
					if (delayNode) {
						xml_attribute stampAttribute =
								delayNode.attribute("stamp");
						if (stampAttribute) {
							string stamp = stampAttribute.value();
							stamp = stamp.substr(0, 19); //2008-04-28T13:47:07
							if (stamp.length()>10) {
								stamp[10] = ' ';
								Date date(stamp);

								string fromname = ServiceI::instance().locateObject<ActiveUserPtr>(0,(*it)->from->userId)->name;
								string content = bodyNode.child_value();
								StrUtil::string_replace(fromname, "&", "&amp;");
								StrUtil::string_replace(content, "&", "&amp;");
								ostringstream msg;
								msg
										<< "<?xml version=\"1.0\" encoding=\"utf-8\"?><hi cmd='offline_msg'>"
										<< "<fromid>" <<(*it)->from->userId<<"</fromid>"
										<< "<fromname>"<<fromname
										<<"</fromname>" << "<fromnick />"
										<< "<toid>"<<(*it)->to->userId<<"</toid>"
										<< "<toname />" << "<tonick />"
										<< "<content>" <<content <<"</content>"
										<< "<sendtime>"<<date.time()
										<< "</sendtime>" << "</hi>";
								string m = msg.str();
								MCE_INFO("Recv offline msg : "<< m);
								m.push_back('\0');

								Server::instance().deliver((*it)->to, m);

							}

						}
					}

				} else if ((*it)->type == NOTIFY_MESSAGE) {
					//notify message
					xml_node messageNode = doc.child("message");
					string title = messageNode.child("subject").first_child().value();
					string content = messageNode.child("body").first_child().value();
					;

					StrUtil::string_replace(title, "&", "&amp;");
					StrUtil::string_replace(content, "&", "&amp;");

					ostringstream msg;
					msg <<"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
							<<"<hi cmd=\"notify\">" <<"<toid>"<<(*it)->to->userId<<"</toid>" <<"<title>"<<title
							<<"</title>" <<"<content>"<<content<<"</content>"
							<<"</hi>";
					string m = msg.str();
					MCE_INFO("Recv notify msg : "<< m);
					m.push_back('\0');
					if (Server::instance().deliver((*it)->to, m)) {
						MCE_DEBUG("Manager::deliver --> send notify OK");
					};
				} else {
					string m = "<?xml version=\"1.0\" encoding=\"utf-8\"?>" + doc.xml();
					m.push_back('\0');
					if (Server::instance().deliver((*it)->to, m)) {
						MCE_DEBUG("Manager::deliver --> send feed_status OK");
					};
				}

			}
		} else {
			MCE_DEBUG("Manager::deliver --> msg type:"<<(*it)->type);
		}
	}
}

//void Manager::event(const EventSeq& seq, const Ice::Current&) {
//	for (size_t i = 0; i < seq.size(); ++i) {
//		string title = xmlEncode(seq.at(i)->title);
//		string content = xmlEncode(seq.at(i)->content);
//		StrUtil::string_replace(title, "&", "&amp;");
//		StrUtil::string_replace(content, "&", "&amp;");
//
//		ostringstream msg;
//		msg <<"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
//				<<"<hi cmd=\"notify\">" <<"<toid>"<<seq.at(i)->to->userId<<"</toid>" <<"<title>"<<title<<"</title>"
//				<<"<content>"<<content<<"</content>" <<"</hi>";
//		string m = msg.str();
//		MCE_INFO("Recv offline msg : "<< m);
//		m.push_back('\0');
//		Server::instance().deliver(seq.at(i)->to, m);
//
//	}
//}
void Manager::express(const JidPtr& jid, const string& msg, const Ice::Current&) {

}
void Manager::leave(const JidPtr& jid, const Ice::Current&) {
	//Server::instance().leave(jid);
}
void Manager::relogin(const Ice::Current&) {

}

void Manager::broadcast(const string& xmlMsg, const Ice::Current&) {

}

bool Manager::indexExist(Ice::Long index, const Ice::Current&){
	return Server::instance().indexExist(index);
}
