
#ifndef SCORE_LOGIN_RECORD_H
#define SCORE_LOGIN_RECORD_H

#include "AdapterI.h"
#include "Singleton.h"
#include "ScoreLoginRecord.h"

namespace xce {
namespace userscore {

using namespace MyUtil;
using namespace com::xiaonei::wservice::userscore::login;

class ScoreLoginRecordAdapter: public MyUtil::AdapterI,
		public AdapterISingleton<MyUtil::ScoreChannel, ScoreLoginRecordAdapter> {
public:
	ScoreLoginRecordAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}

	void incLoginCount(int userId,char type);


protected:
	virtual string name() {
		return "LoginRecord";
	}
	virtual string endpoints() {
		return "@LoginRecord";
	}
	virtual size_t cluster() {
		return 10;
	}

	LoginRecordPrx  getManager(int id);
	LoginRecordPrx  getManagerOneway(int id);

	vector<LoginRecordPrx > _managersOneway;
	vector<LoginRecordPrx > _managers;
};

}
}
#endif
