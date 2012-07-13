#include "NotifyStormAdapter.h"
#include "util/cpp/String.h"
#include <IceUtil/UUID.h>

using namespace xce::notify;
using namespace MyUtil;

void NotifyStormAdapter::initialize() {
	_managerOneway = locate<NotifyStormPrx> ("M", ONE_WAY);
	_adapter = _channel->getCommunicator()->createObjectAdapterWithEndpoints(
			IceUtil::generateUUID(), "default -h " + IpAddr::inner());
	_adapter->activate();
	TaskManager::instance().scheduleRepeated(new SubscriptionGuard);
}

void NotifyStormAdapter::notify(const string& name, const Str2StrMap& context) {
	_managerOneway->notify(name, context);
}

void NotifyStormAdapter::subscribe(const string& name, const ObserverPtr& o) {
	IceUtil::Mutex::Lock lock(_mutex);
	Ice::ObjectPrx prx = _adapter->addWithUUID(o)->ice_timeout(300);
	_observers[name][o] = prx;
	_managerOneway->subscribe(name, ObserverPrx::uncheckedCast(prx));
}

void NotifyStormAdapter::unsubscribe(const string& name, const ObserverPtr& o) {
	IceUtil::Mutex::Lock lock(_mutex);
	map<Ice::ObjectPtr, Ice::ObjectPrx>::iterator pos =
			_observers[name].find(o);
	if (pos != _observers[name].end()) {
		_managerOneway->unsubscribe(name, ObserverPrx::uncheckedCast(
				pos->second));
		_observers[name].erase(pos);
	}
}

void NotifyStormAdapter::ensureSubscription() {
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
	NotifyStormAdapter::instance().ensureSubscription();
}
