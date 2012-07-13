#include "XceStormI.h"
#include "ServiceI.h"
#include "util/cpp/String.h"
#include <util/cpp/InvokeMonitor.h>
#include <Monitor/client/service/ServiceMonitor.h>

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace xce::storm;
using namespace MyUtil;

//---------------------------------------------------------------------------

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&TopicManagerI::instance(), service.createIdentity(TOPIC_MANAGER, ""));
	//	--------------------------------HA--------------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("XceStorm.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("XceStorm.Interval",5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerXceStorm",&TopicManagerI::instance(),mod,interval);
	{ 
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("XceStorm.Status",0);
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status<<" mod : "<<mod<<" interval : "<<interval;
		MCE_WARN( os.str() );
	}
  ServiceMonitorManager::instance().start();
}

void TopicManagerI::notify(const string& name, const Str2StrMap& context,
		const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	IceUtil::Mutex::Lock lock(*this);
	ObserverSetMap::iterator pos = _pool.find(name);
	if (pos != _pool.end()) {
		ObserverSet::iterator it = pos->second.begin();
		for (size_t i = 0; i < pos->second.size() && it != pos->second.end(); ++i, ++it) {
			// The Timer channels for NotifyTimerTask are the range from 0 to 9
			TaskManager::instance().execute(new NotifyTask(i
					% kMaximumTimerTaskChannel, name, *it, context,
					kMaximumTimesOfNotify));
		}
	}
}

void TopicManagerI::subscribe(const string& name, const ObserverPrx& observer,
		const Ice::Current& current) {
  ostringstream oss;
  oss << name << " -> "	<< observer;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	IceUtil::Mutex::Lock lock(*this);
	ObserverSetMap::iterator pos = _pool.find(name);
	ObserverPrx timeoutPrx = ObserverPrx::uncheckedCast(observer->ice_timeout(300)->ice_twoway());
	if (pos != _pool.end()) {
		pos->second.insert(timeoutPrx);
	} else {
		_pool[name].insert(timeoutPrx);
		// the Timer channel for PingTimerTask are the range from -1 to -11,
		// so, all PingTimerTasks are executed in different channels
		// which NotifyTimerTasks are using, thus, these two kinds of TimerTask
		// will not disturb each others
		TaskManager::instance().scheduleRepeated(new PingTimerTask(
				kPeriodOfPingAllInMilliseconds, 0, name));
	}
}

void TopicManagerI::unsubscribe(const string& name,
		const ObserverPrx& observer, const Ice::Current& current) {
  ostringstream oss;
  oss << name << " -> " << observer;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	IceUtil::Mutex::Lock lock(*this);
	ObserverSetMap::iterator pos = _pool.find(name);
	if (pos != _pool.end()) {
		pos->second.erase(observer);
	}
}

ObserverPoolMap TopicManagerI::query(const string& name, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
	ObserverPoolMap result;
	IceUtil::Mutex::Lock lock(*this);
	if (name.empty()) {
		for (ObserverSetMap::iterator i = _pool.begin(); i != _pool.end(); ++i) {
			for (ObserverSet::iterator j = i->second.begin(); j
					!= i->second.end(); ++j) {
				result[i->first].push_back(*j);
			}
		}
	} else {
		ObserverSetMap::iterator pos = _pool.find(name);
		if (pos != _pool.end()) {
			for (ObserverSet::iterator j = pos->second.begin(); j
					!= pos->second.end(); ++j) {
				result[name].push_back(*j);
			}
		}
	}
	return result;
}

void NotifyTask::handle() {
	_observer->update(_context);
	MCE_INFO("NotifyTimerTask::handle success -> " << _name << " -> "
			<< _observer);
}

void NotifyTask::exception(const string& message) {
	if (_retry-- > 0) {
		TaskManager::instance().execute(new NotifyTask(level(), _name,
				_observer, _context, _retry));
	} else {
		TopicManagerI::instance().unsubscribe(_name, _observer);
		MCE_WARN("NotifyTimerTask::handle exception(removed) -> " << _name << " -> "
				<< _observer << " -> " << message);
	}
}

void PingTimerTask::handle() {
	size_t success = 0, failure = 0;
	ObserverPoolMap pool = TopicManagerI::instance().query(_name);
	for (ObserverPoolMap::iterator it = pool.begin(); it != pool.end(); ++it) {
		for (size_t i = 0; i < it->second.size(); ++i) {
			try {
				it->second.at(i)->ice_ping();
				++success;
				MCE_DEBUG("PingTimerTask::handle success -> " << _name << " -> "
						<< it->second.at(i));
			} catch (Ice::Exception& e) {;
				TopicManagerI::instance().unsubscribe(_name, it->second.at(i));
				++failure;
				MCE_WARN("PingTimerTask::handle exception(removed) -> " << _name << " -> "
						<< it->second.at(i) << " -> " << e.what())
			}
		}
	}
	MCE_INFO("PingTimerTask::handle -> summary " << _name << " success: " << success << ", failure: " << failure);
}

