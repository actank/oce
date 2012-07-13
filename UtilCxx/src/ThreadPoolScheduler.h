#ifndef __XCE_UTIL_THREADPOOL_SCHEDULER_H__
#define __XCE_UTIL_THREADPOOL_SCHEDULER_H__

#include <Ice/Ice.h>
#include <IceUtil/RWRecMutex.h>
#include <ext/hash_map>

namespace xce {
namespace util {
using namespace std;
using namespace __gnu_cxx;

class TimerTask: virtual public IceUtil::TimerTask {
public:
	TimerTask(int channel = 0) :
		_channel(channel) {
		;
	}
	virtual ~TimerTask() {
		;
	}
	virtual void runTimerTask() {
		try {
			run();
			finish();
		} catch (std::exception& e) {
			exception(e.what());
		} catch (...) {
			exception("unknown");
		}
	}
	virtual void run() = 0;
	virtual void finish() {
		;
	}
	virtual void exception(const string& message) {
		;
	}

	int channel() {
		return _channel;
	}
	void channel(int channel) {
		_channel = channel;
	}
private:
	int _channel;
};
typedef IceUtil::Handle<TimerTask> TimerTaskPtr;

class ThreadPoolScheduler: virtual public IceUtil::Shared {
	friend class Worker;
public:
	ThreadPoolScheduler();
	virtual ~ThreadPoolScheduler();

	void schedule(const TimerTaskPtr&, const IceUtil::Time&);
	void scheduleRepeated(const TimerTaskPtr&, const IceUtil::Time&);

private:
	typedef hash_map<int, IceUtil::TimerPtr> TimerMap;
	TimerMap _timers;
	IceUtil::RWRecMutex _mutex;
};
typedef IceUtil::Handle<ThreadPoolScheduler> ThreadPoolSchedulerPtr;

}
;
}
;
#endif
