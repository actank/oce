#include "ConnectionPool.h"
#include "ConnectionPoolManager.h"
#include "LogWrapper.h"
#include "util/cpp/String.h"
#include <boost/regex.hpp>
#ifdef NEWARCH
#include <mysql++/options.h>
#endif

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
		MCE_WARN("ConnectionEntry::construct-> No server with flag: " << wrflag
				<< " in " << _instance->name);
		throw NewConnectFailException("No server instance " + _instance->name, server, wrflag);
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
			MCE_WARN("ConnectionEntry::construct -> " << _instance->name << ", " << e.what());
			_check = true;
		}
		try {
			//conn->set_option(mysqlpp::Connection::opt_set_charset_name, server->charset.c_str());
		} catch (mysqlpp::BadOption& e) {
			MCE_WARN("ConnectionEntry::construct -> " << _instance->name << ", " << e.what());
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
			MCE_TRACE("ConnectionEntry::construct -> " << _instance->name << " -> " << query.str());
			mysqlpp::SimpleResult rs = query.execute();
			if (!rs) {
				delete conn;
				MCE_WARN("ConnectionEntry::construct -> " << _instance->name << ":" << rs.info() );
				throw NewConnectFailException(_instance->name + "->" + rs.info(), server, wrflag);
			}
			_conn = conn;
			MCE_TRACE("ConnectionEntry::construct -> " << _instance->name << " -> " << _conn);
		}
	} catch (const mysqlpp::Exception& er) {
		if (conn) {
			delete conn;
		}
		MCE_WARN("ConnectionEntry::construct -> " + _instance->name + ":" + er.what());
		throw NewConnectFailException(_instance->name + "->" + er.what(), server, wrflag);
	}
}

ConnectionEntry::~ConnectionEntry() {
	if (_conn) {
		MCE_TRACE("ConnectionEntry::destruct -> " << _instance->name << " -> "
				<< _conn);
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
			MCE_WARN("ConnectionEntry::" << this << ", " << _instance->name << ", connection broken : " << _conn << " -> " << e.what());
			return false;
		} catch (...) {
			MCE_WARN("ConnectionEntry::" << this << ", " << _instance->name << ", connection broken : " << _conn);
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
	mysqlpp::SimpleResult rs;
	try {
		mysqlpp::Query query = _conn->query();
		query << "SET NAMES " << _server->charset;
		rs = query.execute();
		if (!rs) {
			MCE_WARN("ConnectionHolder::set names -> " << _instance->name << ":"
					<< rs.info());
			NewConnectFailException e(_instance->name + "->" + rs.info(), _server, _wrflag==CDbRServer?true:false);
			throw e;
		};
		return query;
	} catch (const mysqlpp::Exception& e) {
		MCE_WARN("ConnectionEntry::query mysqlpp::Exception "<<e.what());
		NewConnectFailException e(_instance->name, _server, _wrflag==CDbRServer?true:false);
		//#endif
		throw e;
	}
}

//---------------------------------------------------------------------------
int ConnectionPool::updated(const DbInstancePtr& instance) {
	IceUtil::Mutex::Lock lock(*this);
	if (instance && instance->type != _instance->type) {
		MCE_INFO("ConnectionPoolSingler::" << this << ", " << _instance->name
				<< ", update " << instance->name << ", change type "
				<< _instance->type << " -> " << instance->type);
		return DB_INSTANCE_RETYPED;
	} else if (instance->timestamp > _instance->timestamp) {
		_instance = instance;
		_isCut = false;
		MCE_INFO("ConnectionPoolSingler::" << this << ", " << _instance->name << ", update "
				<< instance->name << " success")
		return DB_INSTANCE_UPDATED;
	} else {
		MCE_TRACE("ConnectionPoolSingler::" << this << ", " << _instance->name
				<< ", update " << instance->name << " unchanged");
		evict();
		return DB_INSTANCE_UNCHANGED;
	}
}

void ConnectionPool::tryToDelBadDb(const DbServerPtr& server, const string& pattern, bool wrflag)
{
	MCE_WARN("ConnectionPool::tryToDelBadDb instance = " << _instance->name << " pattern = " << pattern);
	if(wrflag == true) {
		delBadDb(server, pattern, wrflag);
	} else {
		//ConnectionPoolManager::instance().sendNeedChangeMsg(_instance->name, server, wrflag);
	}
}

bool ConnectionPool::compareDbServer(const DbServerPtr& s1, const DbServerPtr& s2) {
	if (s1->type == s2->type
		&& s1->database == s2->database
		&& s1->host == s2->host
		&& s1->port == s2->port
		&& s1->charset == s2->charset
		&& s1->user == s2->user
		&& s1->password == s2->password) {
		return true;
	}
	return false;
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
			MCE_TRACE("ConnectionPoolSingler::" << this << ", "
					<< _instance->name << ", obtain connection -> " << entry
					<< ", read:busy:idle->" << readable << ":" << busyCount
					<< ":" << idleCount);
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
			MCE_TRACE("ConnectionPoolSingler::" << this << ", "
					<< _instance->name << ", obtain new connection -> "
					<< entry << ", read:busy:idle->" << readable << ":"
					<< busyCount << ":" << idleCount);
			return entry;
		}
	}

	ConnectionPoolFullException e(_instance->name);
	MCE_WARN("ConnectionPoolSingler::" << this << ", " << _instance->name
			<< ",  obtain connection : " << e.ice_name());
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
	MCE_TRACE("ConnectionPoolSingler::" << this << ", " << _instance->name
			<< ", release Connection -> " << entry << ", read:busy:idle->"
			<< readable << ":" << busyCount << ":" << idleCount);
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
		MCE_TRACE("ConnectionPoolSingler::" << this << ", " << _instance->name
				<< ", evict outdated -> " << entry << ", read -> busy:idle->"
				<< _rbusyCount << ":" << _ridleCount);
	}
	while (!_wconns.empty()) {
		ConnectionEntryPtr entry = _wconns.back();
		if (entry->validate(_instance, now, _timeout)) {
			break;
		}
		_wconns.pop_back();
		--_widleCount;
		MCE_TRACE("ConnectionPoolSingler::" << this << ", " << _instance->name
				<< ", erase outdated -> " << entry << ", write -> busy:idle->"
				<< _wbusyCount << ":" << _widleCount);
	}
}

