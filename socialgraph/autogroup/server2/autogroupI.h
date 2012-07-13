#ifndef _AUTOGROUPWORKER_H_
#define _AUTOGROUPWORKER_H_

#include "AutoGrouping.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "ServiceI.h"

#include "socialgraph/autogroup/algorithm/executer.h"
#include "socialgraph/autogroup/algorithm/methodfactory.h"
#include "socialgraph/socialgraphutil/mcclient.h"

#include "item.pb.h"
#include "TaskManager.h"

namespace xce {
namespace socialgraph {

static const std::string CATEGORY = "FFAG";
static const int REBUILDINTERVAL = 2 * 3600; 			//rebuild threshold two hours
static const int LEVEL_GROUPING = 5;
static const int LEVEL_REMOVE = 6;

//--------------------------------------------------------------------------------

class AutoGroupI : virtual public AutoGrouping,
		virtual public MyUtil::Singleton<AutoGroupI> {
public:
	virtual ~AutoGroupI() {}
	
	virtual Info GetInfo(int, const Ice::Current&);

private:
	AutoGroupI() {}
	
	friend class MyUtil::Singleton<AutoGroupI>;
};

//--------------------------------------------------------------------------------

class AutoGroupingTask : public MyUtil::Task {
public:
	AutoGroupingTask(int userid, int level = LEVEL_GROUPING):
		Task(level), userid_(userid) {
	}
	~AutoGroupingTask() {
	}

	virtual void handle();

private:
	void Display(Items result);
	int userid_;
};

//--------------------------------------------------------------------------------

class RemoveTask : public MyUtil::Task {
public:
	RemoveTask(int userid, int level = LEVEL_REMOVE):
		Task(level), userid_(userid) {
	}
	~RemoveTask() {
	}

	virtual void handle();

private:
	int userid_;
};

}
}

#endif
