
#ifndef USER_SCORE_H
#define USER_SCORE_H

#include "AdapterI.h"
#include "Singleton.h"
#include "UserScore.h"

namespace xce {
namespace userscore {

using namespace MyUtil;
using namespace com::xiaonei::wservice::userscore;

class UserScoreAdapter: public MyUtil::AdapterI,
		public AdapterISingleton<MyUtil::ScoreChannel, UserScoreAdapter> {
public:
	UserScoreAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}

	void addScore(int userId,int type,int count);


protected:
	virtual string name() {
		return "UserScore";
	}
	virtual string endpoints() {
		return "@UserScore";
	}
	virtual size_t cluster() {
		return 10;
	}

	UserScoreLogicPrx  getManager(int id);
	UserScoreLogicPrx  getManagerOneway(int id);

	vector<UserScoreLogicPrx > _managersOneway;
	vector<UserScoreLogicPrx > _managers;
};

}
}
#endif
