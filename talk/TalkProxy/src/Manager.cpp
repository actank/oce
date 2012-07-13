#include "Manager.h"
#include "Server.h"

#include "ConnectionQuery.h"
#include "XmppTools.h"
#include <sys/resource.h>
#include "Action.h"
#include "MessageHolderAdapter.h"
#include "MessageType.h"
#include <util/cpp/String.h>

using namespace com::xiaonei::talk;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::talk::util;
using namespace ::talk::adapter;

//---------------------------------------------------------------------------
void Manager::express(const JidPtr& jid, const string& msg, const Ice::Current&) {
	MCE_DEBUG("to:"<<jidToString(jid)<<" msg:"<<msg);
	Server::instance().deliver(jid, msg);
}

void Manager::deliver(const common::MessageSeq& mseq, const Ice::Current&) {
	for (common::MessageSeq::const_iterator it = mseq.begin(); it != mseq.end(); ++it) {
		MCE_DEBUG("Manager::deliver --> toJid:"<<jidToString((*it)->to)<<"   msg:"<<(*it)->msg);
		if(Server::instance().deliver((*it)->to, (*it)->msg)){
			if((*it)->type == MESSAGE && (*it)->msgKey!=-1){
				MessageHolderAdapter::instance().send((*it)->to->userId, (*it)->msgKey);
			}
		}
	}
}

bool Manager::verify(const JidPtr& jid, const string& ticket,
		const Ice::Current&) {
	return Server::instance().verify(jid, ticket);
}

bool Manager::checkTicket(const JidPtr& jid, const string& ticket,
		const Ice::Current&) {
	return Server::instance().verify(jid, ticket);
}

string Manager::GetTicket(const JidPtr& jid,
        const Ice::Current&){
	return Server::instance().GetTicket(jid);
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

void Manager::kick(const JidPtr& jid, const Ice::Current&){
	MCE_INFO("Manager::kick --> " << jidToString(jid));
	return Server::instance().kick(jid);
}
