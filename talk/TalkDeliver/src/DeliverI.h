#ifndef DELIVERI_H_
#define DELIVERI_H_

#include <Ice/Ice.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include "TalkDeliver.h"
#include "TalkProxy.h"
#include "ServiceI.h"
#include "TaskManager.h"
#include "Singleton.h"
#include "Pipe.h"

namespace com {
namespace xiaonei {
namespace talk {

using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class VertifyPipe : public Pipe {
public:
	VertifyPipe(const string& name, int index) :
		Pipe(name, index) {
	}
	;
	virtual void handle(const ObjectSeq& seq);

};

class PostPipe : public Pipe {
public:
	PostPipe(const string& name, int index) :
		Pipe(name, index) {
	}
	;
	virtual void handle(const ObjectSeq& seq);
};

class PipeFactoryI : public PipeFactory {
public:
	virtual PipePtr create(const string& type, int index) {

		if (type == "TalkRight") {
			return new VertifyPipe(type,index);
		} else {
			return new PostPipe(type, index);
		}
	}
};

//----------------------------------------------------------------------------
class DeliverManagerI : virtual public DeliverManager,
	public Singleton<DeliverManagerI> {
	
	map<string, DeliverInterfacePrx> _manager;
	map<string, DeliverInterfacePrx> _managerOneway;
	IceUtil::Mutex _mutex;
public:

	virtual void deliver(const MessageSeq& seq,
			const Ice::Current& = Ice::Current());

	DeliverInterfacePrx getProxyServer(const JidPtr& ptr);
	DeliverInterfacePrx getProxyServerOneway(const JidPtr& ptr);

};

class MessageTask : virtual public MyUtil::Task {
	MessageSeq _seq;
public:
	MessageTask(const MessageSeq& seq) :
		Task(time(NULL)), _seq(seq) {
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

#endif /*MESSAGEI_H_*/
