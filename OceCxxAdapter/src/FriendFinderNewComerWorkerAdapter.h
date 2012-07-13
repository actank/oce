#ifndef __FRIENDFINDER_NEWCOMER_WORKER_ADAPTER_H__
#define __FRIENDFINDER_NEWCOMER_WORKER_ADAPTER_H__

#include "SocialGraphWorker.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace xce {
namespace socialgraph {
namespace adapter{

using namespace MyUtil;
using namespace ::xce::socialgraph;

class FriendFinderNewComerWorkerAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SocialGraphWorkerPrx>, public MyUtil::Singleton<FriendFinderNewComerWorkerAdapter> {
public:
	FriendFinderNewComerWorkerAdapter() :
		MyUtil::ReplicatedClusterAdapterI <xce::socialgraph::SocialGraphWorkerPrx>("ControllerFriendFinderNewComerWorker", 120, 1000) {
	}

	void apply(int applicant, int acceptor);
	void accept(int acceptor, int applicant);
	void deny(int hostId, int guestId);
	void ignore(int userId, const MyUtil::IntSeq& ids);

        void rebuild(Ice::Int);

        void addNewComer(Ice::Int);
        void addNewComerWithTime(Ice::Int,Ice::Int);
	void updateInfo(Ice::Int);
};

}
}
}
#endif

