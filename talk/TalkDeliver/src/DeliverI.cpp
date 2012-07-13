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

//---------------------------------------------------------------------------
void MyUtil::initialize() {
	//DeliverManagerI* manager = new DeliverManagerI;
	ServiceI::instance().getAdapter()->add(&DeliverManagerI::instance(), ServiceI::instance().createIdentity("M", ""));

//	ConnectionPoolManager::instance().initialize(ServiceI::instance().getAdapter());

	PipePool::instance().initialize(new PipeFactoryI);
}

//--------------------------------------------------------------------------------
void VertifyPipe::handle(const ObjectSeq& seq) {

	//MCE_INFO("VertifyPipe::handle --> thread_id = " << pthread_self());
	if (seq.empty()) {
		return;
	} else {
		//MCE_DEBUG("VertifyPipe::handle --> objectseq size = " <<seq.size());
	}
	
	VerifySeq vSeq;
	IntSeq needNotVerifyUsers;
	for (size_t i = 0; i < seq.size(); ++i) {
		MessagePtr m = MessagePtr::dynamicCast(seq.at(i));
		//MCE_INFO("m->from = " << m->from->userId << " m->to = " << m->to->userId);
		if( (m->type != MESSAGE && m->type != OFFLINE_MESSAGE) || (m->from->userId == 365734329 || m->to->userId == 365734329) ){//客服ID，不检查好友。
			//MCE_INFO("VertifyPipe::handle --> m->from = " << m->from << " m->to = " << m->to);
			needNotVerifyUsers.push_back(i);
			continue;
		}
		VerifyPtr v = new Verify();
		v->from = m->from->userId;
		v->to = m->to->userId;
		/*if(MESSAGE != m->type){
			v->to = v->from;
		}*/
		v->type = m->type;

/*		if (v->type == SYSTEM_MESSAGE) {
			v->sysMsgType = m->from->endpoint;
		}*/
		v->index = i;

		vSeq.push_back(v);
	}
	MyUtil::IntSeq indexSeq;
	try{
	 indexSeq = TalkRightAdapter::instance().batchVerify(getIndex(), vSeq);
	}catch(Ice::Exception& e){
		MCE_WARN("VertifyPipe::handle-->TalkRightAdapter::batchVerify-->" << e);
		}
	indexSeq.insert(indexSeq.end(), needNotVerifyUsers.begin(), needNotVerifyUsers.end());

	//MCE_DEBUG("VertifyPipe::handle -->indexSeq size:"<<indexSeq.size());
	for (size_t i = 0; i < indexSeq.size(); ++i) {
		MessagePtr m = MessagePtr::dynamicCast(seq.at(indexSeq.at(i)));
		//MCE_DEBUG("VertifyPipe::handle --> "<<m->to->index);
		if (m->to->index == 0 || m->to->index == 2 || m->to->index >1) {
			//MCE_DEBUG("VertifyPipe::handle --> endpoint:"<<m->to->endpoint);
			PipePool::instance().push(m->to->endpoint, 0, m);
		}
	}

}
void PostPipe::handle(const ObjectSeq& seq) {
	//MCE_INFO("PostPipe::handle --> thread_id = " << pthread_self());
	if (seq.empty()) {
		return;
	} else {
		//MCE_DEBUG("PostPipe::handle --> objectseq size = " <<seq.size());
	}
	//MCE_DEBUG("PostPipe::handle --> size:" << seq.size());
	MessageSeq mseq;
	for (size_t i = 0; i < seq.size(); ++i) {
		mseq.push_back(MessagePtr::dynamicCast(seq.at(i)));
	}
	try {
		if((mseq.at(0)->to->endpoint[0] == 'T') || (mseq.at(0)->to->endpoint[0] == 'W') || (mseq.at(0)->to->endpoint[0] == 'P')){
			DeliverManagerI::instance().getProxyServerOneway(mseq.at(0)->to)->deliver(mseq);
		}
	} catch(Ice::Exception& e) {
		MCE_WARN("PostPipe::handle -->ProxyAdapter::delivere-->"<<e<<"  to endpoint:"<< mseq.at(0)->to);
	}
}

