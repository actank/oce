#ifndef __FRIENDFINDER_BYCOMMON_REPLICA_ADAPTER_H__
#define __FRIENDFINDER_BYCOMMON_REPLICA_ADAPTER_H__

#include <boost/lexical_cast.hpp>
#include <FriendFinderByCommon.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>

namespace xce {
namespace friendfinder {
namespace adapter {

class FriendFinderByCommonAdapter : public MyUtil::ReplicatedClusterAdapterI<FriendFinderByCommonManagerPrx>, public MyUtil::Singleton<FriendFinderByCommonAdapter> {
public:
	FriendFinderByCommonAdapter() :
		MyUtil::ReplicatedClusterAdapterI <FriendFinderByCommonManagerPrx>("ControllerFriendFinderByCommon", 120, 300) {
	}

	void setCommonFriendSeq(Ice::Int userId,const com::xiaonei::service::CommonFriendSeq& cfs);
	CommonFriendItemSeq getCommonFriendItemSeq(Ice::Int userId, Ice::Int limit);
	CommonFriendItemSeq getCommonFriendItemSeqById(Ice::Int userId, Ice::Int limit);
	Ice::Int getCount(Ice::Int userId);
	void removeCommonFriendItem(Ice::Int userId, Ice::Int friendId);
	void reload(Ice::Int userId);

	MyUtil::IntSeq getFriendsByCommon(Ice::Int guestId,Ice::Int hostId,Ice::Int limit);
};

};
};
};

#endif
