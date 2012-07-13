#include "ServiceI.h"
#include "ShareLogicI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace ::xce::sharesearch;
using namespace ::MyUtil;
using namespace ::std;
using namespace xce::serverstate;


void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	ShareLogicIPtr s = new ShareLogicI;
	s->initialize();
	service.getAdapter()->add(s, service.createIdentity("M", ""));
	ServerStateSubscriber::instance().initialize("ControllerShareLogic",
					s, 10, 5);
}

void ShareLogicI::initialize(){
	_dispatchThread = new Dispatcher(this);
	_dispatchThread->start(65535).detach();
}

void ShareLogicI::create(const UrlDetailInfo& info, const Ice::Current& crt){
	MCE_INFO("[ShareLogicI::create] " << info.userid << " " << info.shareid << " " << info.urlmd5);
	IceUtil::Mutex::Lock lock(create_mutex_);
	createUrlInfos.push_back(info);
}
void ShareLogicI::share(const UrlInfo& info, const Ice::Current& crt){
	MCE_INFO("[ShareLogicI::share] " << info.userid << " " << info.shareid << " " << info.urlmd5);
	IceUtil::Mutex::Lock lock(share_mutex_);
	shareUrlInfos.push_back(info);
}
void ShareLogicI::swapShare(UrlInfoSeq& infos){
	IceUtil::Mutex::Lock lock(share_mutex_);
	infos.swap(shareUrlInfos);
}
void ShareLogicI::swapCreate(UrlDetailInfoSeq& infos){
	IceUtil::Mutex::Lock lock(create_mutex_);
	infos.swap(createUrlInfos);
}
ShareLogicI::Dispatcher::Dispatcher(ShareLogicIPtr logic) : logic_(logic),
		 lastIndexTime_(time(NULL)),lastUpdateTime_(time(NULL)),maxTime_(30), maxSize_(10000) {
};

void ShareLogicI::Dispatcher::run(){
	MCE_INFO("[Dispatcher]: _dispatchThread started");
	while (true) {
		try {
			invoke();
		} catch (...){
			MCE_WARN("[Dispatcher]: _dispatchThread exception");
		}
	}
}
void ShareLogicI::Dispatcher::swap(){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(thread_mutex_);

	vector<UrlInfo> shareInfos;
	logic_->swapShare(shareInfos);
	shareInfos_.insert(shareInfos_.end(),shareInfos.begin(),shareInfos.end());

	vector<UrlDetailInfo> createInfos;
	logic_->swapCreate(createInfos);
	createInfos_.insert(createInfos_.end(), createInfos.begin(),createInfos.end());

	if(shareInfos_.size() || createInfos_.size())
		thread_mutex_.notify();
}
void ShareLogicI::Dispatcher::invoke() {
	swap();
	invokeAndClearCache();
}

void ShareLogicI::Dispatcher::invokeAndClearCache() {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(thread_mutex_);
	if(size_t(time(NULL) - lastIndexTime_) > maxTime_ || createInfos_.size() > maxSize_){
		std::vector<UrlDetailInfo> create_swap;
		create_swap.swap(createInfos_);
		lastIndexTime_ = time(NULL);
		MyUtil::TaskManager::instance().execute(new NotifyShareIndexTask(create_swap));
	}
	if(size_t(time(NULL) - lastUpdateTime_) > maxTime_ || shareInfos_.size() > maxSize_){
		std::vector<UrlInfo> share_swap;
		share_swap.swap(shareInfos_);
		lastUpdateTime_ = time(NULL);
		MyUtil::TaskManager::instance().execute(new NotifyShareCacheTask(share_swap));
	}	
	thread_mutex_.timedWait(IceUtil::Time::seconds(1));
}
void NotifyShareCacheTask::handle(){
	MCE_INFO("[NotifyShareCacheTask::handle] share "<< infos_.size());
}
void NotifyShareIndexTask::handle(){
	MCE_INFO("[NotifyShareIndexTask::handle] create "<< infos_.size());
}
bool ShareLogicI::isValid(const Ice::Current& current){
	IceUtil::RWRecMutex::RLock lock(state_lock_);
	return state_;
}

void ShareLogicI::setValid(bool newState,
				const Ice::Current& current){
	IceUtil::RWRecMutex::WLock lock(state_lock_);
    state_ = newState;
}

