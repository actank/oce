#ifndef __FRIENDFINDER_BYMSN_REPLICA_ADAPTER_H__
#define __FRIENDFINDER_BYMSN_REPLICA_ADAPTER_H__

#include <boost/lexical_cast.hpp>
#include <FriendFinderByMSN.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>

namespace xce {
namespace friendfinder {
namespace adapter {

class FriendFinderByMSNAdapter : public MyUtil::ReplicatedClusterAdapterI<FriendFinderByMSNManagerPrx>, public MyUtil::Singleton<FriendFinderByMSNAdapter> {
public:
	FriendFinderByMSNAdapter() :
		MyUtil::ReplicatedClusterAdapterI <FriendFinderByMSNManagerPrx>("ControllerFriendFinderByMSN", 120, 300) {
	}

	MSNdata getMSNdata(Ice::Int id);

	MSNmap getPUAmap(Ice::Int id);
	MyUtil::IntSeq getPUAids(Ice::Int id);
	MyUtil::IntSeq getPUAidsWithMask(Ice::Int id,const MyUtil::IntSeq& mask);

	MSNmap getPUKmap(Ice::Int id);
	MyUtil::IntSeq getPUKids(Ice::Int id);
	MyUtil::IntSeq getPUKidsWithMask(Ice::Int id,const MyUtil::IntSeq& mask);

	void add(Ice::Int hostId,Ice::Int guestId,Ice::Int type);

	void remove(Ice::Int hostId,Ice::Int guestId);
	void removePUA(Ice::Int hostId,Ice::Int guestId);
	void removePUK(Ice::Int hostId,Ice::Int guestId);

	void reload(Ice::Int id);
};

};
};
};

#endif
