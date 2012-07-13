#ifndef __ActiveMotionFacadeI_h__
#define __ActiveMotionFacadeI_h__

#include <ActiveTrack.h>
#include "ObjectCacheI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ServiceI.h"

namespace xce {
namespace activetrack {

const size_t MOTION_QUEUE_LIMIT = 1;

const int MOTION_XCE_LOG_TASK = 1;
const int MOTION_PROFILE_HOT_TASK = 1;
const int MOTION_FEED_NEWS_TASK = 2;

//////////////////Job_Node//////////////////
class MotionJob: public IceUtil::Shared {
public:
	MotionJob(const MyUtil::StrSeq&);
	void motionXceLog();
	void motionProfileHot();
	void motionFeedNews();
private:
	MyUtil::StrSeq _params;
};
typedef IceUtil::Handle<MotionJob> MotionJobPtr;


//////////////////Motion Tasks//////////////////
class MotionXceLogTask : virtual public MyUtil::Task {
public:
	MotionXceLogTask(const std::vector<MotionJobPtr>& jobs,
                Ice::Int level=MOTION_XCE_LOG_TASK) :
                        Task(level), _jobs(jobs){
	}
	virtual void handle();
private:
	std::vector<MotionJobPtr> _jobs;
};

class MotionProfileHotTask : virtual public MyUtil::Task {
public:
	MotionProfileHotTask(const std::vector<MotionJobPtr>& jobs,
                Ice::Int level=MOTION_PROFILE_HOT_TASK) :
                        Task(level), _jobs(jobs){
	}
	virtual void handle();
private:
	std::vector<MotionJobPtr> _jobs;
};

class MotionFeedNewsTask : virtual public MyUtil::Task {
public:
	MotionFeedNewsTask(const std::vector<MotionJobPtr>& jobs,
                Ice::Int level=MOTION_FEED_NEWS_TASK) :
                        Task(level), _jobs(jobs){
	}
	virtual void handle();
private:
	std::vector<MotionJobPtr> _jobs;
};

//////////////////ActiveMotionFacadeI//////////////////

class ActiveMotionFacadeI;
typedef IceUtil::Handle<ActiveMotionFacadeI> ActiveMotionFacadeIPtr;

class ActiveMotionFacadeI : virtual public ActiveMotionFacade, public MyUtil::Singleton<ActiveMotionFacadeI> {
public:
	void initialize();

	void enableMotionXceLog(bool flag) {
		_motionXceLog = flag;
	};

	void enableMotionProfileHot(bool flag) {
		_motionProfileHot = flag;
	};

	void enableMotionFeedNews(bool flag) {
		_motionFeedNews = flag;
	};

        virtual void motion(const MyUtil::StrSeq&, const Ice::Current&);

private:
	friend class MyUtil::Singleton<ActiveMotionFacadeI>;
	ActiveMotionFacadeI() :
		_motionXceLog(false),
		_motionProfileHot(false),
		_motionFeedNews(false) {
		};

	bool _motionXceLog;
	bool _motionProfileHot;
	bool _motionFeedNews;

	std::vector<MotionJobPtr> _motion_jobs;
        IceUtil::Monitor<IceUtil::Mutex> motion_mutex_;
        void popMotionJobs( std::vector<MotionJobPtr>& );

	class MotionDispatcher: virtual public IceUtil::Thread {
	public:
		MotionDispatcher(ActiveMotionFacadeIPtr facade) :
			_facade(facade) {
		};
		virtual void run();
	private:
		ActiveMotionFacadeIPtr _facade;
	};
	typedef IceUtil::Handle<MotionDispatcher> MotionDispatcherPtr;
	MotionDispatcherPtr _motion_dp;

};


}
}
#endif
