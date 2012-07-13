#ifndef DISPATCHI_H_
#define DISPATCHI_H_

#include "OnlineStatSync.h"
#include "TalkCommon.h"
#include "TaskManager.h"
#include "ServiceI.h"
#include <IceUtil/Mutex.h>
#include <set>
#include <map>
#include "Singleton.h"
#include "XmppTools.h"
#include "Pipe.h"

namespace talk {
namespace online {

using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class DispatchManagerI : public DispatchManager,
		virtual public Singleton<DispatchManagerI> {

public:
	virtual void presence(const JidPtr& jid, OnlineStat stat, const string& ver,
							const string& ext, const Ice::Current& = Ice::Current());
	virtual void offline(const JidPtr& jid, const Ice::Current& = Ice::Current());
	virtual void offlineBatch(const JidSeq& jids, const Ice::Current& = Ice::Current());

};

class NotifyTask : public MyUtil::Task {
	AvatarPtr _avatar;
public:
	NotifyTask(const AvatarPtr& avatar) : _avatar(avatar) {
	}
	;
	virtual void handle();
};

class NotifyPipe : public Pipe {
public:
	NotifyPipe(const string& name, int index) :
		Pipe(name, index) {
	}
	;
	virtual void handle(const ObjectSeq& seq);
};

class PipeFactoryI : public PipeFactory {
public:
	virtual PipePtr create(const string& type, int index) {
		return new NotifyPipe(type, index);
	}
};

}
;
}
;

#endif /*DISPATCHI_H_*/
