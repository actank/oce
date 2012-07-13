#ifndef __FRIENDFINDER_INFOCACHE_REPLICA_ADAPTER_H__ 
#define __FRIENDFINDER_INFOCACHE_REPLICA_ADAPTER_H__ 

#include "FriendFinderInfoCache.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace xce {
namespace friendfinder {
namespace adapter {
using namespace MyUtil;
class FriendFinderInfoCacheReplicaAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::friendfinder::FriendFinderInfoCachePrx>, 
	public MyUtil::Singleton<FriendFinderInfoCacheReplicaAdapter> {
public:
	FriendFinderInfoCacheReplicaAdapter() : 
		MyUtil::ReplicatedClusterAdapterI <xce::friendfinder::FriendFinderInfoCachePrx>("ControllerFriendFinderInfoCache", 120, 300) {
	}

	NetworkInfo getNetworkInfo(int userId);
	NetworkInfo getNetworkInfoWithHostage(int userId);
	NetworkInfo getNetworkInfoWithHostageFromDB(int userId);

	HighSchoolInfo getHighSchoolInfo(int userId);

	MyUtil::IntSeq getWorkplaceData(int userId, const MyUtil::StrSeq& condition);
 	MyUtil::IntSeq getUnivData(int userId, const MyUtil::StrSeq& condition);
 	MyUtil::IntSeq getHighschoolData(int userId, const MyUtil::StrSeq& condition);
 	MyUtil::IntSeq getJuniorschoolData(int userId, const MyUtil::StrSeq& condition);
 	MyUtil::IntSeq getElementaryschoolData(int userId, const MyUtil::StrSeq& condition);
	int getBirthdayData(int userId);

	MyUtil::IntSeq getFriendListByInfo(int userId, int limit);
   	MyUtil::IntSeq getInfoMatchFriendList(int userId, int limit);
    MyUtil::IntSeq getFriendListByGuestInfo(int hostId,int guestId,int limit);
   	MyUtil::IntSeq getGuestInfoMatchFriendList(int hostId,int guestId,int limit);
   	MyUtil::IntSeq getGuestInfoMatchFriendListBatch(const MyUtil::IntSeq& hostIds,int guestId);
    MyUtil::IntSeq getCommonFriendListByGuestInfo(int userId,int minsharecount,int limit);

	void setWorkplaceDataWithMask(int cluster, const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq);
    void setUnivDataWithMask(int cluster, const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq);
   	void setHighschoolDataWithMask(int cluster, const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq);
   	void setJuniorschoolDataWithMask(int cluster, const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq);
    void setElementaryschoolDataWithMask(int cluster, const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq);

	void setBirthdayData(int cluster, int birthYear, const MyUtil::IntSeq& orderedIds);
};

}
}
}

#endif

