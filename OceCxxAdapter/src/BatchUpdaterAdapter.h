#ifndef __BATCHUPDATER_ADAPTER_H__
#define __BATCHUPDATER_ADAPTER_H__

#include "BatchUpdater.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace MyUtil;
using namespace mop::hi::svc::model;

class BatchUpdaterAdapter : public AdapterI,
	public AdapterISingleton<OceChannel, BatchUpdaterAdapter> {
public:

	void inc(const MyUtil::IntSeq& keyvalue, ::Ice::Int count,
			const std::string& type);
	void dec(const MyUtil::IntSeq& keyvalue, ::Ice::Int count,
			const std::string& type);

	void incThreadViewCount(int id, int count=1);
	void incBlogViewCount(int id, int count=1);
	void incUserViewCount(int id, int count=1);

	void incLoginCount(int id, int count=1);

	void setMessageInReadFlag(int id, int flag);
	void setUserStatus(int id, int flag);

	void setLastLoginTime(int id, string time);
	void setLastLogoutTime(int id, string time);
	void setLastModifyTime(int id, string time);

	void incFriendDist(int userId, int universityId, int count=1);
	void decFriendDist(int id, int universityId, int count=1);

	void incExp(int userId, int count=1);
	void decExp(int userId, int count=1);
	void incTribeCount(int userId, int count=1);
	void decTribeCount(int userId, int count=1);
	void incTribeRequestCount(int userId, int count=1);
	void decTribeRequestCount(int userId, int count=1);
	void incNewMessageCount(int userId, int count=1);
	void decNewMessageCount(int userId, int count=1);
	void incMessageInCount(int userId, int count=1);
	void decMessageInCount(int userId, int count=1);
	void incMessageOutCount(int userId, int count=1);
	void decMessageOutCount(int userId, int count=1);
	void incFriendCount(int id, int count=1, const Ice::Context& ctx = Ice::Context());
	void decFriendCount(int id, int count=1, const Ice::Context& ctx = Ice::Context());
	void incGuestRequestCount(int id, int count=1,
			const Ice::Context& ctx = Ice::Context());
	void decGuestRequestCount(int id, int count=1,
			const Ice::Context& ctx = Ice::Context());
	void incLeavewordCount(int id, int count=1);
	void decLeavewordCount(int id, int count=1);

private:
	virtual void initialize();

	BatchUpdaterManagerPrx _batchUpdaterManager;
	BatchUpdaterManagerPrx _batchUpdaterManagerOneway;

	virtual string name() {
		return "BatchUpdater";
	}
	;
	virtual string endpoints() {
		return "@BatchUpdater";
	}
	;
	virtual size_t cluster() {
		return 0;
	}
	;
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

