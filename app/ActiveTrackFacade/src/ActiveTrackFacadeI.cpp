#include "ServiceI.h"
#include "ActiveTrackFacadeI.h"
#include "OnlineAdapter.h"
#include "DailyAccessCacheAdapter.h"
#include "UserScoreActiveTrackAdapter.h"
#include "UserMonitorAdapter.h"
//#include "SocialScoreLogicAdapter.h"

using namespace ::xce::activetrack;
using namespace ::xce::online::adapter;
using namespace ::xce::usermonitor::adapter;
using namespace ::xce::dailyaccesscache::adapter;
using namespace ::wservice::userscore::adapter;
//using namespace ::xce::socialscore::adapter;
using namespace ::MyUtil;
using namespace ::std;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	ActiveTrackFacadeIPtr atf = new ActiveTrackFacadeI;
	atf->initialize();

	TaskManager::instance().config(ACCESS_ONLINESTATE_TASK, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(ACCESS_DAILYACCESSCACHE_TASK, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(ACCESS_USERSCORE_TASK, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(ACCESS_USERMONITOR_TASK, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(ACTION_SOCIALSCORELOGIC_TASK, ThreadPoolConfig(1, 100));

	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	bool enableOnlineState =
		props->getPropertyWithDefault("ActiveTrackFacade.EnableAccessOnlineState", "false") != "false" ? true : false;
	atf->enableAccessOnlineState(enableOnlineState);
	atf->enableAccessDailyAccessCache(props->getPropertyWithDefault("ActiveTrackFacade.EnableAccessDailyAccessCache", "false") != "false" ? true : false);
	atf->enableAccessUserScore(props->getPropertyWithDefault("ActiveTrackFacade.EnableAccessUserScore", "false") != "false" ? true : false);
	atf->enableAccessUserMonitor(props->getPropertyWithDefault("ActiveTrackFacade.EnableAccessUserMonitor", "false") != "false" ? true : false);

	atf->enableActionSocialScoreLogic(props->getPropertyWithDefault("ActiveTrackFacade.EnableActionSocialScoreLogic", "false") != "false" ? true : false);

	service.getAdapter()->add(atf, service.createIdentity("ATF", ""));
	if( enableOnlineState ){
		OnlineAdapter::instance().intialize();
	}
}

///////////////////////////////////////////////////

void ActiveTrackFacadeI::initialize(){
	_access_dp = new AccessDispatcher(this);
	_access_dp->start(65535).detach();
	_action_dp = new ActionDispatcher(this);
	_action_dp->start(65535).detach();
}

void ActiveTrackFacadeI::access(Ice::Int userId, const std::string& ip, const std::string& url, const Ice::Current & current) {
	/*
	if (userId<0){
		//MCE_WARN("[ActiveTrackFacadeI]: access a warning userId: (" << userId << ", " << ip << ", " << url << ")");
		return;
	}
	*/
	AccessJobPtr job = new AccessJob(userId, ip, url);
	//MCE_DEBUG("[ActiveTrackFacadeI]: access new AccessJob (" << userId << ", " << ip << ", " << url << ")");
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock( access_mutex_ );
	_access_jobs.push_back(job);
        if (_access_jobs.size() >= ACCESS_QUEUE_LIMIT) access_mutex_.notify();
}

void ActiveTrackFacadeI::action(Ice::Int userId, const MyUtil::StrSeq& paths, const std::string& action,
                        const std::string& object, const Ice::Current & current){
	/*
	if (userId<0){
		//MCE_WARN("[ActiveTrackFacadeI]: action a warning userId: (" << userId << ", ... ," << action << ", " << object << ")");
		return;
	}
	*/
	ActionJobPtr job = new ActionJob(userId, paths, action, object);
	//MCE_DEBUG("[ActiveTrackFacadeI]: action new ActionJob (" << userId << ", ... ," << action << ", " << object << ")");
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock( action_mutex_ );
	_action_jobs.push_back(job);
        if (_action_jobs.size() >= ACTION_QUEUE_LIMIT) action_mutex_.notify();
}

void ActiveTrackFacadeI::popAccessJobs(std::vector<AccessJobPtr>& toInvokeCache) {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(access_mutex_);
        if (_access_jobs.size() < ACCESS_QUEUE_LIMIT) access_mutex_.timedWait(IceUtil::Time::seconds(1));
        _access_jobs.swap(toInvokeCache);
};

void ActiveTrackFacadeI::popActionJobs(std::vector<ActionJobPtr>& toInvokeCache) {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(action_mutex_);
        if (_action_jobs.size() < ACTION_QUEUE_LIMIT) action_mutex_.timedWait(IceUtil::Time::seconds(1));
        _action_jobs.swap(toInvokeCache);
};

///////////////////////////////////////////////////

void ActiveTrackFacadeI::AccessDispatcher::run(){
	MCE_INFO("[AccessDispatcher]: _access_dp started");
	while (true) {
		try {
			std::vector<AccessJobPtr> toInvokeCache_swap;
			_facade->popAccessJobs(toInvokeCache_swap);
			MCE_INFO("[AccessDispatcher]: new access ... size = " << toInvokeCache_swap.size() );
			if (_facade->_accessOnlineState) {
				MyUtil::TaskManager::instance().execute(new AccessOnlineStateTask(toInvokeCache_swap));
			}
			if (_facade->_accessDailyAccessCache) {
				MyUtil::TaskManager::instance().execute(new AccessDailyAccessCacheTask(toInvokeCache_swap));
			}
			if (_facade->_accessUserScore) {
				MyUtil::TaskManager::instance().execute(new AccessUserScoreTask(toInvokeCache_swap));
			}
			if (_facade->_accessUserMonitor) {
				MyUtil::TaskManager::instance().execute(new AccessUserMonitorTask(toInvokeCache_swap));
			}
		} catch (...) {
			MCE_WARN("[AccessDispatcher]: _access_dp exception ");
		}
	}
}

///////////////////////////////////////////////////

void ActiveTrackFacadeI::ActionDispatcher::run(){
	MCE_INFO("[ActionDispatcher]: _action_dp started");
	while (true) {
		try {
			std::vector<ActionJobPtr> toInvokeCache_swap;
			_facade->popActionJobs(toInvokeCache_swap);
			MCE_INFO("[ActionDispatcher]: new action ... size = " << toInvokeCache_swap.size() );
			if (_facade->_actionSocialScoreLogic) {
				MyUtil::TaskManager::instance().execute(new ActionSocialScoreLogicTask(toInvokeCache_swap));
			}
		} catch (...) {
			MCE_WARN("[ActionDispatcher]: _action_dp exception ");
		}
	}
}

///////////////////////////////////////////////////

void AccessOnlineStateTask::handle(){
	MCE_INFO("[AccessOnlineStateTask]: access " << _jobs.size() );
	for (std::vector<AccessJobPtr>::iterator it=_jobs.begin(); it!=_jobs.end(); it++)
	{
		(*it)->accessOnlineState();
	}
}
void AccessDailyAccessCacheTask::handle(){
	MCE_INFO("[AccessDailyAccessCacheTask]: access " << _jobs.size() );
	for (std::vector<AccessJobPtr>::iterator it=_jobs.begin(); it!=_jobs.end(); it++)
	{
		(*it)->accessDailyAccessCache();
	}
}
void AccessUserScoreTask::handle(){
	MCE_INFO("[AccessUserScoreTask]: access " << _jobs.size() );
	for (std::vector<AccessJobPtr>::iterator it=_jobs.begin(); it!=_jobs.end(); it++)
	{
		(*it)->accessUserScore();
	}
}
void AccessUserMonitorTask::handle(){
	MCE_INFO("[AccessUserMonitorTask]: access " << _jobs.size() );
	for (std::vector<AccessJobPtr>::iterator it=_jobs.begin(); it!=_jobs.end(); it++)
	{
		(*it)->accessUserMonitor();
	}
}

void ActionSocialScoreLogicTask::handle() {
	MCE_INFO("[ActionSocialScoreLogicTask]: action " << _jobs.size() );
	for (std::vector<ActionJobPtr>::iterator it = _jobs.begin(); it != _jobs.end(); ++it) {
		(*it)->actionSocialScoreLogic();
	}
}

///////////////////////////////////////////////////

AccessJob::AccessJob(Ice::Int userId, const std::string& ip, const std::string& url) :
		_userId(userId), _ip(ip), _url(url) {
}
void AccessJob::accessOnlineState(){
	//MCE_DEBUG("[AccessJob]: accessOnlineState (" << _userId << ")");
	OnlineAdapter::instance().ActiveUsers(_userId);
}
void AccessJob::accessDailyAccessCache(){
	//MCE_DEBUG("[AccessJob]: accessDailyAccessCache (" << _userId << ")");
	int try_limit = 10;
	while(true){
		try {
			DailyAccessCacheAdapter::instance().access(_userId);
			break;
		} catch (Ice::Exception e){
			MCE_WARN("[DailyAccessCacheAdapter]: access(" << _userId << ") found Ice::Exception: " << e.what() );
		} catch (std::exception e){
			MCE_WARN("[DailyAccessCacheAdapter]: access(" << _userId << ") found Ice::Exception: " << e.what() );
		} catch (...){
			MCE_WARN("[DailyAccessCacheAdapter]: access(" << _userId << ") found unknown exception: " );
		}
		if( --try_limit <= 0 ) break;
	}
}
void AccessJob::accessUserScore(){
	//MCE_DEBUG("[AccessJob]: accessUserScore (" << _userId << ")");
	int try_limit = 10;
	while(true){
		try {
			UserScoreActiveTrackAdapter::instance().incrLoginCountTcp(_userId);
			break;
		} catch (Ice::Exception e){
			MCE_WARN("[UserScoreActiveTrackAdapter]: incrLoginCount(" << _userId << ") found Ice::Exception: " << e.what() );
		} catch (std::exception e){
			MCE_WARN("[UserScoreActiveTrackAdapter]: incrLoginCount(" << _userId << ") found Ice::Exception: " << e.what() );
		} catch (...){
			MCE_WARN("[UserScoreActiveTrackAdapter]: incrLoginCount(" << _userId << ") found unknown exception: " );
		}
		if( --try_limit <= 0 ) break;
	}
}
void AccessJob::accessUserMonitor(){
	//MCE_DEBUG("[AccessJob]: accessUserMonitor (" << _userId << ", " << _ip << ", " << _url << ")");
	try {
		UserMonitorAdapter::instance().setUserActivity(_userId, _ip, _url);
	} catch (Ice::Exception e){
		MCE_WARN("[UserMonitorAdapter]: setUserActivity(" << _userId << ") found Ice::Exception: " << e.what() );
	} catch (std::exception e){
		MCE_WARN("[UserMonitorAdapter]: setUserActivity(" << _userId << ") found Ice::Exception: " << e.what() );
	} catch (...){
		MCE_WARN("[UserMonitorAdapter]: setUserActivity(" << _userId << ") found unknown exception: " );
	}
}

///////////////////////////////////////////////////

ActionJob::ActionJob(Ice::Int userId, const MyUtil::StrSeq& paths,const std::string& action, const std::string& object) :
		_userId(userId), _paths(paths), _action(action), _object(object) {
}
void ActionJob::actionSocialScoreLogic(){
	//MCE_DEBUG("[ActionJob]: actionSocialScoreLogic (" << userId << ", ... ," << action << ", " << object << ")");
	//ActionSocialScoreLogicAdapter::instance().action(_userId,_paths,_action,_object);
}

