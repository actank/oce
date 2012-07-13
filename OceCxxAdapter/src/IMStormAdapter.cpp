#include "IMStormAdapter.h"
#include "util/cpp/String.h"
#include <IceUtil/UUID.h>

using namespace xce::talk;
using namespace MyUtil;

void IMStormAdapter::initialize() {
}


IMStormPrx IMStormAdapter::getManager(int id) {
	return getProxy(id);
}

IMStormPrx IMStormAdapter::getManagerOneway(int id) {
		IMStormPrx r = getProxyOneway(id);
		return r;
}




void IMStormAdapter::UserOnlineStatChange(int userid, int onlinestat, int onlinetype, const MyUtil::IntSeq& notifybuddys){
	getManagerOneway(0)->UserOnlineStatChange(userid, onlinestat, onlinetype, notifybuddys);
}


void IMStormAdapter::deliver(const MessageSeq& msgs){
	getManagerOneway(0)->deliver(msgs);
}













void IMStormAdapter::notify(const string& name, const Str2StrMap& context, int servercount,  int touserid) {
	//getManagerOneway(0)->notify(name, context, touserid);
}

void IMStormAdapter::subscribe(const string& name, const ObserverPtr& o, int phoneservercount, int phoneserverindex) {
	/*
	IceUtil::Mutex::Lock lock(_mutex);
	Ice::ObjectPrx prx = _adapter->addWithUUID(o)->ice_timeout(300);
	_observers[name][o] = prx;
	_managerOneway->subscribe(name, ObserverPrx::uncheckedCast(prx), phoneservercount, phoneserverindex);
	*/
}

void IMStormAdapter::unsubscribe(const string& name, const ObserverPtr& o) {
	/*
	IceUtil::Mutex::Lock lock(_mutex);
	map<Ice::ObjectPtr, Ice::ObjectPrx>::iterator pos =
			_observers[name].find(o);
	if (pos != _observers[name].end()) {
		_managerOneway->unsubscribe(name, ObserverPrx::uncheckedCast(
				pos->second));
		_observers[name].erase(pos);
	}
	*/
}

void IMStormAdapter::ensureSubscription() {
	/*
	ObserverMap observers;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		observers = _observers;
	}

	for (ObserverMap::iterator i = observers.begin(); i != observers.end(); ++i) {
		for (map<Ice::ObjectPtr, Ice::ObjectPrx>::iterator j = i->second.begin(); j != i->second.end(); ++j) {
			_managerOneway->subscribe(i->first, ObserverPrx::uncheckedCast(j->second), 0, 0);
		}
	}*/
}

void SubscriptionGuard::handle() {
	IMStormAdapter::instance().ensureSubscription();
}
