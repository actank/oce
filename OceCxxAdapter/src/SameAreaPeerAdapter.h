#ifndef SAMEAREAPEERADAPTER_H_
#define SAMEAREAPEERADAPTER_H_

#include <vector>

#include "Singleton.h"
#include "socialgraph/socialgraphutil/socialgraphbase.h"

namespace xce {
namespace socialgraph {

static const std::string BIZ = "OFSA";

//----------------------------------------------------------

struct PItem {
	PItem() {
		uid_ = 0;
		gender_ = 0;	
	}
	int uid_;
	int gender_;
};

//----------------------------------------------------------

class SameAreaPeerAdapter : public SocialGraphBase,
	public MyUtil::Singleton<SameAreaPeerAdapter> {
public:
	SameAreaPeerAdapter() {}
	std::vector<PItem> getAreaPeer(long key, int limit);
};

}
}

#endif
