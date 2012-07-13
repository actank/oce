#include "DbDescriptorI.h"
#include "Date.h"
#include "util/cpp/String.h"
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <MonitorAdapter.h>
#include <Monitor/client/service/ServiceMonitor.h>
#include <util/cpp/InvokeMonitor.h>

using boost::asio::io_service;
using boost::asio::ip::tcp;
using com::xiaonei::xce::Connecter;
using com::xiaonei::xce::DbDescriptorI;
using com::xiaonei::xce::DbDetector;
using com::xiaonei::xce::DB_CONFIG_SUBJECT;
using com::xiaonei::xce::PingTimerTask;
using com::xiaonei::xce::kPeriodOfPingAllInMilliseconds;
using MyUtil::ServiceI;
using com::xiaonei::xce::DbInstanceSeq;
using com::xiaonei::xce::DbInstanceI;
using com::xiaonei::xce::DbInstancePtr;
using com::xiaonei::xce::DbInstanceIPtr;
using MyUtil::TaskManager;
using com::xiaonei::xce::DbObserverMap;
using com::xiaonei::xce::NotifyTimerTask;
using IceUtil::Time;
using boost::lexical_cast;
using boost::bind;
using boost::filesystem::exists;
using monitor::MonitorAlerterAdapter;
using MyUtil::ServiceMonitorManager;

//---------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&DbDescriptorI::instance(),
			service.createIdentity(DB_CONFIG_SUBJECT, ""));
	TaskManager::instance().scheduleRepeated(new PingTimerTask(
			kPeriodOfPingAllInMilliseconds, -11, "", NULL, 0));
  ServiceMonitorManager::instance().start();
}

//---------------------------------------------------------------------------
DbDescriptorI::DbDescriptorI() {
	ServiceI & service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	_filename = props->getPropertyWithDefault("Service.Config",
			"../etc/database_desc.xml");
  _tempfilename = props->getPropertyWithDefault("Service.Config.Stand","../etc/database_desc.stand");
  srand((unsigned)time(NULL));
  start(128 * 1024).detach();
	preload();
}

DbDescriptorI::~DbDescriptorI() {
}

void DbDescriptorI::preload() {
  if (exists(_tempfilename)) {
    ifstream i(_tempfilename.c_str());
    while (!i.eof()) {
      string line;
      i >> line;
      if (!line.empty()) {
        _stands.insert(line);
      }
    }
  }
	MyUtil::DocumentPtr doc = MyUtil::Parser::parse(_filename);
  MCE_DEBUG("DbDescriptorI::preload -> filename: " << _filename);
	MyUtil::NodeList roots = doc->getChildren();
  if (!roots.size()) {
    MCE_WARN("DbDescriptorI::preload -> roots.size: " << roots.size() << " is invalid");
  	return;
	}
  MyUtil::NodeList instanceNodes = roots[0]->getChildren();
	for (size_t i = 0; i < instanceNodes.size(); ++i) {
	  MyUtil::NodePtr instanceNode = instanceNodes.at(i);
  	if (instanceNode->getName() == "instance") {
	 		DbInstanceIPtr instance;
	  	string type = instanceNode->getAttribute("type");
		  if (type == CDbInstanceRouter) {
  			instance = buildDbInstanceRouter(instanceNode);
	 		} else {
	  		instance = buildDbInstanceSingler(instanceNode);
		  }

      if (instance->type == "singler" && _stands.count(instance->name) > 0) {
        instance->wserver = instance->sserver;
      }
		  updated(instance);
 		}
  }
 	MCE_INFO("DbDescriptorI::preload done total: " << instanceNodes.size());
}

