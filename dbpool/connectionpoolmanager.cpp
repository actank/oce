#include "base/logging.h"
#include "base/network.h"
#include "base/asyncall.h"
#include "base/once.h"
#include "arch/rpcuri.h"

#include "dbpool/connectionpoolmanager.h"

using namespace com::xiaonei::xce;

static Ice::CommunicatorPtr xiaonei_grid() {
  const xce::stringpair arr [] = {
    {"Ice.Override.ConnectTimeout", "300"},
    {"IceGrid.InstanceName", "XiaoNei"},
    {"Ice.ThreadPool.Client.StackSize", "65535"},
    {"Ice.ThreadPool.Server.StackSize", "65535"},
    {"Ice.MessageSizeMax", "20480"},
    {"Ice.Default.Host", xce::Network::local_ip().c_str()},

    {"Ice.Default.Locator", "XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"},
    {0, 0} // 标记结束
  };
  return xce::BuildCommunicator(arr);
}

void ConnectionPoolManager::initialize() {
	IceUtil::RWRecMutex::WLock lock(_mutex);
	if (_observer || _descriptor) {
		return;
	}
	
  Ice::CommunicatorPtr ic = xiaonei_grid();
	ostringstream adapterEndpoints;
  adapterEndpoints << "tcp -h " << ::xce::Network::local_ip();
	_adapter = ic->createObjectAdapterWithEndpoints("DbCxxPool",
			adapterEndpoints.str());
	_adapter->activate();
	Ice::PropertiesPtr props = ic->getProperties();

	string identity = props->getPropertyWithDefault(
			"Service.DbDescriptor.Identity", "DCS@DbDescriptor");

	if (identity != "") {
		_descriptor = DbDescriptorPrx::uncheckedCast(
				ic->stringToProxy(identity));
		LOG(INFO) << "ConnectionPoolManager::DbDescriptor.Proxy -> "
				<< _descriptor;
	}
	_observer = DbObserverPrx::uncheckedCast(_adapter->addWithUUID(this));
	LOG(INFO) << "ConnectionPoolManager::DbObserver.Proxy -> " << _observer;

	// MyUtil::TaskManager::instance().schedule(new VerifyTimerTask(10 * 60 ));
  ::xce::Post(boost::bind(&ConnectionPoolManager::verify, this), 10 * 60 * 1000, 0);
}

static XCE_DECLARE_ONCE(once_init);
void Init() {
  ::com::xiaonei::xce::ConnectionPoolManager::instance().initialize();
}

void ConnectionPoolManager::updated(const DbInstancePtr& instance,
		const Ice::Current&) {
	ConnectionPoolPtr pool = find(instance->name);
	if (pool) {
		LOG(INFO) << "ConectionPoolManager::update DbInstance -> "
				<< instance->name;
		if (pool->updated(instance) == DB_INSTANCE_RETYPED) {
			hook(instance);
		}
	}
}

ConnectionPoolPtr ConnectionPoolManager::locate(const string& name) {
  ::xce::XceOnceInit(&once_init, &Init);

	ConnectionPoolPtr pool = find(name);
	if (pool) {
		return pool;
	} else {
		LOG(INFO) << "ConnectionPoolManager::find pool -> " << name
				<< " none, hooking ... ";
		return create(name);
	}
}

ConnectionPoolPtr ConnectionPoolManager::find(const string& name) {
  ::xce::XceOnceInit(&once_init, &Init);

	IceUtil::RWRecMutex::RLock lock(_mutex);
	ConnectionPoolMap::iterator pos = _pools.find(name);
	if (pos != _pools.end()) {
		return pos->second;
	}
	return 0;
}

ConnectionPoolPtr ConnectionPoolManager::create(const string& name) {
	try {
		DbInstancePtr instance = _descriptor->getDbInstance(name);
		if (instance) {
			return hook(instance);
		}
		LOG(WARNING) << "ConnectionPoolManager::fetch NULL db instance -> " << name;
	} catch (Ice::Exception& e) {
		LOG(WARNING) << "ConnectionPoolManager::create -> " << e << " at " << __FILE__ << ":" << __LINE__;
	}
	return 0;
}

ConnectionPoolPtr ConnectionPoolManager::hook(const DbInstancePtr& instance,
		int min, int max) {
	ConnectionPoolPtr pool;
	{
		IceUtil::RWRecMutex::WLock lock(_mutex);
		ConnectionPoolMap::iterator pos = _pools.find(instance->name);
		if (pos != _pools.end()) {
			return pos->second;
		}
		if (instance->type == CDbInstanceRouter) {
			pool = new ConnectionPoolRouter(instance);
			LOG(INFO) << "ConnectionPoolManager::create ConnectionPoolRouter -> "
					<< instance->name << "->" << instance;
		} else {
			pool = new ConnectionPoolSingler(instance, min, max);
			LOG(INFO) << "ConnectionPoolManager::create ConnectionPoolSingler -> "
					<< instance->name << "->" << instance;
		}
		_pools[instance->name] = pool;
	}
	try {
		_descriptor->subscribe(instance->name, _observer);
	} catch (Ice::Exception& e) {
		LOG(WARNING) << "ConnectionPoolManager::subscribe -> " << e << " at " << __FILE__ << ":" << __LINE__;
	}
	return pool;
}

void ConnectionPoolManager::verify() {
  LOG_F(INFO) << "CHECKPOINT";
	vector<string> names;
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
				continue;
			}
			ConnectionPoolPtr pool = find(instance->name);
			if (pool && (pool->updated(instance) == DB_INSTANCE_RETYPED)) {
				hook(instance);
			} else {
				_descriptor->subscribe(instance->name, _observer);
			}
		} catch (Ice::Exception& e) {
			LOG(WARNING) << "ConnectionPoolManager::verify -> exception: " << e;
		} catch (...) {
			LOG(WARNING) << "ConnectionPoolManager::verify -> exception: unkown reason";
		}
	}
}
