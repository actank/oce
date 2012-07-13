#include "ConnectionHolder.h"
#include "LogWrapper.h"

using namespace com::xiaonei::xce;

ConnectionHolder::ConnectionHolder(const string& instance, int wrflag,
		const string& pattern) : _instance(instance), _pattern(pattern) {
	_pool = ConnectionPoolManager::instance().locate(instance);
	if ( !_pool ) {
		MCE_WARN("ConnectionHolder::ConnectionHolder -> NoSuchConnectionPoolException -> " << instance);
		NoSuchConnectionPoolException e(instance);
		throw e;
	}
	_entry = _pool->obtain(wrflag, pattern);
	if (!_entry) {
		ostringstream oss;
		oss << instance << ",pattern[" << pattern << "], wrflag[" << wrflag
				<< "]";
		MCE_WARN("ConnectionHolder::ConnectionHolder -> NoSuchConnectionException -> " << oss.str());
		NoSuchConnectionException e(oss.str());
		throw e;
	}
}

ConnectionHolder::~ConnectionHolder() {
	if (_entry) {
		_pool = ConnectionPoolManager::instance().find(_instance);
		if ( _pool ) {
			_pool->release(_entry, _pattern);
		}
	}
}

