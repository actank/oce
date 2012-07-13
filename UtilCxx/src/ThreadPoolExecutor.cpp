#include "ThreadPoolExecutor.h"
#include <algorithm>
#include "IceLogger.h"
//#include <iostream>
namespace xce {
namespace util {
using namespace std;
using namespace MyUtil;

ThreadPoolExecutor::ThreadPoolExecutor(size_t corePoolSize,
		size_t maximumPoolSize, size_t keepAliveTime, size_t threadStackSize) :
	_poolSize(0), _corePoolSize(corePoolSize), _waitPoolSize(0),
			_maximumPoolSize(maximumPoolSize), _currentPoolSize(0),
			_largestPoolSize(0),
			_allowCoreThreadTimeOut(true), _completedTaskCount(0),
			_threadStackSize(threadStackSize) {
	if (_corePoolSize < 0) {
		_corePoolSize = 2;
	}
	if (_maximumPoolSize <= 0 || _maximumPoolSize < _corePoolSize) {
		_maximumPoolSize = _corePoolSize * 50;
	}
	if (keepAliveTime <= 0) {
		_keepAliveTime = IceUtil::Time::milliSeconds(100);
	} else {
		_keepAliveTime = IceUtil::Time::milliSeconds(keepAliveTime);
	}
}

ThreadPoolExecutor::~ThreadPoolExecutor() {

}

void ThreadPoolExecutor::execute(const RunnablePtr& command) {
	//MCE_DEBUG("ThreadPoolExecutor::execute1 --> waitPoolSize: " << _waitPoolSize << ", queue: " << _queue.size() << ", poolSize: " << _poolSize);
	if (!command) {
		return;
	}
	if (_poolSize >= _corePoolSize || !addIfUnderCorePoolSize(command)) {
		// If queue size is less than pool size, that is to say there are
		// enough threads to handle all queued commands, so queue the new command.
		// If queue size is greater or equal than pool size, that is to say there
		// are not enough threads to handle all queued commands, so create
		// new thread if the number of threads is under maximum to handle the
		// new command.
		// If the number of threads reaches maximum, the new command
		// have to be queued.
		if (_queue.size() < _poolSize) {
			// The number of threads is enough to handle all queued commands
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
				_queue.push_back(command);
				if (_waitPoolSize > 0) {
					_monitor.notify();
				}
			}
			// Check the pool size again to ensure there is at least one thread
			// to handle commands
			if (_poolSize == 0) {
				ensureQueuedTaskHandled(command);
			}
		} else if (!addIfUnderMaximumPoolSize(command)) {
			// The number of threads reaches the maximum, so have to queue the command,
			// and notification is not required
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
			_queue.push_back(command);
			if (_waitPoolSize > 0) {
				_monitor.notify();
			}
		}
	}
}

size_t ThreadPoolExecutor::cancelAllTasks() {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	size_t n = _queue.size();
	_queue.clear();
	return n;
}

WorkerPtr ThreadPoolExecutor::addThread(const RunnablePtr& firstTask) {
	WorkerPtr w = new Worker(this, firstTask);
	_workers.insert(w);
	size_t nt = ++_poolSize;
	if (nt > _largestPoolSize) {
		_largestPoolSize = nt;
	}
	return w;
}

bool ThreadPoolExecutor::addIfUnderCorePoolSize(const RunnablePtr& firstTask) {
	WorkerPtr w;
	{
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
		if (_poolSize < _corePoolSize) {
			w = addThread(firstTask);
		}
	}
	if (!w) {
		return false;
	}
	w->start(_threadStackSize).detach();
	return true;
}

bool ThreadPoolExecutor::addIfUnderMaximumPoolSize(const RunnablePtr& firstTask) {
	WorkerPtr w;
	{
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
		if (_poolSize < _maximumPoolSize)
			w = addThread(firstTask);
	}
	if (!w) {
		return false;
	}
	w->start(_threadStackSize).detach();
	return true;
}

void ThreadPoolExecutor::ensureQueuedTaskHandled(const RunnablePtr& command) {
	WorkerPtr w;
	{
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
		if (_poolSize < std::max((size_t) _corePoolSize, (size_t) 1)
				&& !_queue.empty()) {
			w = addThread(NULL);
		}
	}
	if (w) {
		w->start(_threadStackSize).detach();
	}
}

bool ThreadPoolExecutor::waitforNewTask() {
// This method must be under the lock block0
	++_waitPoolSize;
	bool gotit = false;
	if (_poolSize > _corePoolSize) {
		gotit = _monitor.timedWait(_keepAliveTime);
	} else {
		if (_allowCoreThreadTimeOut) {
			gotit = _monitor.timedWait(_keepAliveTime * 10);
		} else {
			 _monitor.wait();
			gotit = true;
		}
	}
	--_waitPoolSize;
	return gotit;
}

RunnablePtr ThreadPoolExecutor::getTask() {
	RunnablePtr r;
	for (;;) {
		// Too many workers have been started, so excessive workers should be
		// released if there are not enough tasks
		{
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
			if (!_queue.empty()) {
				r = _queue.front();
				_queue.pop_front();
			} else {
				// Ah, this worker may be releases if waiting timeout expired
				if (waitforNewTask()) {
					// Well, there must be one new command queued, so continue to handle
					continue;
				}
			}
		}
		if (r) {
			return r;
		}
		if (workerCanExit()) {
			// notify other workers
			//			interruptIdleWorkers();
			return NULL;
		}
	}

}

bool ThreadPoolExecutor::workerCanExit() {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	return _queue.empty() || (_allowCoreThreadTimeOut && _poolSize > std::max(
			(size_t) 1, (size_t) _corePoolSize));
}

void ThreadPoolExecutor::workerDone(const WorkerPtr& w) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	_completedTaskCount += w->_completedTaskCount;
	_workers.erase(w);
	--_poolSize;
}

