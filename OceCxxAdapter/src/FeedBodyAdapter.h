#ifndef FEED_BODY_ADAPTER_H_
#define FEED_BODY_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "Feed.h"

namespace mop {
namespace hi {
namespace svc {
namespace feed {

using namespace MyUtil;

//-------------------------------------------------------------------------
class FeedBodyAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::OceChannel, FeedBodyAdapter> {
public:
	void deliver(const FeedBodyPtr& feed, Ice::Int target) {
		add(0, target, feed);
	}
private:
	virtual string name() {
		return "FeedBody";
	}
	virtual string endpoints() {
		return "@FeedBody";
	}
	virtual size_t cluster() {
		return 10;
	}
};

}
;
}
;
}
;
}
;

#endif 
