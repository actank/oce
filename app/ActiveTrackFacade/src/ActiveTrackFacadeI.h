#ifndef __ActiveTrackFacadeI_h__
#define __ActiveTrackFacadeI_h__

#include <ActiveTrack.h>
#include "ObjectCacheI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

namespace xce {
namespace activetrack {

const size_t ACCESS_QUEUE_LIMIT = 50000;
const size_t ACTION_QUEUE_LIMIT = 10000;

const int ACCESS_ONLINESTATE_TASK = 1;
const int ACCESS_DAILYACCESSCACHE_TASK = 2;
const int ACCESS_USERSCORE_TASK = 3;
const int ACCESS_USERMONITOR_TASK = 4;

const int ACTION_SOCIALSCORELOGIC_TASK = 5;

//////////////////Job_Node//////////////////
class AccessJob: public IceUtil::Shared {
public:
	AccessJob(Ice::Int, const std::string&, const std::string&);
	void accessOnlineState();
	void accessDailyAccessCache();
	void accessUserScore();
	void accessUserMonitor();
private:
	Ice::Int _userId;
	std::string _ip;
	std::string _url;
};
typedef IceUtil::Handle<AccessJob> AccessJobPtr;

class ActionJob: public IceUtil::Shared {
public:
	ActionJob(Ice::Int, const MyUtil::StrSeq&,const std::string&, const std::string&);
	void actionSocialScoreLogic();
private:
	Ice::Int _userId;
	MyUtil::StrSeq _paths;
	std::string _action;
	std::string _object;
};
typedef IceUtil::Handle<ActionJob> ActionJobPtr;


//////////////////Access Tasks//////////////////
class AccessOnlineStateTask : virtual public MyUtil::Task {
public:
	AccessOnlineStateTask(const std::vector<AccessJobPtr>& jobs,
                Ice::Int level=ACCESS_ONLINESTATE_TASK ) :
                        Task(level), _jobs(jobs){
	}
	virtual void handle();
private:
	std::vector<AccessJobPtr> _jobs;
};

class AccessDailyAccessCacheTask : virtual public MyUtil::Task {
public:
	AccessDailyAccessCacheTask(const std::vector<AccessJobPtr>& jobs,
                Ice::Int level=ACCESS_DAILYACCESSCACHE_TASK ) :
                        Task(level), _jobs(jobs){
	}
	virtual void handle();
private:
	std::vector<AccessJobPtr> _jobs;
};

class AccessUserScoreTask : virtual public MyUtil::Task {
public:
	AccessUserScoreTask(const std::vector<AccessJobPtr>& jobs,
                Ice::Int level=ACCESS_USERSCORE_TASK ) :
                        Task(level), _jobs(jobs){
	}
	virtual void handle();
private:
	std::vector<AccessJobPtr> _jobs;
};

class AccessUserMonitorTask : virtual public MyUtil::Task {
public:
	AccessUserMonitorTask(const std::vector<AccessJobPtr>& jobs,
                Ice::Int level=ACCESS_USERMONITOR_TASK ) :
                        Task(level), _jobs(jobs){
	}
	virtual void handle();
private:
	std::vector<AccessJobPtr> _jobs;
};

//////////////////Action Tasks//////////////////

class ActionSocialScoreLogicTask : virtual public MyUtil::Task {
public:
	ActionSocialScoreLogicTask(const std::vector<ActionJobPtr>& jobs,
                Ice::Int level=ACTION_SOCIALSCORELOGIC_TASK ) :
                        Task(level), _jobs(jobs){
	}
	virtual void handle();
private:
	std::vector<ActionJobPtr> _jobs;
};


//////////////////ActiveTrackFacadeI//////////////////
class ActiveTrackFacadeI;
typedef IceUtil::Handle<ActiveTrackFacadeI> ActiveTrackFacadeIPtr;

class ActiveTrackFacadeI : virtual public ActiveTrackFacade {
public:
	ActiveTrackFacadeI() :
		_accessOnlineState(false),
		_accessDailyAccessCache(false),
		_accessUserScore(false),
		_accessUserMonitor(false),
		_actionSocialScoreLogic(false) {};

	void initialize();

	void enableAccessOnlineState(bool flag) {
		_accessOnlineState = flag;
	};

	void enableAccessDailyAccessCache(bool flag) {
		_accessDailyAccessCache = flag;
	};

	void enableAccessUserScore(bool flag) {
		_accessUserScore = flag;
	};

	void enableAccessUserMonitor(bool flag) {
		_accessUserMonitor = flag;
	};

	void enableActionSocialScoreLogic(bool flag) {
		_actionSocialScoreLogic = flag;
	};

	virtual void access(Ice::Int, const std::string&, const std::string&, const Ice::Current&);
        virtual void action(Ice::Int, const MyUtil::StrSeq&, const std::string&, const string&, const Ice::Current&);

private:
	bool _accessOnlineState;
	bool _accessDailyAccessCache;
	bool _accessUserScore;
	bool _accessUserMonitor;

	std::vector<AccessJobPtr> _access_jobs;
        IceUtil::Monitor<IceUtil::Mutex> access_mutex_;
        void popAccessJobs( std::vector<AccessJobPtr>& );

	class AccessDispatcher: virtual public IceUtil::Thread {
	public:
		AccessDispatcher(ActiveTrackFacadeIPtr facade) :
			_facade(facade) {
		};
		virtual void run();
	private:
		ActiveTrackFacadeIPtr _facade;
	};
	typedef IceUtil::Handle<AccessDispatcher> AccessDispatcherPtr;
	AccessDispatcherPtr _access_dp;

private:
	bool _actionSocialScoreLogic;

	std::vector<ActionJobPtr> _action_jobs;
        IceUtil::Monitor<IceUtil::Mutex> action_mutex_;
        void popActionJobs( std::vector<ActionJobPtr>& );

	class ActionDispatcher: virtual public IceUtil::Thread {
	public:
		ActionDispatcher(ActiveTrackFacadeIPtr facade) :
			_facade(facade) {
		};
		virtual void run();
	private:
		ActiveTrackFacadeIPtr _facade;
	};
	typedef IceUtil::Handle<ActionDispatcher> ActionDispatcherPtr;
	ActionDispatcherPtr _action_dp;
};


}
}
#endif
