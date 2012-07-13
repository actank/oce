#ifndef __XCE_UTIL_THREADPOOL_EXECUTOR_H__
#define __XCE_UTIL_THREADPOOL_EXECUTOR_H__

#include <Ice/Ice.h>
#include <set>
namespace xce {
namespace util {
using namespace std;

const static int kThreadStackSize = 128*1024;

class Runnable: public IceUtil::Shared {
public:
	virtual void run() = 0;
	virtual void finish() {
		;
	}
	virtual void exception(const string& message) {
		;
	}
};
typedef IceUtil::Handle<Runnable> RunnablePtr;

class Worker;
typedef IceUtil::Handle<Worker> WorkerPtr;

class ThreadPoolExecutor: public virtual IceUtil::Shared {
	friend class Worker;
public:
	ThreadPoolExecutor(size_t corePoolSize, size_t maximumPoolSize,
			size_t keepAliveTime = 100, size_t threadStackSize = kThreadStackSize);
	virtual ~ThreadPoolExecutor();

	void execute(const RunnablePtr& command);
	size_t cancelAllTasks();

	void setCorePoolSize(size_t corePoolSize);
	size_t getCorePoolSize();

	bool allowsCoreThreadTimeOut();
	void allowsCoreThreadTimeOut(bool value);

	void setMaximumPoolSize(size_t maximumPoolSize);
	size_t getMaximumPoolSize();

	size_t getPoolSize();
	size_t getQueueSize();
	size_t getRunningThreadSize();
	size_t getWaitThreadSize();
	size_t getCompletedTaskSize();
	Ice::Long getTotalTaskCount();
	Ice::Long getCompletedTaskCount();

private:
	WorkerPtr addThread(const RunnablePtr& firstTask);
	bool addIfUnderCorePoolSize(const RunnablePtr& firstTask);
	bool addIfUnderMaximumPoolSize(const RunnablePtr& firstTask);
	void ensureQueuedTaskHandled(const RunnablePtr& command);
//	void reject(const RunnablePtr& command);
	bool waitforNewTask();
	RunnablePtr getTask();
	bool workerCanExit();
	void workerDone(const WorkerPtr& worker);

	typedef deque<RunnablePtr> RunnableQueue;
	RunnableQueue _queue;
	typedef set<WorkerPtr> WorkerSet;
	WorkerSet _workers;
	IceUtil::Monitor<IceUtil::Mutex> _monitor;

	volatile size_t _poolSize;
	volatile size_t _corePoolSize;
	volatile size_t _waitPoolSize;
	volatile size_t _maximumPoolSize;
	volatile size_t _currentPoolSize;
	volatile size_t _largestPoolSize;
	volatile bool _allowCoreThreadTimeOut;
	volatile Ice::Long _completedTaskCount;
	size_t _threadStackSize;
	IceUtil::Time _keepAliveTime;
};
typedef IceUtil::Handle<ThreadPoolExecutor> ThreadPoolExecutorPtr;

class Worker: public IceUtil::Thread {
	friend class ThreadPoolExecutor;
public:
	Worker(const ThreadPoolExecutorPtr& pool, const RunnablePtr& firstTask);

	virtual void run();
private:
	void runTask(const RunnablePtr& task);
	void interruptIfIdle();

	ThreadPoolExecutorPtr _pool;
	/**
	 * Initial task to run before entering run loop. Possibly null.
	 */
	RunnablePtr _firstTask;

	/**
	 * Per thread completed task counter; accumulated
	 * into completedTaskCount upon termination.
	 */
	volatile Ice::Long _completedTaskCount;
};

struct ThreadPoolConfig {
	ThreadPoolConfig() :
		core(5), max(100) {
		;
	}
	ThreadPoolConfig(int theCore, int theMax) :
		core(theCore > 0 ? theCore : 1), max(theMax < theCore ? theCore * 2
				: theMax) {
		;
	}

	friend std::ostream& operator<<(std::ostream& os,
			const ThreadPoolConfig& config) {
		os << "ThreadPoolConfig:[core: " << config.core << ", max: " << config.max << "]";
		return os;
	}

	int core;
	int max;
};

}
;
}
;
#endif