void ConnectionPoolSingler::delBadDb(const DbServerPtr& server, const string& pattern, bool wrflag)
{
	MCE_WARN("ConnectionPoolSingler::delBadDb instance = " << _instance->name << " pattern = " << pattern);
	
	IceUtil::Mutex::Lock lock(*this);
	if (_isCut) {
		return;
	}

	DbInstancePtr newInstance = new DbInstance;

	newInstance->name =_instance->name;
	newInstance->timestamp =_instance->timestamp;
	newInstance->type =_instance->type;
	newInstance->wserver =_instance->wserver;
	newInstance->routes =_instance->routes;

	for(DbServerSeq::iterator it = _instance->rservers.begin(); it != _instance->rservers.end(); ++it) {
		if(!compareDbServer(*it, server)) {
			newInstance->rservers.push_back(*it);
		}
	}

	if (newInstance->rservers.size() <= 0) {
		_isCut = true;
		ConnectionPoolManager::instance().sendWarnToHuman(_instance->name, server, wrflag);
		return;
	}
	

	_instance = newInstance;
	
	bool readable = wrflag;
	
	ConnectionList& idleConns = readable ? _rconns : _wconns;

	int& idleCount = readable ? _ridleCount : _widleCount;

	ConnectionList newIdleConns;

	for(ConnectionList::iterator it = idleConns.begin(); it != idleConns.end(); ++it)
	{
		if(compareDbServer((*it)->getServer(), server))
		{
			--idleCount;
		}else
		{
			newIdleConns.insert(newIdleConns.begin(), (*it));
		}
	}

	idleConns = newIdleConns;
	_isCut = true;
	ConnectionPoolManager::instance().sendWarnToHuman(_instance->name, server, wrflag);
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
			MCE_TRACE("ConnectionPoolRouter::" << this << ", Comparing ["
					<< pattern << "] on [" << route->expression
					<< "] not matched");
			continue;
		}
		MCE_TRACE("ConnectionPoolRouter::" << this << ",Comparing [" << pattern
				<< "] on [" << route->expression << "] matched");
		ConnectionPoolPtr pool = ConnectionPoolManager::instance().locate(route->instance);
		if (pool) {
			return pool;
		}
	}
	return 0;
}

void ConnectionPoolRouter::delBadDb(const DbServerPtr& server, const string& pattern, bool wrflag)
{
	MCE_WARN("ConnectionPoolRouter::delBadDb instance = " << _instance->name << " pattern = " << pattern);
	
	ConnectionPoolPtr pool = find(pattern);
	if (pool) {
		return pool->delBadDb(server, pattern, wrflag);
	}
}


