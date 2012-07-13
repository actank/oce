#include <boost/regex.hpp>
#include <mysql++/options.h>
#include "dbpool/connectionpool.h"
#include "dbpool/connectionpoolmanager.h"
#include "base/logging.h"


using namespace com::xiaonei::xce;

ConnectionEntry::ConnectionEntry(const DbInstancePtr& instance, int wrflag) :
	_instance(instance), _wrflag(wrflag), _conn(0), _check(false),
			_action_time(time(NULL)) {
	DbServerPtr server;
	if (wrflag == CDbWServer) {
		server = _instance->wserver;
	} else if (!_instance->rservers.empty()) {
		server = _instance->rservers.at(abs(rand())
				% _instance->rservers.size());
	}
	if (!server) {
		ConnectFailException e("No server instance " + _instance->name);
		LOG(WARNING) << "ConnectionEntry::construct-> No server with flag: " << wrflag
				<< " in " << _instance->name;
		throw e;
	}
	_server = server;
	mysqlpp::Connection* conn = NULL;
	try {
		conn = new mysqlpp::Connection(true);
		try {
			//#ifndef NEWARCH
			//			conn->set_option(mysqlpp::Connection::opt_reconnect, true);
			//			unsigned int conn_timeout_ui = 70;
			//			conn->set_option(mysqlpp::Connection::opt_connect_timeout, conn_timeout_ui);
			//#else
			mysqlpp::ReconnectOption* reconn = new mysqlpp::ReconnectOption(
					true);
			conn->set_option(reconn);
			//#endif
		} catch (mysqlpp::BadOption& e) {
			LOG(WARNING) << "ConnectionEntry::construct -> " << _instance->name << ", " << e.what();
			_check = true;
		}
		try {
			//conn->set_option(mysqlpp::Connection::opt_set_charset_name, server->charset.c_str());
		} catch (mysqlpp::BadOption& e) {
			LOG(WARNING) << "ConnectionEntry::construct -> " << _instance->name << ", " << e.what();
		}
		if (conn->connect(
						server->database.c_str(),
						server->host.c_str(),
						//#ifndef NEWARCH
						//						server->user.c_str(), server->password.c_str(), server->port, 0, 1)) {
						//#else
						server->user.c_str(), server->password.c_str(), server->port)) {
			//#endif
			mysqlpp::Query query = conn->query();
			query << "SET NAMES " << server->charset;
			//#ifndef NEWARCH
			//				LOG(INFO) << "ConnectionEntry::construct -> " << _instance->name << " -> " << query.preview());
			//				mysqlpp::ResNSel rs = query.execute();
			//				if (!rs.success) {
			//#else
			LOG(VERBOSE) << "ConnectionEntry::construct -> " << _instance->name << " -> " << query.str();
			mysqlpp::SimpleResult rs = query.execute();
			if (!rs) {
				//#endif
				delete conn;
				//#ifndef NEWARCH
				//						LOG(WARNING) << "ConnectionEntry::construct -> " + _instance->name + ":" + rs.info);
				//						ConnectFailException e(_instance->name + "->" + rs.info);
				//#else
				LOG(WARNING) << "ConnectionEntry::construct -> " << _instance->name << ":" << rs.info();
				ConnectFailException e(_instance->name + "->" + rs.info());
				//#endif
				throw e;
			}
			_conn = conn;
			LOG(VERBOSE) << "ConnectionEntry::construct -> " << _instance->name << " -> " << _conn;
		}
	} catch (const mysqlpp::Exception& er) {
		if (conn) {
			delete conn;
		}
		// Catch-all for any other MySQL++ exceptions
		ConnectFailException e(_instance->name + "->" + er.what());
		LOG(WARNING) << "ConnectionEntry::construct -> " + _instance->name + ":" + er.what();
		throw e;
	}
}

ConnectionEntry::~ConnectionEntry() {
	if (_conn) {
		LOG(VERBOSE) << "ConnectionEntry::destruct -> " << _instance->name << " -> "
				<< _conn;
		delete _conn;
	}
}

