#ifndef _AUTOGROUPWORKER_H_
#define _AUTOGROUPWORKER_H_

#include "socialgraph/socialgraphutil/socialgraphworkerI.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "ServiceI.h"

#include "socialgraph/autogroup/algorithm/executer.h"
#include "socialgraph/autogroup/algorithm/methodfactory.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"

#include "item.pb.h"
#include "TaskManager.h"

namespace xce {
namespace socialgraph {

static const std::string CATEGORY = "FFAG";
static const int REBUILDINTERVAL = 2 * 3600; 			//rebuild threshold two hours
static const int LEVEL_GROUPING = 5;
static const int LEVEL_REMOVE = 6;

//--------------------------------------------------------------------------------

class AutoGroupWorkerI : virtual public SocialGraphWorkerI,
		virtual public MyUtil::Singleton<AutoGroupWorkerI> {
public:
	virtual ~AutoGroupWorkerI() {}
	
	virtual void Report(const Info&, const Ice::Current&);
  virtual void PreLoad(int, bool, const Ice::Current&);
  virtual void Rebuild(int, const Ice::Current&);
	virtual Info GetInfo(int, const Ice::Current&);

private:
	AutoGroupWorkerI() {}
	
	friend class MyUtil::Singleton<AutoGroupWorkerI>;
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
