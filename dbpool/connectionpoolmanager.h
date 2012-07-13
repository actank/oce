#ifndef __XCE_DBCXXPOOL_CONNECTIONPOOLMANAGER_H__
#define __XCE_DBCXXPOOL_CONNECTIONPOOLMANAGER_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "dbpool/connectionpool.h"

namespace com {
namespace xiaonei {
namespace xce {
using namespace std;

const static int LOG_LEVEL_DEBUG = 0;
const static int LOG_LEVEL_INFO = 1;
const static int LOG_LEVEL_WARN = 2;

class ConnectionPoolManager : public DbObserver {
public:
  static ConnectionPoolManager& instance() {
    static ConnectionPoolManager p_;
    return p_;
  }
	void initialize();

	virtual void updated(const DbInstancePtr& instance, const Ice::Current& =
			Ice::Current());

	ConnectionPoolPtr locate(const string& name);
	ConnectionPoolPtr find(const string& name);
	void verify();

private:	
	ConnectionPoolPtr create(const string& name);
	ConnectionPoolPtr hook(const DbInstancePtr& instance, int min = 0, int max = 100);

	DbDescriptorPrx _descriptor;
	DbObserverPrx _observer;

	IceUtil::RWRecMutex _mutex;
	Ice::ObjectAdapterPtr _adapter;
	typedef map<string, ConnectionPoolPtr> ConnectionPoolMap;
	ConnectionPoolMap _pools;
};

#if 0
class VerifyTimerTask: public MyUtil::Timer {
public:
	VerifyTimerTask(int delay) :
		MyUtil::Timer(delay) {
		;
	}

	virtual void handle();
};
#endif

}
}
}
#endif

