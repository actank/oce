#ifndef __DAILYACCESS_CACHE_H__
#define __DAILYACCESS_CACHE_H__

#include <DailyAccessCache.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <bitset>
#include <Singleton.h>
#include <AtomicSTL.h>
#include <ServiceI.h>
#include <QueryRunner.h>
#include <TaskManager.h>
#include <time.h>

namespace xce {
namespace dailyaccesscache {

const int DAILYACCESS_CACHE_SIZE = 2147483647;
const int TASK_LEVEL_INVOKE = 1;

class DailyAccessCacheManagerI: virtual public DailyAccessCacheManager,
virtual public MyUtil::Singleton<DailyAccessCacheManagerI>/*, virtual public IceUtil::Thread */{
public:
	DailyAccessCacheManagerI();
	void initialize();
	virtual void access(::Ice::Int, const Ice::Current&);
private:

	class TimeThread: virtual public IceUtil::Thread {
	public:
		TimeThread();
		virtual void run();
	public:
		bool isOld(int);
		void setOld(int);
	private:
		void initA();
		void initB();
		std::bitset<DAILYACCESS_CACHE_SIZE> _daily_access_a;
		std::bitset<DAILYACCESS_CACHE_SIZE> _daily_access_b;
	private:
		bool _a1b0;
		IceUtil::RWRecMutex mutex_;
		long _today;
	};
	typedef IceUtil::Handle<TimeThread> TimeThreadPtr;
	TimeThreadPtr _ttp;
};

class InvokeLoginRecordTask: virtual public MyUtil::Task {
public:
        InvokeLoginRecordTask(int userId,
                Ice::Int level=TASK_LEVEL_INVOKE ) :
                        Task(level), _userId(userId){
        }
        virtual void handle();
private:
        int _userId;
};

}
}
#endif