void DbDescriptorI::reload(const Ice::Current&) {
  if (exists(_tempfilename)) {
    throw new XceException("some master is cut!!!");
  } else {
    IceUtil::Mutex::Lock lock(_mutex);
    _stands.clear();
  }
  set<string> filter;
	MyUtil::DocumentPtr doc = MyUtil::Parser::parse(_filename);
	MCE_DEBUG("DbDescriptorI::reload -> filename: " << _filename);
	MyUtil::NodeList roots = doc->getChildren();
	if (!roots.size()) {
		MCE_WARN("DbDescriptorI::reload -> roots.size: " << roots.size() << " is invalid");
		return;
	}
	MyUtil::NodeList instanceNodes = roots[0]->getChildren();
	for (size_t i = 0; i < instanceNodes.size(); ++i) {
		MyUtil::NodePtr instanceNode = instanceNodes.at(i);
		if (instanceNode->getName() == "instance") {
			DbInstanceIPtr instance;
			string type = instanceNode->getAttribute("type");
			if (type == CDbInstanceRouter) {
				instance = buildDbInstanceRouter(instanceNode);
			} else {
				instance = buildDbInstanceSingler(instanceNode);
			}

			updated(instance);
      filter.insert(instance->name);
		}
	}
  {
    DbInstanceSeq nowInstances = getDbInstances();
    for (DbInstanceSeq::iterator it = nowInstances.begin(); it != nowInstances.end(); ++it) {
      if (filter.count((*it)->name) <= 0) {
        remove((*it)->name);
      }
    }
  }
	MCE_INFO("DbDescriptorI::reload done total: " << instanceNodes.size());
}

void DbDescriptorI::remove(const std::string& name) {
  IceUtil::Mutex::Lock lock(_mutex);
  _instances.erase(name);
}

void DbDescriptorI::run() {
  while (true) {
      try {
        MCE_INFO("DbDescriptorI::run begin check");
        DbInstanceSeq myinstances = getDbInstances();
        for (DbInstanceSeq::iterator it = myinstances.begin(); it != myinstances.end(); ++it) {
          if ((*it)->type == "singler" && (*it)->wserver && !DbDetector::instance().isConnected((*it)->wserver->host, (*it)->wserver->port)) {
            DbDetector::instance().detect((*it)->name, *it);
          }
        }
        set<string> stands;
        MCE_INFO("DbDescriptorI::run end check");
        {
          IceUtil::Mutex::Lock lock(_mutex);
          stands=_stands;
        }
        ostringstream oss;
        for (set<string>::iterator i = stands.begin(); i != stands.end(); ++i) {
          oss << *i << " ";
        }
        if (stands.size() > 0) {
          MonitorAlerterAdapter::instance().alert(oss.str(),oss.str()+" is down",monitor::ALERTERALL, 900);
        }
        sleep(60);
    } catch (const Ice::Exception& e) {
      MCE_WARN("DbDescriptorI::run Ice::Exception " << e);
    } catch (const std::exception& e) {
      MCE_WARN("DbDescriptorI::run std::exception " << e.what());
    } catch (...) {
      MCE_WARN("DbDescriptorI::run unknown exception.");
    }
  }
}

DbInstanceSeq DbDescriptorI::getDbInstances(const Ice::Current&) {
	IceUtil::Mutex::Lock lock(_mutex);
	DbInstanceSeq result;
	for (std::map<std::string, DbInstanceIPtr>::iterator it = _instances.begin(); it
			!= _instances.end(); ++it) {
		result.push_back(it->second);
	}
	return result;
}

DbInstancePtr DbDescriptorI::getDbInstance(const string& name,
		const Ice::Current&) {
	IceUtil::Mutex::Lock lock(_mutex);
	std::map<std::string, DbInstanceIPtr>::iterator pos = _instances.find(name);
	if (pos != _instances.end()) {
		MCE_DEBUG("DbDescriptorI::getDbInstance success -> " << name);
		return pos->second;
	}
	MCE_WARN("DbDescriptorI::getDbInstance none -> " << name);
	return 0;
}

void DbDescriptorI::subscribe(const string& name,
		const DbObserverPrx& observer, const Ice::Current&) {
	MCE_DEBUG("DbDescriptorI::subscribe -> " << name << ", " << observer);
	IceUtil::Mutex::Lock lock(_mutex);
	_observers[name].insert(observer);
}

void DbDescriptorI::unsubscribe(const string& name,
		const DbObserverPrx& observer, const Ice::Current&) {
	MCE_DEBUG("DbDescriptorI::unsubscribe -> " << name << ", " << observer);
	IceUtil::Mutex::Lock lock(_mutex);
	DbObserverSetMap::iterator pos = _observers.find(name);
	if (pos != _observers.end()) {
		pos->second.erase(observer);
	}
}