void ThreadPoolExecutor::setCorePoolSize(size_t corePoolSize) {
	if (corePoolSize < 0) {
		return;
	}

	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	int extra = _corePoolSize - corePoolSize;
	_corePoolSize = corePoolSize;
	if (extra < 0) {
		size_t n = _queue.size(); // don't add more threads than tasks
		while (extra++ < 0 && n-- > 0 && _poolSize < corePoolSize) {
			WorkerPtr w = addThread(NULL);
			if (w) {
				w->start(_threadStackSize).detach();
			} else {
				break;
			}
		}
	} else if (extra > 0 && _poolSize > corePoolSize) {
		WorkerSet::iterator it = _workers.begin();
		while (it != _workers.end() && extra-- > 0 && _poolSize > corePoolSize)
			(*it++)->interruptIfIdle();
	}
}

size_t ThreadPoolExecutor::getPoolSize() {
	return _poolSize;
}

size_t ThreadPoolExecutor::getCorePoolSize() {
	return _corePoolSize;
}

bool ThreadPoolExecutor::allowsCoreThreadTimeOut() {
	return _allowCoreThreadTimeOut;
}

void ThreadPoolExecutor::allowsCoreThreadTimeOut(bool value) {
	_allowCoreThreadTimeOut = value;
}

void ThreadPoolExecutor::setMaximumPoolSize(size_t maximumPoolSize) {
	if (maximumPoolSize <= 0 || maximumPoolSize < _corePoolSize) {
		return;
	}
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	int extra = _maximumPoolSize - maximumPoolSize;
	_maximumPoolSize = maximumPoolSize;
	if (extra > 0 && _poolSize > maximumPoolSize) {
		WorkerSet::iterator it = _workers.begin();
		while (it != _workers.end() && extra-- > 0 && _poolSize
				> maximumPoolSize) {
			(*it++)->interruptIfIdle();
		}

	}
}

size_t ThreadPoolExecutor::getMaximumPoolSize() {
	return _maximumPoolSize;
}

size_t ThreadPoolExecutor::getQueueSize() {
	return _queue.size();
}

size_t ThreadPoolExecutor::getRunningThreadSize(){
  return _poolSize; 
}

size_t ThreadPoolExecutor::getWaitThreadSize(){
  return _waitPoolSize;
}
size_t ThreadPoolExecutor::getCompletedTaskSize(){
  return _completedTaskCount;
}
Ice::Long ThreadPoolExecutor::getTotalTaskCount() {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	Ice::Long n = _completedTaskCount;

	for (WorkerSet::iterator it = _workers.begin(); it != _workers.end(); ++it) {
		n += (*it)->_completedTaskCount;
	}
	return n + (Ice::Long) _queue.size();
}

Ice::Long ThreadPoolExecutor::getCompletedTaskCount() {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	Ice::Long n = _completedTaskCount;

	for (WorkerSet::iterator it = _workers.begin(); it != _workers.end(); ++it) {
		n += (*it)->_completedTaskCount;
	}
	return n;
}

Worker::Worker(const ThreadPoolExecutorPtr& pool, const RunnablePtr& firstTask) :
	_pool(pool), _firstTask(firstTask), _completedTaskCount(0) {
	;
}

void Worker::run() {
	RunnablePtr task = _firstTask;
	_firstTask = NULL;
	while (task || (task = _pool->getTask())) {
		runTask(task);
		task = NULL;
	}
	_pool->workerDone(this);
}

void Worker::runTask(const RunnablePtr& task) {
	try {
		task->run();
		++_completedTaskCount;
	} catch (...) {

	}
}

void Worker::interruptIfIdle() {

}
}
}
