#ifndef OZEINACTIVITYCLIENT_H_
#define OZEINACTIVITYCLIENT_H_

#include <iostream>
#include <Ice/Initialize.h>
#include <memory>
#include <string>
#include <vector>
#include "OzeinActivity.h"
#include "FilterAnswer.h"
#include "FilterQuery.h"
#include "CheckResult.h"

#include "Singleton.h"
#include "AdapterI.h"

namespace com{
namespace xiaonei{
namespace ozein{
namespace activity{
namespace generation{
using namespace MyUtil;
using namespace std;
class UserActivityClient: public MyUtil::AdapterI, public MyUtil::AdapterISingleton<MyUtil::MsgFilterChannel, UserActivityClient> {
public:
	UserActivityPrx getProxy(int id);
	int checkActivity(int fromid, int toid, int type, const string& content);
private:

	virtual string name() {
		return "Anti_Spam_Activity";
	}
	virtual string endpoints() {
		return "@UserActivity";
	}
	virtual size_t cluster() {
		return 0;
	}
	vector<UserActivityPrx> _managers;
};
};
};
};
};
};
#endif /* OZEINACTIVITYCLIENT_H_ */
