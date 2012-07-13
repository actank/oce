#ifndef __XCELOGGER_REPLICA_ADAPTER_H__
#define __XCELOGGER_REPLICA_ADAPTER_H__

#include "XceLogger.h"
#include "IceExt/src/Channel.h"
#include "IceExt/src/AdapterI.h"
#include "UtilCxx/src/Singleton.h"
#include <boost/lexical_cast.hpp>
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace com {
namespace xiaonei {
namespace service {
using namespace MyUtil;
using namespace ::xce::log;

class XceLoggerReplica;
typedef IceUtil::Handle<XceLoggerReplica> XceLoggerReplicaPtr;
class XceLoggerReplica : virtual public Ice::Object, virtual public IceUtil::Monitor<IceUtil::Mutex> {
public:
	XceLoggerReplica(const string& ctrlEndpoints,int waitSecond = 10, int sendSize = 32768) :
		_clientCS(ctrlEndpoints, 120, 300),
		_cachedEntry(std::vector<LogEntry>()),
		_ctrlEndpoints(ctrlEndpoints),
		_waitSecond(waitSecond),
		_sendSize(sendSize),
		_currentSize(0) 	{

		_clientCS.intialize();
		// _prx = LoggerPrx::uncheckedCast( _ic->stringToProxy(_loggerEndpoints)->ice_datagram() );
	};

	void initialize(){
		_checkThread = new CheckInvoke(this);
		_checkThread->start(65535).detach();				
	};
	
	void addLog(const StringSeq& categories, const std::string& message) {
		LogEntry entry;
		entry.categories = categories;
		entry.message = message;
		append(entry);
		// MCE_DEBUG("[append log] Categories: " << entry.categories[0] << " Message:" << entry.message);
	};

private:
	/* @brief ClusterState的接口 */
	::xce::clusterstate::ClientInterface<LoggerPrx> _clientCS;

	std::vector<LogEntry> _cachedEntry;
	IceUtil::Mutex _mutex;
	string _ctrlEndpoints;

	int _waitSecond;
	int _sendSize;
	int _currentSize;

	void append(LogEntry& entry) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
		if(_cachedEntry.size() > 100000){
			return;
		}
		_cachedEntry.push_back(entry);
		_currentSize += entry.message.size();
		if (_cachedEntry.size() == 1) {
		// MCE_INFO("[XceLoggerReplica]: notify()");
			notify();
		}
	};

	void retrive(std::vector<LogEntry>& toInvokeCache, int& currentSize) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
		while( _cachedEntry.empty() ) {
		// MCE_INFO("[XceLoggerReplica]: timedWait(IceUtil::Time::seconds(10))");
			timedWait(IceUtil::Time::seconds(_waitSecond));
			if( toInvokeCache.size() != 0 ){
				break;
			}
		}
		toInvokeCache.insert(toInvokeCache.end(), _cachedEntry.begin(), _cachedEntry.end());
		currentSize += _currentSize;
		_cachedEntry.clear();
		_currentSize = 0;
		// MCE_INFO("[XceLogger]: reading new LogEntry from cachedEntry ... _cachedEntry.size()="
		//	<< _cachedEntry.size() << "  toInvokeCache.size()=" << toInvokeCache.size() );
	};

	class CheckInvoke: virtual public IceUtil::Thread {
	public:
		CheckInvoke(XceLoggerReplicaPtr xceLogger) :
			_lastInvokeTime(time(NULL)),_xceLogger(xceLogger), _currentSize(0) {
		};
		virtual void run(){
		// MCE_INFO("[XceLoggerReplica]: _checkThread started");
			while (true) {
				try {
					invoke();
					// MCE_DEBUG("[XceLoggerReplica] : retrive " << _toInvokeCache.size());
					_xceLogger->retrive(_toInvokeCache, _currentSize);
				} catch (...) {
					cerr << "[XceLoggerReplica]: CheckInvoke thread exception " << endl;
				}
			}
		};
	private:
		std::vector<LogEntry> _toInvokeCache;
		time_t _lastInvokeTime;
		XceLoggerReplicaPtr _xceLogger;
		int _currentSize;
		void invoke() {
			if (shouldInvoke()){
				invokeAndClearCache();
			}
		};
		bool shouldInvoke() {
			return (_currentSize > _xceLogger->_sendSize || time(NULL) - _lastInvokeTime >_xceLogger->_waitSecond);
		};
		void invokeAndClearCache() {
			std::vector<LogEntry> toInvokeCache_swap;
			_toInvokeCache.swap(toInvokeCache_swap);
			_currentSize = 0;
			// MCE_INFO("[XceLoggerReplica]: shouldInvoke  size = " << toInvokeCache_swap.size() 
			// 	<< "   waited = " << time(NULL) << " - " << _lastInvokeTime );
			_lastInvokeTime = time(NULL);
			try {
				LoggerPrx prx = _xceLogger->_clientCS.getProxyDatagram(0);
				if ( prx != 0 ) {
					prx->append(toInvokeCache_swap);
				}
			} catch (Ice::Exception& e) {
				MCE_WARN("[XceLoggerReplica]:  Ice::Exception " << e);
			} catch (std::exception & e) {
				MCE_WARN("[XceLoggerReplica]:  td::exception ");
			} catch (...) {
				MCE_WARN("[XceLoggerReplica]:  Exception ");
			}
		};
	};

	friend class CheckInvoke;
	typedef IceUtil::Handle<CheckInvoke> CheckInvokePtr;
	CheckInvokePtr _checkThread;
};

class XceLoggerReplicaAdapter: public Singleton<XceLoggerReplicaAdapter> {
public:
	XceLoggerReplicaPtr getLogger(){
		std::string ctrlEndpoints = "";
		return getLogger(ctrlEndpoints);
	};
	XceLoggerReplicaPtr getLogger(int waitSecond, int size = 65536){
		string ctrlEndpoints =  "ControllerXceLogger" ;
                XceLoggerReplicaPtr logger = new XceLoggerReplica(ctrlEndpoints, waitSecond,size);
                return logger;
	};
	XceLoggerReplicaPtr getLogger(std::string& ctrlEndpoints){

		ctrlEndpoints = ctrlEndpoints.empty() ? "ControllerXceLogger" : ctrlEndpoints;
		XceLoggerReplicaPtr logger = new XceLoggerReplica(ctrlEndpoints);
		return logger;
	};
};

}
}
}
#endif

