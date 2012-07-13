#ifndef _STATUSCACHE_ICE_
#define _STATUSCACHE_ICE_

#include <Util.ice>

module xce {
module statuscache {

	dictionary<int,bool> Int2BoolMap;
	interface StatusCacheManager {
		MyUtil::ByteSeq getStatusMinMax(int minId,int maxId);
		Int2BoolMap getStatusBatch(MyUtil::IntSeq ids);
		MyUtil::IntSeq getStatusIdsFrom(MyUtil::IntSeq ids);
		bool isStatusTrue(int userId);

		bool isGoodUser(int userid);
		void updateStatus(int userId,int status);
		void updateStatusBatch(MyUtil::Int2IntMap statusmap);
		bool isValid();
	};
};
};

#endif