DbInstanceIPtr DbDescriptorI::buildDbInstanceSingler(
		const MyUtil::NodePtr& instanceNode) {
	DbInstanceIPtr instance = new DbInstanceI;
	instance->type = instanceNode->getAttribute("type");
	instance->name = instanceNode->getAttribute("name");
	instance->timestamp
			= MyUtil::Date(instanceNode->getAttribute("timestamp")).time();
	MCE_DEBUG("DbDescriptorI::buildDbInstanceSingler -> " << instance->name
			<< ", Timestamp: " << instance->timestamp);
	MyUtil::NodeList serverNodes = instanceNode->getChildren();
	for (size_t j = 0; j < serverNodes.size(); ++j) {
		MyUtil::NodePtr serverNode = serverNodes.at(j);
		if (serverNode->getName() == "server") {
			DbServerPtr server = new DbServer;
			server->type = serverNode->getAttribute("type");
			server->database = serverNode->getAttribute("database");
			server->host = serverNode->getAttribute("host");
			server->port = boost::lexical_cast<int>(serverNode->getAttribute(
					"port"));
			server->user = serverNode->getAttribute("user");
			server->password = serverNode->getAttribute("password");
			server->charset = serverNode->getAttribute("charset");
			string wrstr = serverNode->getAttribute("wrflag");
			int wrflag = 0;
			try {
				wrflag = boost::lexical_cast<int>(wrstr);
			} catch (...) {
				if (wrstr == "w" || wrstr == "W") {
					wrflag = CDbWServer;
				} else if (wrstr == "r" || wrstr == "R") {
					wrflag = CDbRServer;
				} else if (wrstr == "s" || wrstr == "S") {
					wrflag = CDbSServer;
				} else if (wrstr == "rw" || wrstr == "wr" || wrstr == "RW"
            || wrstr == "WR" || wrstr == "Rw" || wrstr == "Wr" ) { 
          wrflag = CDbWRServer;
        }
			}

			MCE_DEBUG("DbDescriptorI::buildDbInstanceSingler -> Server:" << server->database
					<< ", Type:" << server->type
					<< ", Host:" << server->host
					<< ", Port:" << server->port
					<< ", User:" << server->user
					<< ", Password:" << server->password
					<< ", WRFlag:" << wrflag);
			if (wrflag & CDbWServer) {
				instance->wserver = server;
			}
      if (wrflag & CDbSServer) {
        instance->sserver = server;
      }
			if (wrflag & CDbRServer) {
				instance->rservers.push_back(server);
			}
		}
	}
	return instance;
}

DbInstanceIPtr DbDescriptorI::buildDbInstanceRouter(
		const MyUtil::NodePtr& instanceNode) {
	DbInstanceIPtr instance = new DbInstanceI;
	instance->type = instanceNode->getAttribute("type");
	instance->name = instanceNode->getAttribute("name");
	instance->timestamp
			= MyUtil::Date(instanceNode->getAttribute("timestamp")).time();
	MCE_DEBUG("DbDescriptorI::buildDbInstanceRouter -> " << instance->name
			<< ", Timestamp: " << instance->timestamp);
	MyUtil::NodeList routeNodes = instanceNode->getChildren();
	for (size_t j = 0; j < routeNodes.size(); ++j) {
		MyUtil::NodePtr routeNode = routeNodes.at(j);
		if (routeNode->getName() == "route") {
			string expression = routeNode->getAttribute("expression");
			string name = routeNode->getAttribute("instance");
			RoutePtr route = new Route;
			route->expression = expression;
			route->instance = name;
			instance->routes.push_back(route);
			MCE_DEBUG("DbDescriptorI::buildDbInstanceRouter -> " << expression
					<< " -> " << name);
		}
	}
	return instance;
}

