#ifndef __XCE_DB_DESCRIPTORI_h__
#define __XCE_DB_DESCRIPTORI_h__

#include "DbDescriptor.h"
#include "ServiceI.h"
#include "Parser.h"
#include "Singleton.h"
#include <queue>
#include <boost/asio.hpp>

namespace com {
namespace xiaonei {
namespace xce {

const static string DB_CONFIG_SUBJECT = "DCS";
const static int kPeriodOfPingAllInMilliseconds = 20*60*1000;
const static int kPeriodOfPingOneInMilliseconds = 30*1000 ;
const static int kMaximumTimesOfPing = 2;
const static int kPeriodOfNotifyInMilliseconds = 30*1000 ;
const static int kMaximumTimesOfNotify = 3;
const static int kMaximumTimerTaskChannel = 10;
const static long DETECTE_INTERVAL = 15000000;
const static string DB_TEMP_CONFIG_FILE = "/opt/XiaoNei/etc/database_desc.xml.stand";

struct DbObserverLessTo: public binary_function<DbObserverPrx, DbObserverPrx,
		bool> {
	bool operator()(const DbObserverPrx& __x, const DbObserverPrx& __y) const {
		return __x->ice_getIdentity() < __y->ice_getIdentity();
	}
};

class DbInstanceI : public com::xiaonei::xce::DbInstance {
public:
  DbServerPtr sserver;
};

typedef IceUtil::Handle<DbInstanceI> DbInstanceIPtr;

class DbDescriptorI: public DbDescriptor, public MyUtil::Singleton<DbDescriptorI>, public IceUtil::Thread {
public:
	virtual void reload(const Ice::Current& = Ice::Current());

	virtual DbInstanceSeq getDbInstances(const Ice::Current& = Ice::Current());
	virtual DbInstancePtr getDbInstance(const string& name,
			const Ice::Current& = Ice::Current());

	virtual void subscribe(const string& name, const DbObserverPrx& observer,
			const Ice::Current& = Ice::Current());
	virtual void unsubscribe(const string& name, const DbObserverPrx& observer,
			const Ice::Current& = Ice::Current());
	virtual DbObserverMap query(const string& name, const Ice::Current& = Ice::Current());
  void run();
	DbDescriptorI();
	virtual ~DbDescriptorI();
  bool vote(const std::string& name, const std::string& host, int port);
  void cutToStand(const std::string& name);
  void preload();
  void alert(const string& name, const string& dbinfo, const Ice::Current& current = Ice::Current());
private:
  void exportStanding();
	string checksum(const DbInstanceIPtr& instance);
	DbInstanceIPtr buildDbInstanceSingler(const MyUtil::NodePtr& instanceNode);
	DbInstanceIPtr buildDbInstanceRouter(const MyUtil::NodePtr& instanceNode);
	void updated(const DbInstanceIPtr& instance);
  void remove(const std::string& name);

	string _filename;
  string _tempfilename;
	IceUtil::Mutex _mutex;
	typedef set<DbObserverPrx, DbObserverLessTo> DbObserverSet;
	typedef hash_map<string, DbObserverSet> DbObserverSetMap;
	DbObserverSetMap _observers;
	std::map<std::string, DbInstanceIPtr> _instances;
  std::set<std::string> _stands;
};

class DbDetectorEntry;

typedef IceUtil::Handle<DbDetectorEntry> DbDetectorEntryPtr;

class DbDetectorEntry : public Ice::Object {
public:
  DbDetectorEntry(const std::string& name, int count, const IceUtil::Time& time, const std::string& host, int port) :
    name_(name), count_(count), time_(time), host_(host), port_(port) {}
  IceUtil::Time time() const {return time_;}
  void set_time(const IceUtil::Time& time) {time_ = time;}
  int port() const {return port_;}
  std::string host() {return host_;}
  int count() const {return count_;}
  void incCount(int count) {count_ += count;}
  string name() const {return name_;}
private:

  std::string name_;
  int count_;
  IceUtil::Time time_;
  std::string host_;
  int port_;
};

class DbDetectorEntryCompare : public binary_function<DbDetectorEntryPtr, DbDetectorEntryPtr, bool> {
public:
  bool operator()(const DbDetectorEntryPtr& e1, const DbDetectorEntryPtr& e2) const {
    return e1->time() > e2->time();
  }
};

class DbDetector : public MyUtil::Singleton<DbDetector>, public IceUtil::Thread {
public:
  DbDetector() {start(128 * 1024).detach();}
  void detect(const std::string& name, const DbInstancePtr& instance);
  void run();
  bool isConnected(const std::string& host, int port);
private:

  IceUtil::Monitor<IceUtil::Mutex> mutex_;
  std::priority_queue<DbDetectorEntryPtr, std::vector<DbDetectorEntryPtr>, DbDetectorEntryCompare> queue_;
  std::set<std::string> set_;
};

class NotifyTimerTask: public MyUtil::Timer {
public:
	NotifyTimerTask(int delay, int channel, const DbInstancePtr& instance,
			const DbObserverPrx& observer, int retry) :
		MyUtil::Timer(delay, channel), _instance(instance),
				_observer(observer), _retry(retry) {
		;
	}
	virtual void handle();
	virtual void exception(const string& message);
private:
	DbInstancePtr _instance;
	DbObserverPrx _observer;
	int _retry;
};

class PingTimerTask: public MyUtil::Timer {
public:
	PingTimerTask(int delay, int channel, const string& name,
			const DbObserverPrx& observer, int retry) :
		Timer(delay, channel), _name(name), _observer(observer), _retry(retry) {
		;
	}
	virtual void handle();
	virtual void exception(const string& message);
private:
	string _name;
	DbObserverPrx _observer;
	int _retry;
};

class Connecter {
public:
  Connecter(boost::asio::io_service& ios, bool& flag, const boost::asio::ip::tcp::endpoint& endpoint);
  void connect(const boost::system::error_code& err);
  void close();
private:
  boost::asio::io_service& ios_;
  boost::asio::deadline_timer timer_;
  boost::asio::ip::tcp::socket socket_;
  bool& flag_;
};
}
;
}
;
}
;

#endif
