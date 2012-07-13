#ifndef _XCESTORM_ADAPTER_H_
#define _XCESTORM_ADAPTER_H_

#include "AdapterI.h"
#include "Singleton.h"
#include "NotifyStorm.h"
#include "TaskManager.h"

namespace xce {
namespace notify {
using namespace std;
using namespace MyUtil;

class NotifyStormAdapter: public AdapterI, public AdapterISingleton<
		MyUtil::NotifyChannel, NotifyStormAdapter> {
public:
	void notify(const string& name, const Str2StrMap& context);
	void subscribe(const string& name, const ObserverPtr& o);
	void unsubscribe(const string& name, const ObserverPtr& o);

	void ensureSubscription();
protected:
	virtual void initialize();
	virtual string name() {
		return "NotifyStorm0";//TODO
	}
	virtual string endpoints() {
		return "@NotifyStorm0";
	}
	virtual size_t cluster() {
		return 0;
	}

private:

	IceUtil::Mutex _mutex;
	NotifyStormPrx _managerOneway;
	Ice::ObjectAdapterPtr _adapter;
	typedef map<string, map<Ice::ObjectPtr, Ice::ObjectPrx> > ObserverMap;
	ObserverMap _observers;
};

class SubscriptionGuard: public MyUtil::Timer {
public:
	SubscriptionGuard() :
		MyUtil::Timer((20 * 60 * 1000 )){
		;
	}
	virtual void handle();
};

};
};

#endif /*WEBEVENTSADAPTER_H_*/
