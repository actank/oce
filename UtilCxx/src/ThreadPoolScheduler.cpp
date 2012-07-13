#include "ThreadPoolScheduler.h"

namespace xce {
namespace util {
using namespace std;

ThreadPoolScheduler::ThreadPoolScheduler() {

}

ThreadPoolScheduler::~ThreadPoolScheduler() {

}

void ThreadPoolScheduler::schedule(const TimerTaskPtr& task,
		const IceUtil::Time& delay) {
	{
		IceUtil::RWRecMutex::RLock lock(_mutex);
		TimerMap::iterator pos = _timers.find(task->channel());
		if (pos != _timers.end()) {
			pos->second->schedule(task, delay);
			return;
		}
	}
	{
		IceUtil::RWRecMutex::WLock lock(_mutex);
		IceUtil::TimerPtr timer = new IceUtil::Timer;
		timer->schedule(task, delay);
		_timers[task->channel()] = timer;
	}
}

void ThreadPoolScheduler::scheduleRepeated(const TimerTaskPtr& task,
		const IceUtil::Time& delay) {
	{
		IceUtil::RWRecMutex::RLock lock(_mutex);
		TimerMap::iterator pos = _timers.find(task->channel());
		if (pos != _timers.end()) {
			pos->second->scheduleRepeated(task, delay);
			return;
		}
	}
	{
		IceUtil::RWRecMutex::WLock lock(_mutex);
		IceUtil::TimerPtr timer = new IceUtil::Timer;
		timer->scheduleRepeated(task, delay);
		_timers[task->channel()] = timer;
	}
}

}
}