bool ConnectionEntry::validate(const DbInstancePtr& instance, time_t now,
		int timeout) {
	if (_check && _conn) {
		mysqlpp::Query query = _conn->query();
		query << "select version()";
		try {
			query.store();
		} catch (mysqlpp::BadQuery& e) {
			LOG(WARNING) << "ConnectionEntry::" << this << ", " << _instance->name << ", connection broken : " << _conn << " -> " << e.what();
			return false;
		} catch (...) {
			LOG(WARNING) << "ConnectionEntry::" << this << ", " << _instance->name << ", connection broken : " << _conn;
			return false;
		}
	}
	// If the instance is updated, then its timestamp is new than the
	// the ConnectionEntry's _action_time, so the ConnectionEntry shoule
	// be discarded.
	// If the ConnectionEntry is idle for more than timeout duration, then
	// it should be discarded also.
	if (_action_time <= instance->timestamp || timeout < now - _action_time) {
		return false;
	}
	// Aha, this ConnectionEntry is validated, so mark _action_time as now
	// for delay timeout erasing
	_action_time = now;
	return true;
}

mysqlpp::Query ConnectionEntry::query() {
	mysqlpp::Query query = _conn->query();
	query << "SET NAMES " << _server->charset;
	//#ifndef NEWARCH
	//			mysqlpp::ResNSel rs = query.execute();
	//			if (!rs.success) {
	//				LOG(WARNING) << "ConnectionHolder::set names -> " + _instance->name
	//						+ ":" + rs.info);
	//				ConnectFailException e(_instance->name + "->" + rs.info);
	//#else
	mysqlpp::SimpleResult rs = query.execute();
	if (!rs) {
		LOG(WARNING) << "ConnectionHolder::set names -> " << _instance->name << ":"
				<< rs.info();
		ConnectFailException e(_instance->name + "->" + rs.info());
		//#endif
		throw e;
	};
	return query;
}

//---------------------------------------------------------------------------
int ConnectionPool::updated(const DbInstancePtr& instance) {
	IceUtil::Mutex::Lock lock(*this);
	if (instance && instance->type != _instance->type) {
		LOG(VERBOSE) << "ConnectionPoolSingler::" << this << ", " << _instance->name
				<< ", update " << instance->name << ", change type "
				<< _instance->type << " -> " << instance->type;
		return DB_INSTANCE_RETYPED;
	} else if (instance->timestamp > _instance->timestamp) {
		_instance = instance;
		LOG(VERBOSE) << "ConnectionPoolSingler::" << this << ", " << _instance->name << ", update "
				<< instance->name << " success";
		return DB_INSTANCE_UPDATED;
	} else {
		LOG(VERBOSE) << "ConnectionPoolSingler::" << this << ", " << _instance->name
				<< ", update " << instance->name << " unchanged";
		evict();
		return DB_INSTANCE_UNCHANGED;
	}
}

//----------------------------------------------------------------------------
ConnectionPoolSingler::ConnectionPoolSingler(const DbInstancePtr& instance,
		int min, int max, int timeout) :
	ConnectionPool(instance), _min(min), _max(max), _timeout(timeout),
			_rbusyCount(0), _ridleCount(0), _wbusyCount(0), _widleCount(0) {

}

ConnectionPoolSingler::~ConnectionPoolSingler() {

}

