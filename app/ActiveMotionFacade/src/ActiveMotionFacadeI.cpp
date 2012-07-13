#include "ActiveMotionFacadeI.h"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace ::xce::activetrack;
using namespace ::xce::log;
using namespace ::MyUtil;
using namespace ::std;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	TaskManager::instance().config(MOTION_XCE_LOG_TASK, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(MOTION_PROFILE_HOT_TASK, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(MOTION_FEED_NEWS_TASK, ThreadPoolConfig(1, 100));

	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	bool ableMotionXceLog = props->getPropertyWithDefault("ActiveMotionFacade.EnableMotionXceLog", "false") != "false" ? true : false ;
	ActiveMotionFacadeI::instance().enableMotionXceLog(ableMotionXceLog);
	ActiveMotionFacadeI::instance().enableMotionProfileHot(props->getPropertyWithDefault("ActiveMotionFacade.EnableMotionProfileHot", "false") != "false" ? true : false );
	ActiveMotionFacadeI::instance().enableMotionFeedNews(props->getPropertyWithDefault("ActiveMotionFacade.EnableMotionFeedNews", "false") != "false" ? true : false );

	if( ableMotionXceLog ){
        	ActiveTrack::instance().initialize();
	}

	ActiveMotionFacadeI::instance().initialize();
	service.getAdapter()->add(&ActiveMotionFacadeI::instance(), service.createIdentity("AMF", ""));

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("ActiveMotionFacade.Mod");
        string ctlEndpoints = service.getCommunicator()->getProperties()->getPropertyWithDefault("Controller.Endpoints","ControllerActiveMotionFacade");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Controller.Interval",5);
        ServerStateSubscriber::instance().initialize(ctlEndpoints, &ActiveMotionFacadeI::instance(), mod, interval);

}

///////////////////////////////////////////////////

void ActiveMotionFacadeI::initialize(){
	_motion_dp = new MotionDispatcher(this);
	_motion_dp->start(65535).detach();
}

void ActiveMotionFacadeI::motion(const MyUtil::StrSeq& params, const Ice::Current & current) {
	MotionJobPtr job = new MotionJob(params);
	//MCE_DEBUG("[ActiveMotionFacadeI]: motion new MotionJob (" << userId << ", " << ip << ", " << url << ")");
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock( motion_mutex_ );
	_motion_jobs.push_back(job);
        if (_motion_jobs.size() >= MOTION_QUEUE_LIMIT) motion_mutex_.notify();
}

void ActiveMotionFacadeI::popMotionJobs(std::vector<MotionJobPtr>& toInvokeCache) {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(motion_mutex_);
        if (_motion_jobs.size() < MOTION_QUEUE_LIMIT) motion_mutex_.timedWait(IceUtil::Time::seconds(1));
        _motion_jobs.swap(toInvokeCache);
};

///////////////////////////////////////////////////

void ActiveMotionFacadeI::MotionDispatcher::run(){
	MCE_INFO("[MotionDispatcher]: _motion_dp started");
	while (true) {
		try {
			std::vector<MotionJobPtr> toInvokeCache_swap;
			_facade->popMotionJobs(toInvokeCache_swap);
			MCE_INFO("[MotionDispatcher]: new motion ... size = " << toInvokeCache_swap.size() );
			if (_facade->_motionXceLog) {
				MyUtil::TaskManager::instance().execute(new MotionXceLogTask(toInvokeCache_swap));
			}
			if (_facade->_motionProfileHot) {
				MyUtil::TaskManager::instance().execute(new MotionProfileHotTask(toInvokeCache_swap));
			}
			if (_facade->_motionFeedNews) {
				MyUtil::TaskManager::instance().execute(new MotionFeedNewsTask(toInvokeCache_swap));
			}
		} catch (...) {
			MCE_WARN("[MotionDispatcher]: _motion_dp exception ");
		}
	}
}

///////////////////////////////////////////////////

MotionJob::MotionJob(const MyUtil::StrSeq& params):
		_params(params) {
}
void MotionJob::motionXceLog(){
	//MCE_DEBUG("[MotionJob]: motionXceLog (" << _userId << ", " << _ip << ", " << _url << ")");

	/*

        int userId = 25596255;
        vector<string> paths;
        paths.push_back("RE_68126");
        paths.push_back("AT_PHOTO_12345");
        string action = "REPLY";
        string object = "PHOTO_12345_68128";
        cout << "action: " << userId << " ";
        for( vector<string>::iterator it = paths.begin(); it != paths.end(); ++it ){
                cout << *it << " ";
        }
        cout << action << " ";
        cout << object << " ";
        cout << endl;

        ActiveTrack::instance().action(userId, paths, action, object);
        ActiveTrack::instance().action(userId, "RE_68126", action, object);

	*/
}
void MotionJob::motionProfileHot(){
	//MCE_DEBUG("[MotionJob]: motionProfileHot (" << _userId << ")");
        /*
        ActiveTrack::instance().initialize();
        ActiveTrack::instance().action(userId, paths, action, object);
        ActiveTrack::instance().action(userId, "RE_68126", action, object);
        */

}
void MotionJob::motionFeedNews(){
	//MCE_DEBUG("[MotionJob]: motionFeedNews (" << _userId << ")");
	/*
	int try_limit = 10;
	while(true){
		try {
			FeedNewsAdapter::instance().motion(_userId);
			break;
		} catch (Ice::Exception e){
			MCE_WARN("[FeedNewsAdapter]: motion(" << _userId << ") found Ice::Exception: " << e.what() );
		} catch (std::exception e){
			MCE_WARN("[FeedNewsAdapter]: motion(" << _userId << ") found Ice::Exception: " << e.what() );
		} catch (...){
			MCE_WARN("[FeedNewsAdapter]: motion(" << _userId << ") found unknown exception: " );
		}
		if( --try_limit <= 0 ) break;
	}
	*/
}

///////////////////////////////////////////////////

void MotionXceLogTask::handle(){
	MCE_INFO("[MotionXceLogTask]: motion " << _jobs.size() );
	for (std::vector<MotionJobPtr>::iterator it=_jobs.begin(); it!=_jobs.end(); it++)
	{
		(*it)->motionXceLog();
	}
}
void MotionProfileHotTask::handle(){
	MCE_INFO("[MotionProfileHotTask]: motion " << _jobs.size() );
	for (std::vector<MotionJobPtr>::iterator it=_jobs.begin(); it!=_jobs.end(); it++)
	{
		(*it)->motionProfileHot();
	}
}
void MotionFeedNewsTask::handle(){
	MCE_INFO("[MotionFeedNewsTask]: motion " << _jobs.size() );
	for (std::vector<MotionJobPtr>::iterator it=_jobs.begin(); it!=_jobs.end(); it++)
	{
		(*it)->motionFeedNews();
	}
}

