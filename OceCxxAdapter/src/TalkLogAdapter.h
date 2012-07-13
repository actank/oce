#ifndef __USERACTRECORD_ADAPTER_H__
#define __USERACTRECORD_ADAPTER_H__

#include "AdapterI.h"
#include "TalkLog.h"

namespace xiaonei {
namespace talk {

using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class TalkLogAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, TalkLogAdapter> {
public:

	void feed(const JidPtr& from, const JidPtr& to, const string& act);
	int getCluster() {
		return _cluster;
	}
	;

protected:
	virtual string name() {
		return "TalkLog";
	}
	virtual string endpoints() {
		return "@TalkLog";
	}
	virtual size_t cluster() {
		return 0;
	}
	TalkLogManagerPrx _proxy;
};

};
};

#endif