void DbDescriptorI::updated(const DbInstanceIPtr& instance) {
	IceUtil::Mutex::Lock lock(_mutex);
  
	DbInstanceIPtr orig;
	map<string, DbInstanceIPtr>::iterator pos = _instances.find(instance->name);
	if (pos != _instances.end()) {
		orig = pos->second;
		if (checksum(orig) == checksum(instance)) {
      MCE_INFO("DbDescriptorI::updated checksum same for " << instance->name);
			return;
		}
		instance->timestamp = time(NULL);
	}
	_instances[instance->name] = instance;
	// well, the db instance may be the new one or the updated one
	DbObserverSetMap::iterator pos1 = _observers.find(instance->name);
	if (orig && pos1 != _observers.end()) {
		// the db instance is the udpated one
		MCE_INFO("DbDescriptorI::updated -> " << instance->name << ", Old: "
				<< orig->timestamp << ", New: " << instance->timestamp);
		DbObserverSet::iterator it = pos1->second.begin();
		for (size_t i = 0; i < pos1->second.size() && it != pos1->second.end(); ++i, ++it) {
			TaskManager::instance().schedule(new NotifyTimerTask(0, i
					% kMaximumTimerTaskChannel, instance, *it,
					kMaximumTimesOfNotify));
		}
	}
}

string DbDescriptorI::checksum(const DbInstanceIPtr& instance) {
	std::ostringstream oss;
	oss << instance->name << instance->type;
	if (instance->wserver) {
		oss << instance->wserver->type << instance->wserver->database
				<< instance->wserver->host << instance->wserver->port;
		oss << instance->wserver->charset << instance->wserver->user
				<< instance->wserver->password;
	}
  if (instance->sserver) {
    oss << instance->sserver->type << instance->sserver->database
        << instance->sserver->host << instance->sserver->port;
    oss << instance->sserver->charset << instance->sserver->user
        << instance->sserver->password;
  }
	for (DbServerSeq::const_iterator it = instance->rservers.begin(); it
			!= instance->rservers.end(); ++it) {
		oss << (*it)->type << (*it)->database << (*it)->host << (*it)->port;
		oss << (*it)->charset << (*it)->user << (*it)->password;
	}
	for (RouteSeq::const_iterator it = instance->routes.begin(); it
			!= instance->routes.end(); ++it) {
		oss << (*it)->expression << (*it)->instance;
	}
	return MyUtil::StrUtil::md5(oss.str());
}

bool DbDescriptorI::vote(const std::string& name,const std::string& host, int port) {
  MCE_WARN("DbDescriptorI::vote " << name << " " << host << " " << port);
  MonitorAlerterAdapter::instance().alert("dbdescriptor_"+name,name+":"+host+":"+boost::lexical_cast<string>(port)+" is down", monitor::ALERTERALL, 900);
  {
    IceUtil::Mutex::Lock lock(_mutex);
    if (_stands.count(name) > 0) {
      return true;
    }
  }
  size_t count = 0;
  DbObserverMap pool = DbDescriptorI::instance().query(name);
  DbObserverMap::iterator ii = pool.find(name);
  if (ii == pool.end()) {
    MCE_WARN("DbDescriptorI::vote contain no " << name);
    return true;
  }
  for (DbObserverSeq::iterator i = ii->second.begin(); i != ii->second.end(); ++i) {
    try {
      if (!(*i)->isMasterConnected(host, port)){
        count++;
      }
    } catch (const Ice::Exception& e) {
      MCE_WARN("DbDescriptorI::vote Ice::Exception " << e << " for proxy: " << (*i));
    } catch (const std::exception& e) {
      MCE_WARN("DbDescriptorI::vote std::exception " << e.what() << " for proxy: " << (*i));
    } catch (...) {
      MCE_WARN("DbDescriptorI::vote unknown exception for proxy: " << (*i));
    }
  }
  MCE_WARN("DbDescriptorI::vote total: " << pool.size() << " size: " << count);
  return count == pool.size() ? false : true;
  
}

void DbDescriptorI::cutToStand(const std::string& name) {
  MCE_WARN("DbDescriptorI::cutToStand begin " << name);
  ostringstream oss;
  DbInstanceIPtr instance=new DbInstanceI;
  {
    IceUtil::Mutex::Lock lock(_mutex);
    if (_stands.count(name) <= 0) {
      map<string, DbInstanceIPtr>::iterator i = _instances.find(name);
      if (i->second->sserver && i->second->wserver != i->second->sserver) {
        _stands.insert(name);
        instance->type = i->second->type;
        instance->name = i->second->name;
        instance->timestamp = i->second->timestamp;
        instance->routes = i->second->routes;
        instance->rservers = i->second->rservers;
        instance->wserver = instance->sserver = i->second->sserver;
        oss << i->second->wserver->type << ":" 
            << i->second->wserver->host << ":" 
            << i->second->wserver->port << ":" 
            << i->second->wserver->user << ":" 
            << i->second->wserver->password << ":" 
            << i->second->wserver->database << ":" 
            <<i->second->wserver->charset;
      }
    }
  }
  MCE_WARN("DbDescriptorI::cutToStand instance: " << instance);
  if (instance) {
    exportStanding();
    updated(instance);
    MCE_WARN("DbDescriptorI::cutToStand end " << name);
    alert(name, oss.str());
  }
}

