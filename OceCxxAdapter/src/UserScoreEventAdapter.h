#ifndef USERSCORE_EVENT_H
#define USERSCORE_EVENT_H

#include "AdapterI.h"
#include "Singleton.h"
#include "UserScoreEvent.h"

namespace wservice {
namespace userscore {
namespace adapter{

using namespace MyUtil;
using namespace com::xiaonei::wservice::userscore::event;

class UserScoreEventAdapter: public MyUtil::AdapterI,
		public AdapterISingleton<MyUtil::ScoreChannel, UserScoreEventAdapter> {
public:
	UserScoreEventAdapter() {
		_managersUdp.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}

	void send(const MyUtil::Str2StrMap&);

protected:
	virtual string name() {
		return "ScoreEvent";
	}
	virtual string endpoints() {
		return "@ScoreEvent";
	}
	virtual size_t cluster() {
		return 0;
	}

	EventSenderPrx  getManagerUdp(int id);
	EventSenderPrx  getManagerOneway(int id);

	vector<EventSenderPrx > _managersUdp;
	vector<EventSenderPrx > _managersOneway;
};

}
}
}
#endif
