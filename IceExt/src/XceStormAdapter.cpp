#include "XceStormAdapter.h"
#include "util/cpp/String.h"
#include <IceUtil/UUID.h>

using namespace xce::storm;
using namespace MyUtil;

void XceStormAdapter::initialize() {
	_managerOneway = locate<TopicManagerPrx> ("TM", ONE_WAY);
	_adapter = _channel->getCommunicator()->createObjectAdapterWithEndpoints(
			IceUtil::generateUUID(), "default -h " + IpAddr::inner());
	_adapter->activate();
	TaskManager::instance().scheduleRepeated(new SubscriptionGuard);
}

void XceStormAdapter::notify(const string& name, const Str2StrMap& context) {
	_managerOneway->notify(name, context);
}

void XceStormAdapter::subscribe(const string& name, const ObserverPtr& o) {
	IceUtil::Mutex::Lock lock(_mutex);
	Ice::ObjectPrx prx = _adapter->addWithUUID(o)->ice_timeout(300);
	_observers[name][o] = prx;
	_managerOneway->subscribe(name, ObserverPrx::uncheckedCast(prx));
}

void XceStormAdapter::unsubscribe(const string& name, const ObserverPtr& o) {
	IceUtil::Mutex::Lock lock(_mutex);
	map<Ice::ObjectPtr, Ice::ObjectPrx>::iterator pos =
			_observers[name].find(o);
	if (pos != _observers[name].end()) {
		_managerOneway->unsubscribe(name, ObserverPrx::uncheckedCast(
				pos->second));
	}
}

void XceStormAdapter::ensureSubscription() {
	ObserverMap observers;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		observers = _observers;
	}
	for (ObserverMap::iterator i = observers.begin(); i != observers.end(); ++i) {
		for (map<Ice::ObjectPtr, Ice::ObjectPrx>::iterator j = i->second.begin(); j != i->second.end(); ++j) {
			_managerOneway->subscribe(i->first, ObserverPrx::uncheckedCast(j->second));
		}
	}
}

void SubscriptionGuard::handle() {
	XceStormAdapter::instance().ensureSubscription();
}
