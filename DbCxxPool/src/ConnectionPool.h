#ifndef __ConnectionPool_h__
#define __ConnectionPool_h__

#include <iostream>
#include <list>
#include <mysql++/mysql++.h>
#ifdef NEWARCH
#include <mysql++/options.h>
#endif
#include <Ice/Ice.h>
#include <Ice/LocalException.h>
#include <IceUtil/IceUtil.h>
#include "DbDescriptor.h"

namespace com {
namespace xiaonei {
namespace xce {
using namespace std;

const static int DB_INSTANCE_UPDATED = 0;
const static int DB_INSTANCE_RETYPED = 1;
const static int DB_INSTANCE_UNCHANGED = 2;

class NewConnectFailException: virtual public ConnectFailException {
public:
	NewConnectFailException(const std::string& info, const DbServerPtr& server, bool wrflag): ConnectFailException(info), server_(server), readable_(wrflag) {}
	virtual ~NewConnectFailException() throw() {}
	DbServerPtr server() const {return server_;}
	bool readable() const {return readable_;}
private:
	DbServerPtr server_;
	bool readable_;
};

class ConnectionEntry: public IceUtil::Shared {
public:
	ConnectionEntry(const DbInstancePtr& instance, int wrflag);
	~ConnectionEntry();

	bool readable() {
		return _wrflag == CDbRServer;
	}
	bool validate(const DbInstancePtr& instance, time_t now, int timeout);

	mysqlpp::Query query();

	int errnum() {
		return _conn->errnum();
	}
	const char* error() {
		return _conn->error();
	}

	DbServerPtr getServer()
	{
		return _server;
	}
private:
	DbInstancePtr _instance;
	int _wrflag;

	DbServerPtr _server;

	mysqlpp::Connection* _conn;
	bool _check;

	int _action_time;
};
typedef IceUtil::Handle<ConnectionEntry> ConnectionEntryPtr;
typedef list<ConnectionEntryPtr> ConnectionList;

//---------------------------------------------------------------------------
class ConnectionPool: public IceUtil::Shared, public IceUtil::Mutex {
public:
	ConnectionPool(const DbInstancePtr& instance) :
		_instance(instance), _isCut(false) {
	}

	virtual ~ConnectionPool() {
	}

	virtual ConnectionEntryPtr obtain(int wrflag, const string& pattern) = 0;
	virtual void release(ConnectionEntryPtr& conn, const string& pattern) = 0;

	int updated(const DbInstancePtr& instance);

	void tryToDelBadDb(const DbServerPtr& server, const string& pattern, bool wrflag);

	bool compareDbServer(const DbServerPtr& s1, const DbServerPtr& s2);

	virtual void delBadDb(const DbServerPtr& server, const string& pattern, bool wrflag) = 0;

protected:
	virtual void evict() {
	}

	DbInstancePtr _instance;
	bool _isCut;
};
typedef IceUtil::Handle<ConnectionPool> ConnectionPoolPtr;

class ConnectionPoolSingler: public ConnectionPool {
public:
	ConnectionPoolSingler(const DbInstancePtr& instance, int min = 0, int max =
			100, int timeout = 300);
	virtual ~ConnectionPoolSingler();

	virtual ConnectionEntryPtr obtain(int wrflag, const string& pattern);
	virtual void release(ConnectionEntryPtr& conn, const string& pattern);

	virtual void delBadDb(const DbServerPtr& server, const string& pattern, bool wrflag);

private:
	virtual void evict();

	int _min;
	int _max;
	int _timeout;

	ConnectionList _rconns;
	int _rbusyCount;
	int _ridleCount;
	ConnectionList _wconns;
	int _wbusyCount;
	int _widleCount;
};
typedef IceUtil::Handle<ConnectionPool> ConnectionPoolPtr;

class ConnectionPoolRouter: public ConnectionPool {
public:
	ConnectionPoolRouter(const DbInstancePtr& instance) :
		ConnectionPool(instance) {
	}

	virtual ~ConnectionPoolRouter() {
	}

	virtual ConnectionEntryPtr obtain(int wrflag, const string& pattern);
	virtual void release(ConnectionEntryPtr& conn, const string& pattern);

private:
	ConnectionPoolPtr find(const string& pattern);
	
	virtual void delBadDb(const DbServerPtr& server, const string& pattern, bool wrflag);
};

}
}
}

#endif
