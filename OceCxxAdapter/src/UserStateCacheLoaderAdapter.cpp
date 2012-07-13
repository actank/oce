#include "UserStateCacheLoaderAdapter.h"

using namespace xce::userstatecache;
using namespace MyUtil;

UserStateCacheLoaderAdapter::UserStateCacheLoaderAdapter() {
	_managers.resize(cluster() > 0 ? cluster() : 1);
        _invokeLoad.start(128 * 1024).detach();
}

UserStateCacheLoaderAdapter::~UserStateCacheLoaderAdapter() {
}

string UserStateCacheLoaderAdapter::name() {
        return "L";
}

string UserStateCacheLoaderAdapter::endpoints() {
        return "@UserStateCacheLoader";
}

size_t UserStateCacheLoaderAdapter::cluster(){
        return CLUSTER;
} 

UserStateCacheLoaderPrx UserStateCacheLoaderAdapter::getUserStateCacheLoader(int id) {
	return UserStateCacheLoaderPrx::uncheckedCast(locate<UserStateCacheLoaderPrx>(_managers, "L", id, TWO_WAY)->ice_timeout(1000));
}

void UserStateCacheLoaderAdapter::load(int id){
	_invokeLoad.invoke(id);
}

void UserStateCacheLoaderAdapter::doLoad(const vector<int>& userIds) {
	try {
		MCE_INFO("UserStateCacheLoaderAdapter::load "<<userIds.size());
		getUserStateCacheLoader(0)->loads(userIds);
	} catch (const Ice::Exception &e) {
		MCE_WARN("UserStateCacheLoaderAdapter::load -> exception "<<e.what());
	}
}

void InvokeLoad::invoke(int userId){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_cache.push_back(userId);
        if (!_cache.empty()) {
                _mutex.notify();
        }
}

void InvokeLoad::invoke(const MyUtil::IntSeq& userIds) {
        if (userIds.empty()) {
                return;
        }
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
        _cache.insert(_cache.end(), userIds.begin(), userIds.end());
        if (!_cache.empty()) {
                _mutex.notify();
        }
}

void InvokeLoad::run() {
        while (true) {
                try {
                        MyUtil::IntSeq batch;
                        {
                                IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
                                if (_cache.empty()) {
                                        _mutex.wait();
                                        continue;
                                }
                                batch.swap(_cache);
                        }
                        sleep(1);
                        UserStateCacheLoaderAdapter::instance().doLoad(batch);
                } catch (...) {
                        MCE_WARN("[InvokeLoad::run]InvokeLoad main loop exception");
                }
        }
}
