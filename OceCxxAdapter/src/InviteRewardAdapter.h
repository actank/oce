#ifndef INVITE_REWARD_H
#define INVITE_REWARD_H

#include "AdapterI.h"
#include "Singleton.h"
#include "InviteReward.h"

namespace wservice {
namespace invitereward {
namespace adapter{

using namespace MyUtil;
using namespace com::xiaonei::wService::invitereward;

class InviteRewardAdapter: public MyUtil::AdapterI,
		public AdapterISingleton<MyUtil::WServiceInviteChannel, InviteRewardAdapter> {
public:
	InviteRewardAdapter() {
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}

	void addStar(int userId);


protected:
	virtual string name() {
		return "InviteRewardN";
	}
	virtual string endpoints() {
		return "@InviteRewardN";
	}
	virtual size_t cluster() {
		return 0;
	}

	InviteRewardManagerPrx  getManagerOneway(int id);

	vector<InviteRewardManagerPrx > _managersOneway;
};

}
}
}
#endif