void DbDescriptorI::alert(const std::string& name, const std::string& dbinfo, const Ice::Current& current) {
  ostringstream oss;
  oss << "name: " << name << " db: " << dbinfo;
  MyUtil::InvokeClient::create(current, oss.str(), MyUtil::InvokeClient::INVOKE_LEVEL_WARN);
  try {
    MonitorAlerterAdapter::instance().alert("dbdescriptor_"+name,dbinfo+" is down for "+name,monitor::ALERTERALL, 900);
  } catch (const Ice::Exception& e) {
    MCE_WARN("DbDescriptorI::alert Ice::Exception "<< e);
  } catch (...) {
    MCE_WARN("DbDescriptorI::alert unknown exception ");
  }
}

void DbDescriptorI::exportStanding() {
  MCE_WARN("DbDescriptorI::exportStanding to " <<_tempfilename);
  IceUtil::Mutex::Lock lock(_mutex);
  ofstream o(_tempfilename.c_str());
  for (set<string>::iterator i = _stands.begin(); i != _stands.end(); ++i) {
    o << *i << endl;
  }
  o.close();
}

DbObserverMap DbDescriptorI::query(const string& name, const Ice::Current&) {
	DbObserverMap result;
	IceUtil::Mutex::Lock lock(_mutex);
	if (name.empty()) {
		for (DbObserverSetMap::iterator i = _observers.begin(); i
				!= _observers.end(); ++i) {
			for (DbObserverSet::iterator j = i->second.begin(); j
					!= i->second.end(); ++j) {
				result[i->first].push_back(*j);
			}
		}
	} else {
		DbObserverSetMap::iterator pos = _observers.find(name);
		if (pos != _observers.end()) {
			for (DbObserverSet::iterator j = pos->second.begin(); j
					!= pos->second.end(); ++j) {
				result[name].push_back(*j);
			}
		}
	}
	return result;
}

void NotifyTimerTask::handle() {
	_observer->updated(_instance);
	MCE_INFO("NotifyTimerTask::handle success -> " << _instance->name << " -> " << _observer);
}

void NotifyTimerTask::exception(const string& message) {
	if (_retry-- > 0) {
		TaskManager::instance().schedule(new NotifyTimerTask(
				kPeriodOfNotifyInMilliseconds, channel(), _instance, _observer,
				_retry));
	} else {
		MCE_WARN("NotifyTimerTask::handle exception(removed) -> " << _instance->name << " -> "
				<< _observer << " -> " << message);
		DbDescriptorI::instance().unsubscribe(_instance->name, _observer);
	}
}

void PingTimerTask::handle() {
	if (_name.empty() || !_observer) {
		// This is the first time that the timer is executed, so all observers should
		// be tried to touched through many TimerTasks created
		size_t success = 0, failure = 0;
		DbObserverMap pool = DbDescriptorI::instance().query("");
		for (DbObserverMap::iterator it = pool.begin(); it != pool.end(); ++it) {
			for (size_t i = 0; i < it->second.size(); ++i) {
				try {
					it->second.at(i)->ice_ping();
					++success;
				} catch (...) {
					TaskManager::instance().schedule(new PingTimerTask(
							kPeriodOfPingOneInMilliseconds, (i
									% kMaximumTimerTaskChannel + 1) * -1,
							it->first, it->second.at(i), kMaximumTimesOfPing));
					++failure;
				}
			}
		}
		MCE_INFO("PingTimerTask::handle -> summary that success: " << success << ", failure: " << failure);
	} else {
		_observer->ice_ping();
		MCE_DEBUG("PingTimerTask::handle success -> " << _name << " -> "
				<< _observer);
	}
}

