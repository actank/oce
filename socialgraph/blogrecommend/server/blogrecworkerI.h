#ifndef BLOG_REC_WORKER_H_
#define BLOG_REC_WORKER_H_

#include "socialgraph/socialgraphutil/socialgraphworkerI.h"

#include <iostream>
#include <string>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "ServiceI.h"

#include "item.pb.h"
#include "UtilCxx/src/TaskManager.h"

namespace xce {
namespace socialgraph {

static const std::string BLOG = "BLOG";
static const std::string BLOGTAG = "BLOGTAG";

static const int LEVEL_CREATE = 7;		//create线程的线程池大小
static const int LEVEL_REMOVE = 8;		//remove线程的线程池大小

//--------------------------------------------------------------

class BlogrecWorkerI: virtual public SocialGraphWorkerI,
		virtual public MyUtil::Singleton<BlogrecWorkerI> {
public:
	BlogrecWorkerI() {}

	virtual void Report(const Info&, const Ice::Current&);
	virtual void PreLoad(int, bool, const Ice::Current&);
	virtual void Rebuild(int, const Ice::Current&);
};

//--------------------------------------------------------------

class ProcessTask: public MyUtil::Task {
public:
	ProcessTask(long blogid, int level = LEVEL_CREATE):
		Task(level), blogid_(blogid) {
	}
	~ProcessTask() {}

	virtual void handle();

private:
	long blogid_;
};

}
}

#endif 
