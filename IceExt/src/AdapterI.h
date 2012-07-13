#ifndef __ADAPTER_I_H__
#define __ADAPTER_I_H__

#include <sstream>
#include "IceExt/src/Channel.h"
#include "ObjectCache.h"
#include "IceUtil/Thread.h"
#include <IceUtil/UUID.h>
#include "UtilCxx/src/Date.h"
#include "IceLogger.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace MyUtil {
using namespace std;
//提供使用了Controller的HA结构的服务Adapter
template<class T> class ReplicatedClusterAdapterI {
public:
	ReplicatedClusterAdapterI (string endpoint, int interval, int timeout,const MyUtil::ChannelPtr& channel=new MyUtil::OceChannel,
							   bool isMutilServices = false, string identifier = ""):
							   _clientCS(endpoint, interval, timeout, channel, isMutilServices, identifier) 
	{
		_clientCS.intialize();
	}

	virtual ~ReplicatedClusterAdapterI() {
	}

	map<int, std::vector<T> > getAvailableProxies() {
		return _clientCS.getAvailableProxies();
	}

	map<int, std::vector<T> > getAvailableProxiesOneway() {
		return _clientCS.getAvailableProxiesOneway();
	}

	T getProxy(long mod) {
		return _clientCS.getProxy(mod);
	}

	vector<T> getProxySeq(long mod) {
		return _clientCS.getProxySeq(mod);
	}

  vector<T> getValidProxySeq(long mod) {
		return _clientCS.getValidProxySeq(mod);
  }

  vector<T> getInvalidProxySeq(long mod) {
		return _clientCS.getInvalidProxySeq(mod);
  }

	vector<T> getAllProxySeq(int mod) {
		return _clientCS.getAllProxySeq(mod);
	}

	vector<T> getAllProxySeqOneway(int mod) {
		return _clientCS.getAllProxySeqOneway(mod);
	}

	T getProxyOneway(long mod) {
		return _clientCS.getProxyOneway(mod);
	}

	T getProxyBatchOneway(long mod) {
		return _clientCS.getProxyBatchOneway(mod);
	}

	T getProxyDatagram(long mod) {
		return _clientCS.getProxyDatagram(mod);
	}

	int getCluster() {
		return _clientCS.getCluster();
	}

private:
	xce::clusterstate::ClientInterface<T> _clientCS;
};

//---------------------------------------------------------------------------
template<class C, class A> class AdapterISingleton {
public:
	static A& instance() {
		if (!__instance) {
			IceUtil::Mutex::Lock lock(__mutex );
			if (!__instance) {
				A* instance = new A;
				instance->configure(new C);
				__instance = instance;
			}
		}
		return *__instance;
	}

protected:
	AdapterISingleton() {
	}
	virtual ~AdapterISingleton() {
		__instance = 0;
	}

	AdapterISingleton(const AdapterISingleton& rhs);
	AdapterISingleton& operator=(const AdapterISingleton& rhs);

protected:
	// __instance and __mutex that with __ prefix are to avoiding name ambiguous
	static A* __instance;
	static IceUtil::Mutex __mutex;
};

template<class C, class A> A* AdapterISingleton<C, A>::__instance = 0;
template<class C, class A> IceUtil::Mutex AdapterISingleton<C, A>::__mutex;

//---------------------------------------------------------------------------
class AdapterI: public IceUtil::Mutex {
public:
	void configure(const ChannelPtr& channel);

	Ice::ObjectPtr get(Ice::Int category, Ice::Long id, int timeout = 0);
	ObjectResultPtr get(Ice::Int category, const MyUtil::LongSeq& ids,
			bool parallel = false, int timeout = 0);

	void add(Ice::Int category, Ice::Long id, const Ice::ObjectPtr& o);
	void add(Ice::Int category, const ObjectMap& os);

	void remove(Ice::Int category, Ice::Long id);
	void remove(Ice::Int category, const MyUtil::LongSeq& ids);

	bool has(Ice::Int category, Ice::Long id, int timeout = 0);
	bool has(Ice::Int category, const MyUtil::LongSeq& ids, int timeout = 0);

	template<class Prx> Prx locate(const string& name, InvokeType it,
			int timeout = 260) {
		return locate<Prx> (name, _endpoints, it, timeout);
	}
	template<class Prx> Prx locate(const string& name, const string& endpoint,
			InvokeType it, int timeout) {
		return _channel->uncheckedCast<Prx> (_channel->locate(name, endpoint,
				it, timeout));
	}
	template<class Prx, class Id> Prx locate(vector<Prx>& holder,
			const string& name, Id id, InvokeType it, int timeout = 260) {
		size_t count = cluster() > 0 ? cluster() : 1;
		if (holder.size() < count || !holder.at(0)) {
			IceUtil::Mutex::Lock lock(*this);
			if (holder.size() < count || !holder.at(0)) {
				holder.resize(count);
				for (int i = count-1; i >= 0; --i) {
					ostringstream endpoints;
					endpoints << _endpoints;
					if (_cluster > 0) {
						endpoints << i;
					}
					holder[i]=locate<Prx> (name, endpoints.str(), it,
							timeout);
				}
			}
		}
		if (_cluster > 0) {
			return holder.at(abs((int)(id % count)));
		} else {
			return holder.at(0);
		}
	}

