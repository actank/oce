#include "UserAdsCacheLoaderAdapter.h"

using namespace xce::useradscache;
using namespace std;
using namespace MyUtil;

UserAdsCacheLoaderAdapter::UserAdsCacheLoaderAdapter() {
	_managers.resize(cluster() > 0 ? cluster() : 1);
	_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	_invokeLoad.start(128 * 1024).detach();
}

UserAdsCacheLoaderPrx UserAdsCacheLoaderAdapter::getUserAdsCacheLoader(int id) {
	return UserAdsCacheLoaderPrx::uncheckedCast(locate<UserAdsCacheLoaderPrx>(_managers, "M", id, TWO_WAY)->ice_timeout(1000));
}

UserAdsCacheLoaderPrx UserAdsCacheLoaderAdapter::getUserAdsCacheLoaderOneway(int id) {
	return UserAdsCacheLoaderPrx::uncheckedCast(locate<UserAdsCacheLoaderPrx>(_managersOneway, "M", id, ONE_WAY));
}

void UserAdsCacheLoaderAdapter::load(int userId) {
	_invokeLoad.invoke(userId);
}

void UserAdsCacheLoaderAdapter::load(const vector<int>& userIds) {
	_invokeLoad.invoke(userIds);
}

void UserAdsCacheLoaderAdapter::doLoad(const vector<int>& userIds) {
	getUserAdsCacheLoaderOneway(0)->load(userIds);
}

string UserAdsCacheLoaderAdapter::name() {
	return "M";
}

string UserAdsCacheLoaderAdapter::endpoints() {
	return "@UserAdsCacheLoader";
}

size_t UserAdsCacheLoaderAdapter::cluster(){
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
			UserAdsCacheLoaderAdapter::instance().doLoad(batch);
		} catch (...) {
			MCE_WARN("[InvokeLoad::run]InvokeLoad main loop exception");
		}
	}
}
