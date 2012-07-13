#ifndef __RECOMMEND_FRIEND_ADAPTER_H__
#define __RECOMMEND_FRIEND_ADAPTER_H__

#include "RecommendFriend.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace xce {
namespace recommendfriend {
namespace adapter {

using namespace MyUtil;
using namespace com::xiaonei::wService;

class RecommendFriendAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::WServiceChannel, RecommendFriendAdapter>,
	virtual public IceUtil::Monitor<IceUtil::Mutex>  {
public:
	RecommendFriendAdapter() {
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void removeRecommendFriend(int ownerId, int otherId);
private:
	virtual string name() {
		return "M";
	}
	virtual string endpoints() {
		return "@RecommendFriend";
	}
	virtual size_t cluster() {
		return 1;
	}
	RecommendFriendPrx getRecommendFriendManagerOneway(int id);
	vector<RecommendFriendPrx> _managersOneway;

};

};
};
};

#endif

