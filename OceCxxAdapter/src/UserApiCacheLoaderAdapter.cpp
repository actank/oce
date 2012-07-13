#include "UserApiCacheLoaderAdapter.h"

using namespace xce::userapicache;
using namespace std;
using namespace MyUtil;

UserApiCacheLoaderAdapter::UserApiCacheLoaderAdapter() {
	_managers.resize(cluster() > 0 ? cluster() : 1);
	_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	_invokeLoad.start(128 * 1024).detach();
}

UserApiCacheLoaderPrx UserApiCacheLoaderAdapter::getUserApiCacheLoader(int id) {
	return UserApiCacheLoaderPrx::uncheckedCast(locate<UserApiCacheLoaderPrx>(_managers, "M", id, TWO_WAY)->ice_timeout(1000));
}

UserApiCacheLoaderPrx UserApiCacheLoaderAdapter::getUserApiCacheLoaderOneway(int id) {
	return UserApiCacheLoaderPrx::uncheckedCast(locate<UserApiCacheLoaderPrx>(_managersOneway, "M", id, ONE_WAY));
}

void UserApiCacheLoaderAdapter::load(int userId) {
	_invokeLoad.invoke(userId);
}

void UserApiCacheLoaderAdapter::load(const vector<int>& userIds) {
	_invokeLoad.invoke(userIds);
}

void UserApiCacheLoaderAdapter::doLoad(const vector<int>& userIds) {
	getUserApiCacheLoaderOneway(0)->load(userIds);
}

string UserApiCacheLoaderAdapter::name() {
	return "M";
}

string UserApiCacheLoaderAdapter::endpoints() {
	return "@UserApiCacheLoader";
}

size_t UserApiCacheLoaderAdapter::cluster(){
	return CLUSTER;
}

void InvokeLoad::invoke(int userId) {
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
	while(true) {
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
			UserApiCacheLoaderAdapter::instance().doLoad(batch);
		} catch (...) {
			MCE_WARN("[InvokeLoad::run]InvokeLoad main loop exception");
		}
	}
}