void PingTimerTask::exception(const string& message) {
	if (_name.empty() || !_observer) {
		return;
	}
	if (_retry-- > 0) {
		TaskManager::instance().schedule(new PingTimerTask(
				kPeriodOfPingOneInMilliseconds, channel(), _name, _observer,
				_retry));
	} else {
		MCE_WARN("PingTimerTask::handle exception(removed) -> " << _name << " -> "
				<< _observer << " -> " << message);
		DbDescriptorI::instance().unsubscribe(_name, _observer);
	}
}

void DbDetector::detect(const std::string& name, const DbInstancePtr& instance) {
  MCE_DEBUG("DbDetector::detect " << name);
  if (instance->type == "router") {
    return;
  }
  DbServerPtr server = instance->wserver;
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
  int count = set_.count(name);
  MCE_WARN("DbDetector::detect count of "<<name << " is " << count);
  if (count <= 0) {
    set_.insert(name);
    queue_.push(new DbDetectorEntry(name, 0, Time::now()+Time::microSeconds(DETECTE_INTERVAL), server->host, server->port));
    MCE_WARN("DbDetector::detect size of queue is " << queue_.size());
    if (queue_.size() == 1) {
      mutex_.notify();
    }
  }
}

void DbDetector::run() {
  while (true) {
    try {
      IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
      if (queue_.empty()) {
        MCE_INFO("DbDetector::run waiting");
        mutex_.wait();
        continue;
      }
      DbDetectorEntryPtr entry = queue_.top();
      queue_.pop();
      Time left = entry->time() - Time::now();
      if (left.toMicroSeconds() > 0) {
        MCE_INFO("DbDetector::run wait for " << left.toMicroSeconds() << " ms");
        mutex_.timedWait(left);
      }
      MCE_WARN("DbDetector::run name: " << entry->name() << " host: " << entry->host() << " port: " << entry->port() << " check count: " << entry->count());
      if (isConnected(entry->host(), entry->port())) {
        set_.erase(entry->name());
        continue;
      }
      entry->incCount(1);
      if (entry->count() >= 5) {
        if (!DbDescriptorI::instance().vote(entry->name(), entry->host(), entry->port())) {
          DbDescriptorI::instance().cutToStand(entry->name());
        }
        set_.erase(entry->name());
      } else {
        entry->set_time(Time::now() + Time::microSeconds(DETECTE_INTERVAL));
        queue_.push(entry);
      }
    } catch (const Ice::Exception& e) {
      MCE_WARN("DbDetector::run Ice::Exception " << e);
    } catch (const std::exception& e) {
      MCE_WARN("DbDetector::run std::exception " << e.what());
    }catch (...) {
      MCE_WARN("DbDetector::run unknown exception");
    }
  } 
}

bool DbDetector::isConnected(const std::string& host, int port) {
  bool flag = false;
  try {
    io_service ios;
    tcp::resolver rr(ios);
    tcp::resolver::query qy(host, boost::lexical_cast<string>(port));
    tcp::resolver::iterator i = rr.resolve(qy);
    tcp::resolver::iterator end;
    for (; i != end; ++i) {
      Connecter c(ios, flag, *i);
      ios.run();
      if (flag) {
        return flag;
      }
    }
  } catch (const std::exception& e) {
    MCE_WARN("DbDetector::isConnected std exception " << e.what());
  } catch (...) {
    MCE_WARN("DbDetector::isConnected unknown exception");
  }
  MCE_WARN("DbDetector::isConnected host: " << host << " port: " << port << " "<<flag);
  return false;
}

Connecter::Connecter(io_service& ios, bool& flag, const tcp::endpoint& endpoint) : ios_(ios), timer_(ios), socket_(ios), flag_(flag) {
  socket_.async_connect(endpoint, boost::bind(&Connecter::connect, this, boost::asio::placeholders::error));
  timer_.expires_from_now(boost::posix_time::milliseconds(5000));
  timer_.async_wait(boost::bind(&Connecter::close, this));
}

void Connecter::close() {
  socket_.close();
}

void Connecter::connect(const boost::system::error_code& err) {
  if (err) {
    MCE_WARN("Connecter::connect " << err.message());
    timer_.cancel();
    return;
  }
  flag_ = true;
  timer_.cancel();
}

