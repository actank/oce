#ifndef TALKRIGHTADAPTER_H_
#define TALKRIGHTADAPTER_H_

#include "TalkRight.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace com {
namespace xiaonei {
namespace talk {
namespace adapter {

using namespace MyUtil;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;

class TalkRightAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, TalkRightAdapter> {
public:
	
	TalkRightAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	MyUtil::IntSeq batchVerify(const VerifySeq& seq);
	MyUtil::IntSeq batchVerify(int index, const VerifySeq& seq);

	bool isHiddenUser(int userId);
	void addHiddenUser(int userId);
	void removeHiddenUser(int userId);
	void reloadBlockUser(int userId);
	
	UserStatusPtr getUserStatus(int userId);
	void reloadUserStatus(int userId);
	
	void modifySystemMessageBlock(int userId, const MyUtil::StrSeq& typeSeq);
	MyUtil::StrSeq getSystemMessageBlock(int userId);

	int getCluster() {
		return _cluster;
	}
protected:
	virtual string name() {
		return "TalkRight";
	}
	virtual string endpoints() {
		return "@TalkRight";
	}
	virtual size_t cluster() {
		return 10;
	}
	RightManagerPrx getManager(int id);
	RightManagerPrx getManagerOneway(int id);

	vector<RightManagerPrx> _managersOneway;
	vector<RightManagerPrx> _managers;
};

}
;
}
;
}
;
}
;

#endif /*TALKRIGHTADAPTER_H_*/
