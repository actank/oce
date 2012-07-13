#include "DeliverI.h"
#include "ConnectionQuery.h"
#include "TalkProxy.h"
#include "PresenceAdapter.h"
#include "Date.h"
#include "XmppTools.h"
#include "TaskManager.h"
#include "TalkRightAdapter.h"
#include "MessageType.h"

//#include "WebEventAdapter.h"
using namespace com::xiaonei::talk;
using namespace com::xiaonei::xce;
using namespace mop::hi::svc::adapter;
using namespace MyUtil;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk::adapter;
using namespace talk::mucdeliver;
//---------------------------------------------------------------------------
void MyUtil::initialize() {
	//DeliverManagerI* manager = new DeliverManagerI;
	ServiceI::instance().getAdapter()->add(&MucDeliverManagerI::instance(), ServiceI::instance().createIdentity("M", ""));

//	ConnectionPoolManager::instance().initialize(ServiceI::instance().getAdapter());

	PipePool::instance().initialize(new PipeFactoryI);
}

//--------------------------------------------------------------------------------
void PostPipe::handle(const ObjectSeq& seq) {

	if (seq.empty()) {
		return;
	}
	MessageSeq mseq;
	for (size_t i = 0; i < seq.size(); ++i) {
		mseq.push_back(MessagePtr::dynamicCast(seq.at(i)));
	}
	try {
		MucDeliverManagerI::instance().getProxyServerOneway(mseq.at(0)->to)->deliver(mseq);
	} catch(Ice::Exception& e) {
		MCE_WARN("PostPipe::handle --> deliver err:"<<e<<"  to endpoint:"<< mseq.at(0)->to);
	}
	MCE_DEBUG(__FUNCTION__ << " --> deliver size="<<mseq.size());
}

//--------------------------------------------------------------------------------
DeliverInterfacePrx MucDeliverManagerI::getProxyServer(const JidPtr& ptr) {
	IceUtil::Mutex::Lock lock(_mutex);
	map<string,DeliverInterfacePrx>::iterator it = _manager.find(ptr->endpoint);
	if (it != _manager.end()) {
		return it->second;
	}
	Ice::ObjectPrx prx = ServiceI::instance().getAdapter()->getCommunicator()->stringToProxy("M@"+ptr->endpoint);
	DeliverInterfacePrx dPrx = DeliverInterfacePrx::uncheckedCast(prx);
	_manager[ptr->endpoint] = dPrx;
	return dPrx;
}
DeliverInterfacePrx MucDeliverManagerI::getProxyServerOneway(const JidPtr& ptr) {
	IceUtil::Mutex::Lock lock(_mutex);

	map<string,DeliverInterfacePrx>::iterator it =
			_managerOneway.find(ptr->endpoint);
	if (it != _managerOneway.end()) {
		return it->second;
	}
	Ice::ObjectPrx prx = ServiceI::instance().getAdapter()->getCommunicator()->stringToProxy("M@"+ptr->endpoint);
	DeliverInterfacePrx dPrx =
			DeliverInterfacePrx::uncheckedCast(prx->ice_oneway());
	_managerOneway[ptr->endpoint] = dPrx;
	return dPrx;
}
void MucDeliverManagerI::deliver(const MessageSeq& seq, const Ice::Current& current) {
	MCE_DEBUG(__FUNCTION__ << " --> seq.size="<<seq.size());
	for (size_t i = 0; i < seq.size(); ++i) {
		PipePool::instance().push(seq.at(i)->to->endpoint, 0, seq.at(i));
	}
}

//--------------------------------------------------
