#ifndef __XCE_DBCXXPOOL_CONNECTIONPOOLMANAGER_H__
#define __XCE_DBCXXPOOL_CONNECTIONPOOLMANAGER_H__


#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ConnectionPool.h"
#include "Singleton.h"
#include "TaskManager.h"
#include <boost/asio.hpp>
#include "LogWrapper.h"
#include <XceAlert.h>

namespace com {
namespace xiaonei {
namespace xce {
using namespace std;

const static int LOG_LEVEL_DEBUG = 0;
const static int LOG_LEVEL_INFO = 1;
const static int LOG_LEVEL_WARN = 2;

class ConnectionPoolManager: public MyUtil::Singleton<ConnectionPoolManager>,
		public DbObserver, public IceUtil::Thread {
	friend class MyUtil::Singleton<ConnectionPoolManager>;
public:

	void initialize();

	virtual void updated(const DbInstancePtr& instance, const Ice::Current& =
			Ice::Current());
  virtual bool isMasterConnected(const std::string& host, int port, const Ice::Current& = Ice::Current());

	ConnectionPoolPtr locate(const string& name);
	ConnectionPoolPtr find(const string& name);
	void verify();

	void sendWarnToHuman(const string& instanceName, const DbServerPtr& hostName, bool wrFlag);
	
	void sendNeedChangeMsg(const string& instanceName, const DbServerPtr& hostName, bool wrFlag);

	virtual void run()
	{ 
		while(true)
		{
			MCE_DEBUG("ConnectionPoolManager::run one loop start");
      verify();
			/*vector<string> names;
			{
				IceUtil::RWRecMutex::RLock lock(_mutex);
				for (ConnectionPoolMap::iterator it = _pools.begin(); it
						!= _pools.end(); ++it) {
					names.push_back(it->first);
				}
			}
			for (size_t i = 0; i < names.size(); ++i) {
				try {
						DbInstancePtr instance = _descriptor->getDbInstance(names.at(i));
						if (!instance) {
							MCE_WARN("ConnectionPoolManager::run -> Can't find instance name " << names.at(i) << "from DbDescriptor");
							continue;
						}
						ConnectionPoolPtr pool = find(instance->name);
						if ( pool ) {
							_descriptor->subscribe(instance->name, _observer);
						} else {
							MCE_WARN("ConnectionPoolManager::run Can't find instance pool -> " << instance->name);
						}
				} catch (Ice::Exception& e) {
					MCE_WARN("ConnectionPoolManager::run -> exception: " << e);
				} catch (...) {
					MCE_WARN("ConnectionPoolManager::run -> exception: unkown reason");
				}
			}*/
			MCE_DEBUG("ConnectionPoolManager::run one loop end");
			sleep(_subscriberInterval);
		}
	}

private:
	ConnectionPoolManager() {
		;
	}
	~ConnectionPoolManager() {
		;
	}
	ConnectionPoolManager(const ConnectionPoolManager& rhs);
	ConnectionPoolManager& operator=(const ConnectionPoolManager& rhs);

	ConnectionPoolPtr create(const string& name);
	ConnectionPoolPtr hook(const DbInstancePtr& instance, int min = 0, int max =
			100);

	bool changeType(const DbInstancePtr& instance, int min = 0, int max =
			100);
	
	DbDescriptorPrx _descriptor;
	XceAlertPrx _alert;
	DbObserverPrx _observer;

	int _subscriberInterval;

	IceUtil::RWRecMutex _mutex;
	Ice::ObjectAdapterPtr _adapter;
	typedef map<string, ConnectionPoolPtr> ConnectionPoolMap;
	ConnectionPoolMap _pools;

};

class VerifyTimerTask: public MyUtil::Timer {
public:
	VerifyTimerTask(int delay) :
		MyUtil::Timer(delay) {
		;
	}

	virtual void handle();
};

class DbCxxPoolConnecter {
public:
  DbCxxPoolConnecter(boost::asio::io_service& ios, bool& flag, const boost::asio::ip::tcp::endpoint& endpoint);
  void connect(const boost::system::error_code& err) {
    if (err) {
      return;
      timer_.cancel();
    }
    flag_ = true;
    timer_.cancel();
  }
  void close() {
    socket_.close();
  }
private:
  boost::asio::io_service& ios_;
  boost::asio::deadline_timer timer_;
  boost::asio::ip::tcp::socket socket_;
  bool& flag_;
};

}
}
}
#endif