	ObjectCachePrx locate(Ice::Int id, int timeout = 260);

protected:
	AdapterI();
	virtual ~AdapterI();

	AdapterI(const AdapterI& rhs);
	AdapterI& operator=(const AdapterI& rhs);

	virtual void initialize() {
		;
	}
	virtual string name() = 0;
	virtual string endpoints() = 0;
	virtual size_t cluster() = 0;

	string _name;
	string _endpoints;
	volatile size_t _cluster;

	MyUtil::ChannelPtr _channel;
	Ice::ObjectAdapterPtr _adapter;
	Ice::PropertiesPtr _props;
	vector<ObjectCachePrx> _caches;
};

class ReplicatedAdapterI: public AdapterI {
public:
	template<class Prx> Prx locate(const string& name, InvokeType it,
			int timeout = 260) {
		return _channel->uncheckedCast<Prx> (select_best_proxy(
				AdapterI::locate<Prx>(name, it, timeout)));
	}
	template<class Prx> Prx locate(vector<Prx>& holder, const string& name,
			int id, InvokeType it, int timeout = 260) {
		return _channel->uncheckedCast<Prx> (select_best_proxy(
				AdapterI::locate<Prx>(holder, name, id, it, timeout)));
	}
	template<class Prx> vector<Prx> locateAll(vector<Prx>& holder,
			const string& name, int id, InvokeType it, int timeout = 260) {
		vector<Prx> results;
		vector<Ice::ObjectPrx> proxies = select_all_proxy(
				AdapterI::locate<Prx>(holder, name, id, it, timeout));
		for (vector<Ice::ObjectPrx>::iterator it = proxies.begin(); it
				!= proxies.end(); ++it) {
			results.push_back(_channel->uncheckedCast<Prx> ((*it)));
		}
		return results;
	}
protected:
	virtual bool isValid(const Ice::ObjectPrx& proxy) = 0;
	virtual void initialize();
private:
	IceUtil::Mutex _replicated_proxies_mutex;
	typedef vector<Ice::ObjectPrx> ProxySeq;
	typedef pair<ProxySeq, ProxySeq> ProxySeqPair;
	typedef map<Ice::ObjectPrx, ProxySeqPair> ReplicatedProxyCache;
	ReplicatedProxyCache _replicated_proxies_cache;
	/**
	 *  Must hold the lock of _replicaed_proxies_mutex before call ensure_replicated_proxy_in_cache
	 **/
	void ensure_replicated_proxy_in_cache(const Ice::ObjectPrx& replicated);
	Ice::ObjectPrx select_best_proxy(const Ice::ObjectPrx& replicated);
	vector<Ice::ObjectPrx> select_all_proxy(const Ice::ObjectPrx& replicated);

	class ValidateThread: virtual public IceUtil::Thread {
	public:
		ValidateThread(ReplicatedAdapterI& replicated_adapter) :
			_replicated_adapter(replicated_adapter) {
		}
		virtual void run();
	private:
		ReplicatedAdapterI& _replicated_adapter;
	};

	IceUtil::Handle<ValidateThread> _validate_thread;
};
//---------------------------------------------------------------------------
class Collector: public IceUtil::Shared {
public:
	Collector(AdapterI& adapter, size_t cluster, Ice::Int category,
			const MyUtil::LongSeq& ids, int timeout);
	virtual ~Collector();

	virtual ObjectResultPtr execute() = 0;
protected:
	AdapterI& _adapter;
	size_t _cluster;
	Ice::Int _category;
	MyUtil::LongSeq _ids;
	int _timeout;

	MyUtil::ObjectMap _result;
	map<int, LongSeq> _targets;
};
typedef IceUtil::Handle<Collector> CollectorPtr;

//---------------------------------------------------------------------------
class SerialCollector: public Collector {
public:
	SerialCollector(AdapterI& adapter, size_t cluster, Ice::Int category,
			const MyUtil::LongSeq& ids, int timeout);
	virtual ObjectResultPtr execute();
};

//---------------------------------------------------------------------------
class ParallelCollector: public Collector, public IceUtil::Monitor<
		IceUtil::Mutex> {
public:
	ParallelCollector(AdapterI& adapter, size_t cluster, Ice::Int category,
			const MyUtil::LongSeq& ids, int timeout);
	virtual ObjectResultPtr execute();

private:
	typedef IceUtil::Handle<ParallelCollector> ParallelCollectorPtr;

	int _count;
	bool _done;

	void finish(const ObjectMap& data);

	class CollectTask: public MyUtil::Task {
	public:
		CollectTask(ParallelCollectorPtr callback, int index, int timeout);
		virtual void handle();
	private:
		ParallelCollectorPtr _action;
		int _index;
		int _timeout;
	};
};

}
;
#endif
