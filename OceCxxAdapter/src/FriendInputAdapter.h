#ifndef __FRIENDINPUT_ADAPTER_H__
#define __FRIENDINPUT_ADAPTER_H__

#include "FriendInput.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace com {
namespace xiaonei {
namespace services {
namespace FriendInput {

class FriendInputAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton</*MyUtil::SearchFoFChannel*/MyUtil::GadgetsChannel, FriendInputAdapter> {
public:
	FriendInputAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void reLoadFriend(int id);
  vector<int> getFriendInput(int hostId, string condition, int limit);

private:
	virtual string name() {
		return "Manager";
	}
	virtual string endpoints() {
		return "@FriendInput";
	}
	virtual size_t cluster() {
		return 10;
	}
	com::xiaonei::services::FriendInput::FriendInputManagerPrx getFriendInputManager(int id);
	com::xiaonei::services::FriendInput::FriendInputManagerPrx getFriendInputManagerOneway(int id);
  //vector<int> getFriendInput(int hostId, string condition, int limit);
	vector<com::xiaonei::services::FriendInput::FriendInputManagerPrx> _managers;
	vector<com::xiaonei::services::FriendInput::FriendInputManagerPrx> _managersOneway;
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

