#ifndef STATUSCACHE_H
#define STATUSCACHE_H

#include <StatusCache.h>
#include <AdapterI.h>
#include <bitset>

namespace xce {
namespace statuscache {

const int STATUS_CACHE_SIZE = 2147483647;

class StatusCacheAdapter :
	public MyUtil::ReplicatedClusterAdapterI<StatusCacheManagerPrx>,
		public MyUtil::Singleton<StatusCacheAdapter> {
public:
	StatusCacheAdapter() : MyUtil::ReplicatedClusterAdapterI <StatusCacheManagerPrx>("ControllerStatusCache", 120, 3000) {}
	MyUtil::ByteSeq getStatusMinMax(int minId,int maxId);
        xce::statuscache::Int2BoolMap getStatusBatch(const MyUtil::IntSeq& ids);
        MyUtil::IntSeq getStatusIdsFrom(const MyUtil::IntSeq& ids);
        void getAllStatus(std::bitset<STATUS_CACHE_SIZE>& s);
        bool isStatusTrue(int userId);
	bool isGoodUser(int userid);
        void updateStatus(int userId,int status);
        void updateStatusBatch(const MyUtil::Int2IntMap& data);
        bool isValid();
};

}
}

#endif
