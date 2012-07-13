#ifndef __XCE_DESCRIPTOR_I_H__
#define __XCE_DESCRIPTOR_I_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "SvcDescriptor.h"
#include "ServiceI.h"
#include "Parser.h"

namespace MyUtil {
using namespace std;

const string SVC_DESCRIPTOR = "SD";

struct ObserverLessTo: public binary_function<SvcObserverPrx, SvcObserverPrx,
		bool> {
	bool operator()(const SvcObserverPrx& __x, const SvcObserverPrx& __y) const {
		return __x->ice_getIdentity() < __y->ice_getIdentity();
	}
};
typedef set<SvcObserverPrx, ObserverLessTo> ObserverSet;
typedef hash_map<string, ObserverSet> ObserverSetMap;

class SvcDescriptorI: public SvcDescriptor,
		public IceUtil::Thread,
		public IceUtil::Mutex {
public:
	virtual SvcInstancePtr query(const string& name, const Ice::Current& =
			Ice::Current());
	virtual void notify(const string& name, const Ice::Current& =
			Ice::Current());

	virtual void subscribe(const string& name, const SvcObserverPrx& observer,
			const Ice::Current& = Ice::Current());
	virtual void unsubscribe(const string& name,
			const SvcObserverPrx& observer, const Ice::Current& =
					Ice::Current());

private:
	virtual void run();

	string checksum(const SvcInstancePtr& svci);

	string _filename;

	ObserverSetMap observers;
	SvcInstanceMap instances;
};

class NotifyTask: public MyUtil::Task {
public:
	NotifyTask(SvcDescriptorI& descriptor, const SvcInstancePtr& instance,
			const Ice::Context& ctx, ObserverSet& observers) :
		_descriptor(descriptor), _instance(instance), _ctx(ctx), _observers(
				observers) {
		;
	}
	virtual void handle();
private:
	SvcDescriptorI& _descriptor;
	SvcInstancePtr _instance;
	Ice::Context _ctx;
	ObserverSet _observers;
};

class PingTask: public MyUtil::Task {
public:
	PingTask(SvcDescriptorI& descriptor, const ObserverSetMap& observers) :
		_descriptor(descriptor), _observers(observers) {
		;
	}
	virtual void handle();
private:
	SvcDescriptorI& _descriptor;
	ObserverSetMap _observers;
};

}
;

#endif
