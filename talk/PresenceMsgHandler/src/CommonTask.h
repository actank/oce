#ifndef COMMONTASK_H_
#define COMMONTASK_H_

#include "TalkCacheLoader.h"
#include "TalkCommon.h"
#include "TalkLogic.h"
#include "TalkCache.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "TalkUtil/src/pugixml.hpp"
#include "MessageType.h"
#include "PresenceHandlerI.h"

namespace com {
namespace xiaonei {
namespace talk {
using namespace MyUtil;
using namespace pugi;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::cache;
using namespace com::xiaonei::talk::cache::loader;


class PushOnlineBuddyTask : virtual public MyUtil::Task{
public:
	PushOnlineBuddyTask(const JidPtr& _jid, const string& _show, const string& _ver, const string& _subver, const string& _ext, const string& _secver="") : jid(_jid), show(_show), ver(_ver), subver(_subver), ext(_ext), secver(_secver){
	}
	virtual void handle();
private:
	JidPtr jid; 
	string show;
  string ver;
	string subver;
	string ext;
	string secver;
};

class PresenceNotifyTask : virtual public MyUtil::Timer{
protected:
	/*xml_document_ptr _xmlDoc;
	JidPtr _from;
	MessageType _type;*/
	AvatarSeq _activeAvatars;
	AvatarSeq _selfOtherAvatars;
	AvatarMap _buddies;
	NotifyType _type;
	TalkUserPtr _user;
	MessageSeq _mSeq;
	//string _status;
	//string _photo;
	MyUtil::Str2StrMap _parameters;
	TimeStat task_ts;
public:
	PresenceNotifyTask(const AvatarSeq& activeAvatars, const AvatarSeq& selfOtherAvatars,
			const AvatarMap& buddies, NotifyType type, const MyUtil::Str2StrMap& paras):
		Timer(100, (activeAvatars.at(0)->jid->userId % 13 )), _activeAvatars(activeAvatars), _selfOtherAvatars(selfOtherAvatars),
		_buddies(buddies), _type(type), _parameters(paras) {
			task_ts.reset();
	}
protected:
	void _notifyItsOtherAvatars();
	void _notifyAvatarToBuddies();
	void _notifyOtherAvatarsToBuddies();
	virtual void handle();
};


class BatchOfflineNotifyTask : public MyUtil::Task {
	JidSeq _fromSeq;
public:
	BatchOfflineNotifyTask(const JidSeq& seq) :
		Task(), _fromSeq(seq) {
	}
	;
	virtual void handle();
};

class ExitAllRoomTask : public MyUtil::Task {
	JidSeq _fromSeq;
public:
	ExitAllRoomTask(const JidSeq& seq) :
		Task(), _fromSeq(seq) {
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

#endif
