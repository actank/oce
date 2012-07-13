#include "PassportCacheLoaderAdapter.h"

using namespace xce::passportcache;
using namespace std;
using namespace MyUtil;

PassportCacheLoaderAdapter::PassportCacheLoaderAdapter() {
	_managers.resize(cluster() > 0 ? cluster() : 1);
	_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	_invokeLoad.start(128 * 1024).detach();
}

PassportCacheLoaderPrx PassportCacheLoaderAdapter::getPassportCacheLoader(int id) {
	return PassportCacheLoaderPrx::uncheckedCast(locate<PassportCacheLoaderPrx>(_managers, "M", id, TWO_WAY)->ice_timeout(1000));
}

PassportCacheLoaderPrx PassportCacheLoaderAdapter::getPassportCacheLoaderOneway(int id) {
	return PassportCacheLoaderPrx::uncheckedCast(locate<PassportCacheLoaderPrx>(_managersOneway, "M", id, ONE_WAY));
}

void PassportCacheLoaderAdapter::load(const PassportCacheDataPtr& ticket) {
	_invokeLoad.invoke(ticket);
}

void PassportCacheLoaderAdapter::load(const PassportCacheDataSeq& tickets) {
	_invokeLoad.invoke(tickets);
}

void PassportCacheLoaderAdapter::reload(int userId){
	getPassportCacheLoaderOneway(0)->reload(userId);
}

void PassportCacheLoaderAdapter::doLoad(const PassportCacheDataSeq& tickets) {
	getPassportCacheLoaderOneway(0)->load(tickets);
}

string PassportCacheLoaderAdapter::name() {
	return "M";
}

string PassportCacheLoaderAdapter::endpoints() {
	return "@PassportCacheLoader";
}

size_t PassportCacheLoaderAdapter::cluster(){
	return CLUSTER;
}

void InvokeLoad::invoke(const PassportCacheDataPtr& ticket) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_cache.push_back(ticket);
	if (!_cache.empty()) {
		_mutex.notify();
	}
}

void InvokeLoad::invoke(const PassportCacheDataSeq& tickets) {
	if (tickets.empty()) {
		return;
	}
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_cache.insert(_cache.end(), tickets.begin(), tickets.end());
	if (!_cache.empty()) {
		_mutex.notify();
	}
}

void InvokeLoad::run() {
	while(true) {
		try {
			PassportCacheDataSeq batch;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				if (_cache.empty()) {
					_mutex.wait();
					continue;
				}
				batch.swap(_cache);
			}
			sleep(1);
			PassportCacheLoaderAdapter::instance().doLoad(batch);
		} catch (Ice::Exception &e) {
			MCE_WARN("[InvokeLoad::run] InvokeLoad main loop exception "<<e.what());
		} catch (...) {
			MCE_WARN("[InvokeLoad::run] InvokeLoad main loop exception");
		}
	}
}