ConnectionEntryPtr ConnectionPoolSingler::obtain(int wrflag,
		const string& pattern) {
	bool readable = (wrflag == CDbRServer);
	ConnectionEntryPtr entry;
	bool createNewConnection = false;
	{
		IceUtil::Mutex::Lock lock(*this);
		ConnectionList& idleConns = readable ? _rconns : _wconns;
		int& busyCount = readable ? _rbusyCount : _wbusyCount;
		int& idleCount = readable ? _ridleCount : _widleCount;
		if (!idleConns.empty()) {
			entry = idleConns.front();
			idleConns.pop_front();
			++busyCount;
			--idleCount;
			LOG(VERBOSE) << "ConnectionPoolSingler::" << this << ", "
					<< _instance->name << ", obtain connection -> " << entry
					<< ", read:busy:idle->" << readable << ":" << busyCount
					<< ":" << idleCount;
			return entry;
		}
		createNewConnection = (busyCount < _max);
	}
	if (createNewConnection) {
		entry = new ConnectionEntry(_instance, wrflag);
		if (entry) {
			IceUtil::Mutex::Lock lock(*this);
			int& busyCount = readable ? _rbusyCount : _wbusyCount;
			int& idleCount = readable ? _ridleCount : _widleCount;
			++busyCount;
			LOG(VERBOSE) << "ConnectionPoolSingler::" << this << ", "
					<< _instance->name << ", obtain new connection -> "
					<< entry << ", read:busy:idle->" << readable << ":"
					<< busyCount << ":" << idleCount;
			return entry;
		}
	}

	ConnectionPoolFullException e(_instance->name);
	LOG(WARNING) << "ConnectionPoolSingler::" << this << ", " << _instance->name
			<< ",  obtain connection : " << e.ice_name();
	throw e;
	return NULL;

}
void ConnectionPoolSingler::release(ConnectionEntryPtr& entry,
		const string& pattern) {
	bool validated = entry->validate(_instance, time(NULL), _timeout);
	bool readable = entry->readable();
	IceUtil::Mutex::Lock lock(*this);
	ConnectionList& idleConns = readable ? _rconns : _wconns;
	int& busyCount = readable ? _rbusyCount : _wbusyCount;
	int& idleCount = readable ? _ridleCount : _widleCount;
	if (validated) {
		idleConns.insert(idleConns.begin(), entry);
		++idleCount;
	}
	--busyCount;
	LOG(VERBOSE) << "ConnectionPoolSingler::" << this << ", " << _instance->name
			<< ", release Connection -> " << entry << ", read:busy:idle->"
			<< readable << ":" << busyCount << ":" << idleCount;
}

void ConnectionPoolSingler::evict() {
	time_t now = time(NULL);
	while (!_rconns.empty()) {
		ConnectionEntryPtr entry = _rconns.back();
		if (entry->validate(_instance, now, _timeout)) {
			break;
		}
		_rconns.pop_back();
		--_ridleCount;
		LOG(VERBOSE) << "ConnectionPoolSingler::" << this << ", " << _instance->name
				<< ", evict outdated -> " << entry << ", read -> busy:idle->"
				<< _rbusyCount << ":" << _ridleCount;
	}
	while (!_wconns.empty()) {
		ConnectionEntryPtr entry = _wconns.back();
		if (entry->validate(_instance, now, _timeout)) {
			break;
		}
		_wconns.pop_back();
		--_widleCount;
		LOG(VERBOSE) << "ConnectionPoolSingler::" << this << ", " << _instance->name
				<< ", erase outdated -> " << entry << ", write -> busy:idle->"
				<< _wbusyCount << ":" << _widleCount;
	}
}
//---------------------------------------------------------------------------
ConnectionEntryPtr ConnectionPoolRouter::obtain(int wrflag,
		const string& pattern) {
	ConnectionPoolPtr pool = find(pattern);
	if (pool) {
		return pool->obtain(wrflag, pattern);
	}
	return 0;
}

void ConnectionPoolRouter::release(ConnectionEntryPtr& entry,
		const string& pattern) {
	ConnectionPoolPtr pool = find(pattern);
	if (pool) {
		return pool->release(entry, pattern);
	}
}

ConnectionPoolPtr ConnectionPoolRouter::find(const string& pattern) {
	for (RouteSeq::iterator it = _instance->routes.begin(); it
			!= _instance->routes.end(); ++it) {
		RoutePtr route = *it;
		boost::regex expression(route->expression);
		if (!boost::regex_match(pattern, expression)) {
			LOG(VERBOSE) << "ConnectionPoolRouter::" << this << ", Comparing ["
					<< pattern << "] on [" << route->expression
					<< "] not matched";
			continue;
		}
		LOG(VERBOSE) << "ConnectionPoolRouter::" << this << ",Comparing [" << pattern
				<< "] on [" << route->expression << "] matched";
		ConnectionPoolPtr pool = ConnectionPoolManager::instance().locate(route->instance);
		if (pool) {
			return pool;
		}
	}
	return 0;
}

