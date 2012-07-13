#ifndef __BATCHUPDATER_ADAPTER_H__
#define __BATCHUPDATER_ADAPTER_H__

#include "BatchUpdater.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace MyUtil;
using namespace mop::hi::svc::model;

const static int INDEX = 0;

class BatchUpdaterReplicaAdapter : 
	public MyUtil::Singleton<BatchUpdaterReplicaAdapter> {
public:
	BatchUpdaterReplicaAdapter();	

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


	void inc(const MyUtil::LongSeq& keyvalue, ::Ice::Int count,
                        const std::string& type);
        void dec(const MyUtil::LongSeq& keyvalue, ::Ice::Int count,
                        const std::string& type);

        void incThreadViewCount(long id, int count=1);
        void incBlogViewCount(long id, int count=1);
        void incUserViewCount(long id, int count=1);

        void incLoginCount(long id, int count=1);

        void setMessageInReadFlag(long id, int flag);
        void setUserStatus(long id, int flag);

        void setLastLoginTime(long id, string time);
        void setLastLogoutTime(long id, string time);
        void setLastModifyTime(long id, string time);

        void incFriendDist(int userId, long universityId, int count=1);
        void decFriendDist(int id, long universityId, int count=1);

        void incFriendCount(long id, int count=1, const Ice::Context& ctx = Ice::Context());
        void decFriendCount(long id, int count=1, const Ice::Context& ctx = Ice::Context());
        void incGuestRequestCount(long id, int count=1,
                        const Ice::Context& ctx = Ice::Context());
        void decGuestRequestCount(long id, int count=1,
                        const Ice::Context& ctx = Ice::Context());

	void incLeavewordCount(long id, int count=1);
        void decLeavewordCount(long id, int count=1);

private:

	xce::clusterstate::ClientInterface<BatchUpdaterManagerPrx> _clientCS;   /* @brief ClusterState的接口 */

	int _cluster;

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

