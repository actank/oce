#include "CompleteUserInfoAdapter.h"
#include <AdapterI.h> 
#include "Util.h"
using namespace xce::adapter::completeuserinfo;

CompleteUserInfoAdapter::CompleteUserInfoAdapter() : MyUtil::ReplicatedClusterAdapterI<CompleteUserInfoManagerPrx>("ControllerCompleteUserInfo", 120, 300) {
	start(128 * 1024).detach();
}

void CompleteUserInfoAdapter::completeUserInfo(const vector<int>& userIds) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	userIds_.insert(userIds_.end(), userIds.begin(), userIds.end());
	if(userIds_.size() > 100) {
		mutex_.notify();
	}
}

void CompleteUserInfoAdapter::completeUserInfo(int userId) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	userIds_.push_back(userId);
	if(userIds_.size() > 100) {
		mutex_.notify();
	}
}

void CompleteUserInfoAdapter::run() {
	while(true) {
		try {
			vector<int> batch;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
				if(userIds_.empty()) {
					mutex_.timedWait(IceUtil::Time::seconds(2));
				}
				batch.swap(userIds_);
			}
			if(batch.size() == 0) {
				continue;
			}
			getCompleteUserInfoManager(0)->completeUserInfo(batch);
		} catch(...) {
			MCE_WARN("[CompleteUserInfoAdapter::run] Exception");
		}
	}
}

bool CompleteUserInfoAdapter::isValid() {
	return getCompleteUserInfoManager(0)->isValid();
}
void CompleteUserInfoAdapter::setValid(bool valid) {
	return getCompleteUserInfoManager(0)->setValid(valid);
}

CompleteUserInfoManagerPrx CompleteUserInfoAdapter::getCompleteUserInfoManager(int userId) {
	return getProxy(userId);
}
