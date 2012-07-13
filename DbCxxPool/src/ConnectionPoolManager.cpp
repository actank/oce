#include "ConnectionPoolManager.h"
#include "Channel.h"
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

using namespace com::xiaonei::xce;
using namespace boost::asio::ip;
using namespace boost::asio;

void ConnectionPoolManager::initialize() {
	IceUtil::RWRecMutex::WLock lock(_mutex);
	if (_observer || _descriptor) {
		return;
	}
	MyUtil::ChannelPtr ocechannel = new MyUtil::OceChannel;
	Ice::CommunicatorPtr ic = ocechannel->getCommunicator();
	ostringstream adapterEndpoints;
	adapterEndpoints << "tcp -h " << MyUtil::IpAddr::inner();
	_adapter = ic->createObjectAdapterWithEndpoints("DbCxxPool",
			adapterEndpoints.str());
	_adapter->activate();
	Ice::PropertiesPtr props = ic->getProperties();

	string identity = props->getPropertyWithDefault(
			"Service.DbDescriptor.Identity", "DCS@DbDescriptor");

	if (identity != "") {
		_descriptor = DbDescriptorPrx::uncheckedCast(
				ic->stringToProxy(identity));
		MCE_INFO("ConnectionPoolManager::DbDescriptor.Proxy -> "
				<< _descriptor);
	}
	_observer = DbObserverPrx::uncheckedCast(_adapter->addWithUUID(this));
	_alert = XceAlertPrx::uncheckedCast(ic->stringToProxy(props->getPropertyWithDefault("Service.XceAlert.Identity", "XA@XceAlert")));
	MCE_INFO("ConnectionPoolManager::DbObserver.Proxy -> " << _observer);

	MyUtil::TaskManager::instance().schedule(new VerifyTimerTask(10 * 60 ));

	//subscribe to DbDescriptor as a thread, and the sleep interval is subscriberInterval
	_subscriberInterval = 10 ;

	start(65535).detach();
}

DbCxxPoolConnecter::DbCxxPoolConnecter(io_service& ios, bool& flag, const tcp::endpoint& endpoint) : ios_(ios), timer_(ios), socket_(ios), flag_(flag) {
  socket_.async_connect(endpoint, boost::bind(&DbCxxPoolConnecter::connect, this, boost::asio::placeholders::error));
  timer_.expires_from_now(boost::posix_time::milliseconds(5000));
  timer_.async_wait(boost::bind(&DbCxxPoolConnecter::close, this));
}

bool ConnectionPoolManager::isMasterConnected(const std::string& host, int port, const Ice::Current&) {
  bool flag = false;
  try {
    io_service ios;
    tcp::resolver rr(ios);
    tcp::resolver::query qy(host, boost::lexical_cast<string>(port));
    tcp::resolver::iterator i = rr.resolve(qy);
    tcp::resolver::iterator end;
    for (; i != end; ++i) {
      DbCxxPoolConnecter c(ios, flag, *i);
      ios.run();
      if (flag) {
        return true;
      }   
    }   
  } catch (const std::exception& e) {
    MCE_WARN("DbDetector::isConnected std exception " << e.what());
  }
  MCE_WARN("ConnectionPoolManager::isMasterConnected host: " <<host << " port: " << port << " " << flag);
  return false;
}

void ConnectionPoolManager::updated(const DbInstancePtr& instance,
		const Ice::Current&) {
	MCE_INFO("ConnectionPoolManager::updated instance name = " << instance->name);
	ConnectionPoolPtr pool = find(instance->name);
	if (pool) {
		MCE_INFO("ConnectionPoolManager::update DbInstance -> "
				<< instance->name);
		if (pool->updated(instance) == DB_INSTANCE_RETYPED) {
			changeType(instance);
		}
	}
}

ConnectionPoolPtr ConnectionPoolManager::locate(const string& name) {
	ConnectionPoolPtr pool = find(name);
	if (pool) {
		return pool;
	} else {
		MCE_TRACE("ConnectionPoolManager::find pool -> " << name
				<< " none, hooking ... ");
		return create(name);
	}
}

