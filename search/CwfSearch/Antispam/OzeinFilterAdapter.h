#ifndef OZEINFILTERCLIENT_H_
#define OZEINFILTERCLIENT_H_

#include <iostream>
#include <Ice/Initialize.h>
#include <memory>
#include <string>
#include <vector>
#include "OzeinContent.h"
#include "FilterAnswer.h"
#include "FilterQuery.h"
#include "CheckResult.h"

#include "Singleton.h"
#include "AdapterI.h"

namespace com{
namespace xiaonei{
namespace ozein{
namespace content{
namespace generation{
using namespace MyUtil;
using namespace std;
class OzeinFilterClient: public MyUtil::AdapterI, public MyUtil::AdapterISingleton<MyUtil::MsgFilterChannel, OzeinFilterClient> {
public:
	FilterPrx getManager(int id);
	FilterPrx getManagerOneway(int id);
	void init_data(Ice::InitializationData& initData);
	CheckResultMore checkerAndFilter(int fromUserId, int toId, int type, const vector<string>& contentV, const string& userIp);
	FilterAnswer ozeinContent(FilterQuery query);
private:

	virtual string name() {
		return "filter";
	}
	virtual string endpoints() {
		return "@OzeinFilter";
	}
	virtual size_t cluster() {
		return 0;
	}
	vector<FilterPrx> _managersOneway;
	vector<FilterPrx> _managers;
};
};
};
};
};
};
#endif /* OZEINFILTERCLIENT_H_ */
