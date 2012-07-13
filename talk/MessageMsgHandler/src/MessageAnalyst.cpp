#include "MessageAnalyst.h"
#include "MessageTask.h"

using namespace com::xiaonei::talk;

void MessageAnalyst::handle(const JidPtr& jid, const string& msg, const string& ip) {
	StatFunc statF("MessageAnalyst::handle");
	xml_document_ptr doc = new xml_document();
	if (!doc->load(msg.c_str())) {
		MCE_WARN("MessageAnalyst::handle-->XMPP error xml" << msg);
		return;
	}
	const char* nodeName = doc->first_child().name();
	if (nodeName != NULL) {
	
			MessageTaskBuilder::instance().build(jid, doc, ip);
	
	}
}

//-------------------------------------------------
void MessageTaskBuilder::build(const JidPtr& jid, const xml_document_ptr& doc, const string& ip){
	TaskManager::instance().execute(new MessageTask(jid,doc, ip));
}



