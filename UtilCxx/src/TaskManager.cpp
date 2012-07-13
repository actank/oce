#include "TaskManager.h"
#include "LogWrapper.h"

using namespace MyUtil;

//---------------------------------------------------------------------------
TaskManager::TaskManager() {
	_scheduler = new ThreadPoolScheduler;
}

TaskManager::~TaskManager() {
}

void TaskManager::config(int level, const ThreadPoolConfig& config) {
	if (level != TASK_LEVEL_TIMER) {
		MCE_INFO("TaskManager::config -> level: " << level << ", config: " << config);
		ThreadPoolExecutorPtr executor = locateExecutor(level);
		executor->setCorePoolSize(config.core);
		executor->setMaximumPoolSize(config.max);
	}
}

void TaskManager::config(const Ice::PropertiesPtr& props, const string& name,
		int level, const ThreadPoolConfig& config) {
	int theCore = props->getPropertyAsIntWithDefault(
			"Service.ThreadPool.TaskHandler." + name + ".Size", config.core);
	int theMax = props->getPropertyAsIntWithDefault(
			"Service.ThreadPool.TaskHandler." + name + ".SizeMax", config.max);
	// if theMax > 0 then renew the config else keep original config.
	// Keeping original config that means NORMAL CONFIG -> [1, 100], others -> [1, 1]
	if (level == TASK_LEVEL_NORMAL) {
		// if the level is TASK_LEVEL_NORMAL, it may uses another suite property keys,
		// so try them again
		theCore = props->getPropertyAsIntWithDefault(
				"Service.ThreadPool.TaskHandler.Size", theCore);
		theMax = props->getPropertyAsIntWithDefault(
				"Service.ThreadPool.TaskHandler.SizeMax", theMax);
	}
	if (theMax > 0) {
		ThreadPoolConfig theConfig(theCore, theMax);
		this->config(level, theConfig);
	}
}

int TaskManager::clear(int level) {
	ThreadPoolExecutorPtr executor = findExecutor(level);
	if (executor) {
		return (int)executor->cancelAllTasks();
	} else {
		return -1;
	}
}

int TaskManager::size(int level) {
	ThreadPoolExecutorPtr executor = findExecutor(level);
	if (executor) {
		return executor->getQueueSize();
	} else {
		return -1;
	}
}
int TaskManager::runningThreadSize(int level){
	ThreadPoolExecutorPtr executor = findExecutor(level);
	if (executor) {
		return executor->getRunningThreadSize();
	} else {
		return -1;
	}
}
int TaskManager::waitThreadSize(int level){
	ThreadPoolExecutorPtr executor = findExecutor(level);
	if (executor) {
		return executor->getWaitThreadSize();
	} else {
		return -1;
	}
}
int TaskManager::completedTaskSize(int level){
	ThreadPoolExecutorPtr executor = findExecutor(level);
	if (executor) {
		return executor->getCompletedTaskSize();
	} else {
		return -1;
	}
}

std::map<int, int> TaskManager::getLevel2Size() {
	std::map<int,int> level_map;

	IceUtil::RWRecMutex::RLock lock(_monitor);
	if (!_executors.empty()) {
		for (ThreadPoolExecutorMap::iterator iter = _executors.begin();
				iter != _executors.end(); ++iter) {
			level_map.insert(std::make_pair<int, int>(iter->first, iter->second->getQueueSize()));
		}
	}
	
	return level_map;
}

void TaskManager::config(const ThreadPoolConfig& config) {
	_config = config;
}

void TaskManager::execute(const TaskPtr& task) {
	locateExecutor(task->level())->execute(task);
}

void TaskManager::schedule(const TimerPtr& timer) {
	_scheduler->schedule(timer, IceUtil::Time::milliSeconds(timer->delay()));
}

void TaskManager::scheduleRepeated(const TimerPtr& timer) {
	_scheduler->scheduleRepeated(timer, IceUtil::Time::milliSeconds(timer->delay()));
}

ThreadPoolExecutorPtr TaskManager::locateExecutor(int channel) {
	{
		IceUtil::RWRecMutex::RLock lock(_monitor);
		ThreadPoolExecutorMap::iterator pos = _executors.find(channel);
		if (pos != _executors.end()) {
			return pos->second;
		}
	}
	{
		ThreadPoolExecutorPtr executor = new ThreadPoolExecutor(
				_config.core, _config.max);
		IceUtil::RWRecMutex::WLock lock(_monitor);
		_executors[channel] = executor;
		return executor;
	}
}

ThreadPoolExecutorPtr TaskManager::findExecutor(int channel) {
	IceUtil::RWRecMutex::RLock lock(_monitor);
	ThreadPoolExecutorMap::iterator pos = _executors.find(channel);
	if (pos != _executors.end()) {
		return pos->second;
	}
	return NULL;
}
