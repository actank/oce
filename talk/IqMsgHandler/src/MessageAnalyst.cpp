#include "MessageAnalyst.h"
#include "IqTask.h"
#include "IqHandlerI.h"
#include "XmppTools.h"
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::util;
void MessageAnalyst::handle(const JidPtr& jid, const string& msg) {
	StatFunc statF("MessageAnalyst::handle");
	xml_document_ptr doc = new xml_document();
	if (!doc->load(msg.c_str())) {
		return;
	}
	const char* nodeName = doc->first_child().name();
	if (nodeName != NULL) {
		IqTaskBuilder::instance().build(jid, doc);
	}
}
//-------------------------------------------------
void IqTaskBuilder::build(const JidPtr& jid, const xml_document_ptr& doc) {

	StatFunc statF("IqTaskBuilder::build");
	string type = doc->child("iq").attribute("type").value();
	if (type == "get") {
		int level = 0;
		for (xml_node n = doc->child("iq").first_child(); n; n = n.next_sibling()) {
			if (strcmp(n.name(), "query") == 0) {
				string xmlns = n.attribute("xmlns").value();
				if (xmlns == "http://talk.renren.com/xfeed/reply" || xmlns == "http://talk.xiaonei.com/xfeed/reply" || 
						xmlns == "http://talk.xiaonei.com/xfeed" || xmlns == "http://talk.renren.com/xfeed" ||
						xmlns == "http://talk.renren.com/xfeed/replycount" || xmlns == "http://talk.xiaonei.com/xfeed/replycount" || 
            xmlns == "http://talk.renren.com/webticket"  ||  xmlns == "http://talk.xiao.com/webticket"
				){
					level = 100;
				}
			}
			else if (strcmp(n.name(), "vCard") == 0) {
				level = 111;
			}
		}
		TaskManager::instance().execute(new IqGetTask(level, jid,doc));
	} else if (type == "set") {
		TaskManager::instance().execute(new IqSetTask(jid,doc));
	} else if (type == "result") {
		TaskManager::instance().execute(new IqResultTask(jid,doc));
	} else if (type == "error") {
		TaskManager::instance().execute(new IqErrorTask(jid,doc));
	}
}

//-------------------------------------------------


