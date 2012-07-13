#ifndef TALKROOMDELIVERI_H_
#define TALKROOMDELIVERI_H_

#include <Ice/Ice.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include "MucDeliver.h"
#include "TalkProxy.h"
#include "ServiceI.h"
#include "TaskManager.h"
#include "Singleton.h"
#include "Pipe.h"


namespace talk {
namespace mucdeliver{
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace talk::mucdeliver;
using namespace MyUtil;


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
		return new PostPipe(type, index);
	}
};

//----------------------------------------------------------------------------
class MucDeliverManagerI : virtual public MucDeliverManager,
	public Singleton<MucDeliverManagerI> {

	map<string, DeliverInterfacePrx> _manager;
	map<string, DeliverInterfacePrx> _managerOneway;
	IceUtil::Mutex _mutex;
public:

	virtual void deliver(const MessageSeq& seq,
			const Ice::Current& = Ice::Current());

	DeliverInterfacePrx getProxyServer(const JidPtr& ptr);
	DeliverInterfacePrx getProxyServerOneway(const JidPtr& ptr);

};

}
;
}
;

#endif /*MESSAGEI_H_*/
