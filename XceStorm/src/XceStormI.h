#ifndef __XCE_STORMI_H__
#define __XCE_STORMI_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "XceStorm.h"
#include "ServiceI.h"
#include <ext/hash_map>
#include "util/cpp/String.h"

namespace xce {
namespace storm {

using namespace std;
using namespace __gnu_cxx;
using namespace MyUtil;

const static string TOPIC_MANAGER = "TM";
const static int kPeriodOfPingAllInMilliseconds = 20*60*1000;
const static int kPeriodOfPingOneInMilliseconds = 30*1000;
const static int kMaximumTimesOfPing = 2;
const static int kPeriodOfNotifyInMilliseconds = 30*1000;
const static int kMaximumTimesOfNotify = 3;
const static int kMaximumTimerTaskChannel = 10;

struct ObserverLessTo: public binary_function<ObserverPrx, ObserverPrx, bool> {
	bool operator()(const ObserverPrx& __x, const ObserverPrx& __y) const {
		return __x->ice_getIdentity() < __y->ice_getIdentity();
	}
};

class TopicManagerI: public TopicManager,
		public IceUtil::Mutex,
		public Singleton<TopicManagerI> {
public:
	virtual void notify(const string& name, const MyUtil::Str2StrMap& context,
			const Ice::Current& = Ice::Current());

	virtual void subscribe(const string& name, const ObserverPrx& observer,
			const Ice::Current& = Ice::Current());
	virtual void unsubscribe(const string& name, const ObserverPrx& observer,
			const Ice::Current& = Ice::Current());

	virtual ObserverPoolMap query(const string& name, const Ice::Current& =
			Ice::Current());
private:
	typedef set<ObserverPrx, ObserverLessTo> ObserverSet;
	typedef hash_map<string, ObserverSet> ObserverSetMap;
	ObserverSetMap _pool;
};

class NotifyTask: public MyUtil::Task {
public:
	NotifyTask(int channel, const string& name,
			const ObserverPrx& observer, const Ice::Context& context,
			int retry) :
		MyUtil::Task(channel), _name(name), _observer(observer),
				_context(context), _retry(retry) {
		;
	}
	virtual void handle();
	virtual void exception(const string& message);
private:
	string _name;
	ObserverPrx _observer;
	Ice::Context _context;
	int _retry;
};

class PingTimerTask: public MyUtil::Timer {
public:
	PingTimerTask(int delay, int channel, const string& name
			) :
		Timer(delay, channel), _name(name) {
		;
	}
	virtual void handle();
private:
	string _name;
};
}
;

}
;

#endif