ConnectionPoolPtr ConnectionPoolManager::find(const string& name) {
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
		MCE_WARN("ConnectionPoolManager::fetch NULL db instance -> " << name);
	} catch (Ice::Exception& e) {
		MCE_WARN("ConnectionPoolManager::create -> " << e << " at " << __FILE__ << ":" << __LINE__);
	}
	return 0;
}

bool ConnectionPoolManager::changeType(const DbInstancePtr& instance,
		int min, int max) {
	ConnectionPoolPtr pool;
	{
		IceUtil::RWRecMutex::WLock lock(_mutex);
		ConnectionPoolMap::iterator pos = _pools.find(instance->name);
		if (pos == _pools.end()) {
			return false;
		}
		if (instance->type == CDbInstanceRouter) {
			pool = new ConnectionPoolRouter(instance);
			MCE_INFO("ConnectionPoolManager::changeType from ConnectionSingler to ConnectionPoolRouter -> "
					<< instance->name << "->" << instance);
		} else {
			pool = new ConnectionPoolSingler(instance, min, max);
			MCE_INFO("ConnectionPoolManager::changeType from ConnectionRouter to ConnectionPoolSingler -> "
					<< instance->name << "->" << instance);
		}
		_pools[instance->name] = pool;
	}
	
	try {
		_descriptor->subscribe(instance->name, _observer);
	} catch (Ice::Exception& e) {
		MCE_WARN("ConnectionPoolManager::subscribe -> " << e << " at " << __FILE__ << ":" << __LINE__);
		return false;
	}

	return true;
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
			MCE_TRACE("ConnectionPoolManager::create ConnectionPoolRouter -> "
					<< instance->name << "->" << instance);
		} else {
			pool = new ConnectionPoolSingler(instance, min, max);
			MCE_TRACE("ConnectionPoolManager::create ConnectionPoolSingler -> "
					<< instance->name << "->" << instance);
		}
		_pools[instance->name] = pool;
	}
	try {
		_descriptor->subscribe(instance->name, _observer);
	} catch (Ice::Exception& e) {
		MCE_WARN("ConnectionPoolManager::subscribe -> " << e << " at " << __FILE__ << ":" << __LINE__);
	}
	return pool;
}

void ConnectionPoolManager::verify() {
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
			MCE_WARN("ConnectionPoolManager::verify -> exception: " << e);
		} catch (...) {
			MCE_WARN("ConnectionPoolManager::verify -> exception: unkown reason");
		}
	}
}

void ConnectionPoolManager::sendNeedChangeMsg(const string& instanceName, const DbServerPtr& hostName, bool wrFlag)
{
	
	MCE_WARN("ConnectionPoolManager::sendNeedChangeMsg instance = " << instanceName );
	
	try
	{
		
		//_descriptor->applyBackup(instanceName);

		sendWarnToHuman(instanceName, hostName, wrFlag);
	}catch(...)
	{
		MCE_WARN("ConnectionPoolManager::sendNeedChangeMsg can't notify DbDescriptor to change DB!");
	}
}

void ConnectionPoolManager::sendWarnToHuman(const string& instanceName, const DbServerPtr& hostName, bool wrFlag)
{

	MCE_WARN("ConnectionPoolManager::sendWarnToHuman instance = " << instanceName << " hostName = " << hostName << " wrFlag = " << wrFlag );
	
	try
	{
		string machineName;

		machineName.assign(getenv("NICKNAME"));

		vector<string> typeSeq;

		typeSeq.push_back("DB");
		
		typeSeq.push_back(machineName);
		
		typeSeq.push_back(instanceName);
		
		ostringstream msg;

		msg << hostName->type << ":" << hostName->host << ":" << hostName->port << ":" << hostName->user << ":" << hostName->password << ":" << hostName->database;
    _descriptor->alert(instanceName, msg.str());
	
	} catch (const Ice::Exception& e) {
		MCE_WARN("ConnectionPoolManager::sendWarnToHuman Ice::Exception "<<e);
	} catch(...) {
		MCE_WARN("ConnectionPoolManager::sendWarnToHuman can't notify DbDescriptor to warn people!");
	}
}


void VerifyTimerTask::handle() {
	ConnectionPoolManager::instance().verify();
}

