#ifndef __XCE_UTIL_TASK_MANAGER_H__
#define __XCE_UTIL_TASK_MANAGER_H__

#include "ThreadPoolExecutor.h"
#include "ThreadPoolScheduler.h"
#include "Singleton.h"

#include <list>
#include <map>

namespace MyUtil {
using namespace std;
using namespace xce::util;

const static int TASK_LEVEL_NORMAL = 0;
const static int TASK_LEVEL_CREATE = -1;
const static int TASK_LEVEL_QUERY = -2;
const static int TASK_LEVEL_DELAY_CREATE = -3;
const static int TASK_LEVEL_PRELOAD = -4;
const static int TASK_LEVEL_RELOAD = -5;
const static int TASK_LEVEL_PARALLEL_GET = -6;// for ServiceAdapter parallel get
const static int TASK_LEVEL_TIMER = -128;

//---------------------------------------------------------------------------

class Task: public Runnable {
public:
	Task(int level = TASK_LEVEL_NORMAL) :
		_level(level) {
		;
	}
	virtual ~Task() {
		;
	}
	virtual void run() {
		try {
			handle();
		} catch (std::exception& e) {
			exception(e.what());
		} catch (...) {
			exception("unknown");
		}
	}
	virtual void handle() = 0;

	int level() const {
		return _level;
	}
	void level(int level) {
		_level = level;
	}
protected:
	int _level;
};
typedef IceUtil::Handle<Task> TaskPtr;

//---------------------------------------------------------------------------
class Timer: public TimerTask {
public:
	Timer(int delay, int channel = 0) :
		TimerTask(channel), _delay(delay) {
		;
	}
	virtual ~Timer() {
		;
	}

	virtual void run() {
		handle();
	}
	virtual void handle() = 0;

	int delay() const {
		return _delay;
	}
	void delay(int delay) {
		_delay = delay;
	}

protected:
	int _delay;
};
typedef IceUtil::Handle<Timer> TimerPtr;

//---------------------------------------------------------------------------
class TaskManager: public Singleton<TaskManager> {
	friend class Singleton<TaskManager> ;
public:
	void config(int level, const ThreadPoolConfig& config = ThreadPoolConfig());
	void config(const Ice::PropertiesPtr& props, const string& name, int level,
			const ThreadPoolConfig& config = ThreadPoolConfig());
	void config(const ThreadPoolConfig& config);

	void execute(const TaskPtr& task);
	void schedule(const TimerPtr& timer);
	void scheduleRepeated(const TimerPtr& timer);

	int clear(int level);
	int size(int level);
  int runningThreadSize(int level);
  int waitThreadSize(int level);
  int completedTaskSize(int level);
	std::map<int, int> getLevel2Size();

private:
	TaskManager();
	~TaskManager();

	TaskManager(const TaskManager& rhs);
	TaskManager& operator=(const TaskManager& rhs);

	ThreadPoolExecutorPtr locateExecutor(int channel);
	ThreadPoolExecutorPtr findExecutor(int channel);

	IceUtil::RWRecMutex _monitor;
	typedef hash_map<int, ThreadPoolExecutorPtr> ThreadPoolExecutorMap;
	ThreadPoolExecutorMap _executors;
	ThreadPoolSchedulerPtr _scheduler;

	ThreadPoolConfig _config;
};

}

#endif

