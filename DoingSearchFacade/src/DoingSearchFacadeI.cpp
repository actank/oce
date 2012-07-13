#include "ServiceI.h"
#include "DoingSearchFacadeI.h"
#include "DoingSearchNewContainerAdapter.h"

using namespace ::xce::doingsearch;
using namespace ::MyUtil;
using namespace ::std;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	DoingSearchFacadeIPtr dsf = new DoingSearchFacadeI;
	dsf->initialize();

	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	dsf->enableNotifyDoingSearchNewContainer(props->getPropertyWithDefault("DoingSearchFacade.EnableNotifyDoingSearchNewContainer", "false") != "false" ? true : false);

	service.getAdapter()->add(dsf, service.createIdentity("DSF", ""));
}

DoingSearchFacadeI::DoingSearchFacadeI() : _enableNotifyDoingSearchNewContainer(false) {
}

void DoingSearchFacadeI::enableNotifyDoingSearchNewContainer(bool flag) { 
	_enableNotifyDoingSearchNewContainer = flag; 
}

void DoingSearchFacadeI::initialize(){
	_dispatchThread = new Dispatcher(this);
	_dispatchThread->start(65535).detach();
}

void DoingSearchFacadeI::access(Ice::Long doingID, Ice::Long userID, Ice::Long timestamp, const std::string& content, const Ice::Current& current) {
	if (doingID==0||userID==0) {
		MCE_WARN("DoingID or UserID is zero.");
		return;
	}
	JobPtr job = new Job(doingID, userID, timestamp, content);
	MCE_DEBUG("[DoingSearchFacadeI]: access new Job (" << doingID << ", " << userID << ", " << timestamp << ", " << content << ")");
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
	_jobs.push_back(job);
}

void DoingSearchFacadeI::retrive(std::vector<JobPtr>& toInvokeCache) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
	while( _jobs.empty() ) {
		MCE_TRACE("[Dispatcher]: timedWait(IceUtil::Time::seconds(1))");
		timedWait(IceUtil::Time::seconds(1));
		if( toInvokeCache.size() != 0 ){
			break;
		}
	}
	toInvokeCache.insert(toInvokeCache.end(), _jobs.begin(), _jobs.end());
	_jobs.clear();
	MCE_TRACE("[Dispatcher]: reading new Job from _jobs ... _jobs.size()="
		<< _jobs.size() << "  toInvokeCache.size()=" << toInvokeCache.size() );
};

Job::Job(Ice::Long doingID, Ice::Long userID, Ice::Long timestamp, const std::string& content) :
		_doingID(doingID), _userID(userID), _timestamp(timestamp), _content(content) {
}

void Job::notifyDoingSearchNewContainer() {
	MCE_DEBUG("[Job]: notifyDoingSearchNewContainer (" << _doingID << ", "<< _userID << ", " << _timestamp << ", " << _content << ")");
	DoingItem item;
	item.id=_doingID;
	item.userid=_userID;
	item.time=_timestamp;
	item.content=_content;
	DoingSearchNewContainerAdapter::instance().addDoing(item);
}

NotifyDoingSearchNewContainerTask::NotifyDoingSearchNewContainerTask(const std::vector<JobPtr>& jobs):_jobs(jobs) {
}

void NotifyDoingSearchNewContainerTask::handle(){
	for (std::vector<JobPtr>::iterator it=_jobs.begin(); it!=_jobs.end(); it++)
	{
		(*it)->notifyDoingSearchNewContainer();
	}
}

DoingSearchFacadeI::Dispatcher::Dispatcher(DoingSearchFacadeIPtr facade) : _lastInvokeTime(time(NULL)),_facade(facade) {
};

void DoingSearchFacadeI::Dispatcher::run(){
	MCE_INFO("[Dispatcher]: _dispatchThread started");
	while (true) {
		try {
			invoke();
			_facade->retrive(_toInvokeCache);
		} catch (...) {
			MCE_WARN("[Dispatcher]: _dispatchThread exception ");
		}
	}
}

void DoingSearchFacadeI::Dispatcher::invoke() {
	if (shouldInvoke()){
		invokeAndClearCache();
	}
}

bool DoingSearchFacadeI::Dispatcher::shouldInvoke() {
	return (_toInvokeCache.size() > 1000) || (time(NULL) - _lastInvokeTime > 1);
}

void DoingSearchFacadeI::Dispatcher::invokeAndClearCache() {
	std::vector<JobPtr> toInvokeCache_swap;
	_toInvokeCache.swap(toInvokeCache_swap);
	_lastInvokeTime = time(NULL);
	if (_facade->_enableNotifyDoingSearchNewContainer) {
		MyUtil::TaskManager::instance().execute(new NotifyDoingSearchNewContainerTask(toInvokeCache_swap));
	}
}
