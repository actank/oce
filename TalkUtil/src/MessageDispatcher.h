#ifndef MESSAGEDISPATCHER_H_
#define MESSAGEDISPATCHER_H_

#include <Ice/Ice.h>
#include <IceUtil/Shared.h>
#include <Singleton.h>
#include "TaskManager.h"
#include "TalkCommon.h"

namespace com {
namespace xiaonei {
namespace talk {
namespace util {

using namespace com::xiaonei::talk::common;


class MessageDispatcher {
protected:
	IceUtil::Mutex _mutex;
	MessageSeq _seq;

public:
	MessageDispatcher(MessageDispatcher* dispatcher, size_t timeslot = 1);
	
	
	void push(const MessageSeq& seq);
	void push(const MessagePtr& msg);
	void pop(MessageSeq& seq);
	
	bool check(MessagePtr& mPtr){
		return true;
	};
	void deliver(const MessageSeq& seq){};

};

class MessageDispatchTask : public MyUtil::Task {
protected:
	size_t _timeslot;
	MessageDispatcher* _dispatcher;
public:
	MessageDispatchTask(MessageDispatcher* dispatcher,size_t timeslot) :
		Task(time(NULL)+timeslot),_timeslot(timeslot),_dispatcher(dispatcher){
	}
	;
	virtual void handle();
	
};

}
;
}
;
}
;
}
;

#endif /*BATCHPOST_H_*/
