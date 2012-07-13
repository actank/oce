#include "base/logging.h"
#include "dbpool/connectionholder.h"

using namespace com::xiaonei::xce;

ConnectionHolder::ConnectionHolder(const string& instance, int wrflag,
		const string& pattern) : _instance(instance), _pattern(pattern) {
	ConnectionPoolPtr pool = ConnectionPoolManager::instance().locate(instance);
	if (!pool) {
		LOG_F(WARNING) << "NoSuchConnectionPoolException -> " << instance;
		NoSuchConnectionPoolException e(instance);
		throw e;
	}
	_entry = pool->obtain(wrflag, pattern);
	if (!_entry) {
		ostringstream oss;
		oss << instance << ",pattern[" << pattern << "], wrflag[" << wrflag
				<< "]";
		LOG_F(WARNING) << "NoSuchConnectionException -> " << oss.str();
		NoSuchConnectionException e(oss.str());
		throw e;
	}
}

ConnectionHolder::~ConnectionHolder() {
	if (_entry) {
		ConnectionPoolPtr pool = ConnectionPoolManager::instance().find(_instance);
		if (pool) {
			pool->release(_entry, _pattern);
		}
	}
}

