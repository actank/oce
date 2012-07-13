#ifndef USERSCORE_ACTIVETRACK_H
#define USERSCORE_ACTIVETRACK_H

#include "AdapterI.h"
#include "Singleton.h"
#include "UserScoreActiveTrack.h"

namespace wservice {
namespace userscore {
namespace adapter{

using namespace MyUtil;
using namespace com::xiaonei::wservice::userscore::activetrack;

class UserScoreActiveTrackAdapter: public MyUtil::AdapterI,
		public AdapterISingleton<MyUtil::ScoreChannel, UserScoreActiveTrackAdapter> {
public:
	UserScoreActiveTrackAdapter() {
		_managersUdp.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}

	void incrLoginCount(int userId);
	void incrLoginCountTcp(int userId);


protected:
	virtual string name() {
		return "ActiveTrack";
	}
	virtual string endpoints() {
		return "@ActiveTrack";
	}
	virtual size_t cluster() {
		return 0;
	}

	ActiveTrackLogicPrx  getManagerUdp(int id);
	ActiveTrackLogicPrx  getManagerOneway(int id);

	vector<ActiveTrackLogicPrx > _managersUdp;
	vector<ActiveTrackLogicPrx > _managersOneway;
};

}
}
}
#endif
