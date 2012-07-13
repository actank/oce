#ifndef COMMONTASK_H_
#define COMMONTASK_H_
/*
#include "TalkCommon.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "pugixml.hpp"
#include "MessageType.h"

namespace com {
namespace xiaonei {
namespace talk {

using namespace MyUtil;
using namespace pugi;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;


//--------------------------------------
//Statistic action Task
class StatInfo : public IceUtil::Shared {
public:
	int userId;
	size_t timestamp;
	string action;
	int value;

	StatInfo() :
		userId(-1), timestamp(time(NULL)), value(1) {
	}
	;
};

typedef IceUtil::Handle<StatInfo> StatInfoPtr;
typedef vector<StatInfoPtr> StatInfoSeq;

class StatManager : public Singleton<StatManager> {
protected:
	IceUtil::Mutex _mutex;
	StatInfoSeq _snapSeq;
	StatInfoSeq _actionSeq;
	int _total_msg;
	int _offline_msg;

public:
	StatManager();

	void addToSnap(const StatInfoPtr& stat);
	void addToSnap(const StatInfoSeq& seq);
	void addToAction(const StatInfoPtr& stat);
	void addToAction(const StatInfoSeq& seq);
	void incMsgCount(bool OfflineMsg = false);
//----------------------------------------------
	void doInsertSnap();
	void doInsertAction();

//----------------------------------------------
	class StatTask : virtual public MyUtil::Timer {
protected:
		size_t _timeslot;

public:
		StatTask(size_t timeslot = 5000) :
			Timer(timeslot), _timeslot(timeslot) {

		}
		;
		virtual void handle();

	};
};

//--------------------------------------
*/

/*
class LoadOfflineMessageTask: public Task{
	JidPtr _jid;
public:
	LoadOfflineMessageTask(const JidPtr& jid):_jid(jid){};
	virtual void handle();
};
*/
/*
}
;
}
;
}
;
*/
#endif
