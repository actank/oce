#include "MessageDispatcher.h"
#include "IceLogger.h"

using namespace com::xiaonei::talk::util;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;
MessageDispatcher::MessageDispatcher(MessageDispatcher* dispatcher,
		size_t timeslot) {
	MyUtil::TaskManager::instance().execute(new MessageDispatchTask(dispatcher,timeslot));
}

void MessageDispatcher::push(const MessageSeq& seq) {
	IceUtil::Mutex::Lock lock(_mutex);
	if (seq.empty()) {
		return;
	}
	//MCE_DEBUG("call NotifyTaskManager::addTargetHandler  seq.size:"<<seq.size());
	_seq.insert(_seq.end(), seq.begin(), seq.end());
}
void MessageDispatcher::push(const MessagePtr& msg) {
	IceUtil::Mutex::Lock lock(_mutex);
	if (msg) {
		_seq.push_back(msg);
	}
}

void MessageDispatcher::pop(MessageSeq& seq) {
	IceUtil::Mutex::Lock lock(_mutex);
	_seq.swap(seq);
}

//--------------------------------------------------------
void MessageDispatchTask::handle() {
	//MCE_DEBUG("run TargetHandlerTask::handle()");
	MyUtil::TaskManager::instance().execute(new MessageDispatchTask(_dispatcher,_timeslot));

	MessageSeq seq;
	_dispatcher->pop(seq);

	map<string, MessageSeq> msgMap;

	for (size_t i = 0; i < seq.size(); ++i) {
		if (_dispatcher->check(seq.at(i))) {
			msgMap[seq.at(i)->to->endpoint].push_back(seq.at(i));
		}
	}
	for (map<string,MessageSeq>::iterator it = msgMap.begin(); it
			!= msgMap.end(); ++it) {
		_dispatcher->deliver(it->second);
	}
	//
	//	MCE_DEBUG("TargetHandlerTask::handle()  tmpMap.size:"<<tmpMap.size());
	//	for (TargetHandlerMap::iterator it = tmpMap.begin(); it != tmpMap.end(); ++it) {
	//		if (it->second.empty()) {
	//			continue;
	//		}
	//		try {
	//			it->second.at(0)->batchDeliver(it->second);
	//		} catch(...) {
	//			MCE_WARN("TargetHandlerTask::handle -> call batchDeliver err");
	//		}
	//	}

}

//---------------------------------------------------