//--------------------------------------------------------------------------------

DeliverInterfacePrx DeliverManagerI::getProxyServer(const JidPtr& ptr) {
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
DeliverInterfacePrx DeliverManagerI::getProxyServerOneway(const JidPtr& ptr) {
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
void DeliverManagerI::deliver(const MessageSeq& seq, const Ice::Current& current) {
	//TaskManager::instance().execute(new MessageTask(seq));
	//MCE_DEBUG("DeliverManagerI::deliver --> seq.size="<<seq.size());
	if (!seq.empty()) {
		if (seq.at(0)->to->userId == 128487631 || seq.at(0)->to->userId == 200865373) {
			//MCE_DEBUG("DeliverManagerI::deliver --> msg:"<< seq.at(0)->msg);
		}
	}
	for(MessageSeq::const_iterator it = seq.begin(); it != seq.end(); ++it){
		MessagePtr m = (*it);
		if(m->type == MESSAGE){
			MCE_INFO("DeliverManagerI::deliver message-->from=" << jidToString(m->from) << " to=" << jidToString(m->to));
		}
		if(m->type == NOTIFY2_MESSAGE){
			MCE_INFO("DeliverManagerI::deliver notify2-->from=" << jidToString(m->from) << " to=" << jidToString(m->to));
		}
	}
	for (size_t i = 0; i < seq.size(); ++i) {
		PipePool::instance().push("TalkRight", abs(seq.at(i)->to->userId)%10, seq.at(i));
	}
}

//--------------------------------------------------


//void MessageTask::handle() {
//
//	VerifySeq vSeq;
//	for (size_t i = 0; i < _seq.size(); ++i) {
//		VerifyPtr v = new Verify();
//		v->from = _seq.at(i)->from->userId;
//		v->to = _seq.at(i)->to->userId;
//		v->type = _seq.at(i)->type;
//		if (v->type == SYSTEM_MESSAGE) {
//			v->sysMsgType = _seq.at(i)->from->endpoint;
//		}
//		v->index = i;
//
//		vSeq.push_back(v);
//	}
//
//	MyUtil::IntSeq indexSeq;
//	bool flag = true;
//	try {
//		indexSeq = TalkRightAdapter::instance().batchVerify(vSeq);
//	} catch(...) {
//		MCE_WARN("MessageTask::handle() -> do verify err");
//		flag = false;
//	}
//
//	map<string, MessageSeq> msgMap;
//	if (flag) {
//		for (size_t i = 0; i < indexSeq.size(); ++i) {
//			msgMap[_seq.at(indexSeq.at(i))->to->endpoint].push_back(_seq.at(indexSeq.at(i)));
//		}
//	} else {
//		for (size_t i = 0; i < _seq.size(); ++i) {
//			msgMap[_seq.at(i)->to->endpoint].push_back(_seq.at(i));
//		}
//	}
//	MCE_DEBUG("MessageTask::handle() -> msg.size()="<<msgMap.size());
//	for (map<string,MessageSeq>::iterator it = msgMap.begin(); it
//			!= msgMap.end(); ++it) {
//		MCE_DEBUG("MessageTask::handle() ->  to index = "<<it->second.at(0)->to->index);
//		if (it->second.at(0)->to->index > 1) {
//			try {
//				DeliverManagerI::instance().getProxyServerOneway(it->second.at(0)->to)->deliver(it->second);
//			} catch(Ice::Exception& e) {
//				MCE_WARN("MessageTask::handle() -> call proxyserver err:"<<e);
//			} catch(...) {
//				MCE_WARN("MessageTask::handle() -> call proxyserver err");
//			}
//		} else if (it->second.at(0)->to->index == 0) {
//			MCE_DEBUG("send message to front size:"<<it->second.size());
//			try {
//				MessageRouterAdapter::instance().deliverTalkMessage(it->second);
//			} catch(Ice::Exception& e) {
//				MCE_WARN("MessageTask::handle() -> call frontserver err:"<<e);
//			} catch(...) {
//				MCE_WARN("MessageTask::handle() -> call frontserver err");
//			}
//		}
//	}
//}
