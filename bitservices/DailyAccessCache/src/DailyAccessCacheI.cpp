#include "DailyAccessCacheI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "ScoreLoginRecordAdapter.h"

using namespace com::xiaonei::xce;
using namespace std;
using namespace xce::clusterstate;
using namespace xce::serverstate;
using namespace xce::dailyaccesscache;
using namespace xce::userscore;

void MyUtil::initialize() {
        ServiceI& service = ServiceI::instance();
        service.getAdapter()->add(&DailyAccessCacheManagerI::instance(),service.createIdentity("M", ""));
        
	TaskManager::instance().config(TASK_LEVEL_INVOKE, ThreadPoolConfig(1, 100));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("DailyAccessCache.Mod", 0);
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("DailyAccessCache.Interval", 5);
        ServerStateSubscriber::instance().initialize("ControllerDailyAccessCache", &DailyAccessCacheManagerI::instance(), mod, interval);

        DailyAccessCacheManagerI::instance().initialize();
}

DailyAccessCacheManagerI::DailyAccessCacheManagerI() {
}

void DailyAccessCacheManagerI::initialize() {
	_ttp = new TimeThread();
	_ttp->start(1024*1024).detach();
	MCE_INFO("[DailyAccessCacheManagerI] initialized.");
}


DailyAccessCacheManagerI::TimeThread::TimeThread(){
        MCE_INFO("[TimeThread] initializing ..." );
	tzset();
        time_t time_ = time(NULL);

        struct tm *local;
        local = localtime(&time_);

        local->tm_hour = 0;
        local->tm_min = 0;
        local->tm_sec = 0;

        _today = timelocal(local);

	for (int i=0;i<DAILYACCESS_CACHE_SIZE;++i) {
	     _daily_access_a.reset(i);
	     _daily_access_b.reset(i);
        }

        MCE_INFO( "[TimeThread] initialized. today : " << _today << "(" << ((time_-_today)/3600) << "h ago) Zero Time is: " << asctime(local) );
}

void DailyAccessCacheManagerI::TimeThread::initA() {
        MCE_INFO("TimeThread::initA -> start ...");
        for (int i=0;i<DAILYACCESS_CACHE_SIZE;++i) {
               _daily_access_a.reset(i);
        }
        MCE_INFO("TimeThread::initA -> done!");
}

void DailyAccessCacheManagerI::TimeThread::initB() {
        MCE_INFO("TimeThread::initB -> start ...");
        for (int i=0;i<DAILYACCESS_CACHE_SIZE;++i) {
               _daily_access_b.reset(i);
        }
        MCE_INFO("TimeThread::initB -> done!");
}

bool DailyAccessCacheManagerI::TimeThread::isOld(int userId) {
	IceUtil::RWRecMutex::RLock lock(mutex_);
	if( _a1b0 ){
		return _daily_access_a[userId];
	} else{
		return _daily_access_b[userId];
	}
}

void DailyAccessCacheManagerI::TimeThread::setOld(int userId) {
	IceUtil::RWRecMutex::RLock lock(mutex_);
	if( _a1b0 ){
		_daily_access_a.set(userId);
	} else{
		_daily_access_b.set(userId);
	}
}

void DailyAccessCacheManagerI::TimeThread::run() {
	while(true){
		if (time(NULL)-_today >= (24*3600)) {
			_today += 24*3600;
			bool a1b0_yesterday = false;
			{
				IceUtil::RWRecMutex::WLock lock(mutex_);
				a1b0_yesterday = _a1b0;
				_a1b0 = !_a1b0;
			}
			MCE_INFO("TimeThread::run -> start a new day ...");
			if( a1b0_yesterday ){
				initA();
			} else {
				initB();
			}
		}
		sleep(1);
	}
}

void DailyAccessCacheManagerI::access(int userid, const Ice::Current& current) {
	if ( _ttp->isOld(userid) ){
		//MCE_INFO("[DailyAccessCacheManagerI] access(1) -> " << userid );
		return;
	}
	_ttp->setOld(userid);
	TaskManager::instance().execute(new InvokeLoginRecordTask(userid));
	MCE_INFO("[DailyAccessCacheManagerI] access(0) -> " << userid );
}

void InvokeLoginRecordTask::handle() {
	int try_limit = 10;
	while(true){
		try {
			ScoreLoginRecordAdapter::instance().incLoginCount(_userId,4);
			break;
		} catch (Ice::Exception e){
			MCE_WARN("[ScoreLoginRecordAdapter]: incLoginCount(" << _userId << ") found Ice::Exception: " << e.what() );
		} catch (std::exception e){
			MCE_WARN("[ScoreLoginRecordAdapter]: incLoginCount(" << _userId << ") found Ice::Exception: " << e.what() );
		} catch (...){
			MCE_WARN("[ScoreLoginRecordAdapter]: incLoginCount(" << _userId << ") found unknown exception: " );
		}
		if( --try_limit <= 0 ) break;
	}
}

