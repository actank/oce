#ifndef _ONLINESTATEFILTER_ICE_
#define _ONLINESTATEFILTER_ICE_

#include <Util.ice>

module xce {
module onlinestatefilter {

        /* from OnlineEventMediator
	const int OFFLINE = 0;
        const int ONLINEWEBPAGGER = 1;
        const int ONLINEXNT = 2;
        const int ONLINEOTHER=3;
	*/

	dictionary<int,bool> Int2BoolMap;
	interface OnlineStateFilterManager {
		MyUtil::ByteSeq getOnlineStateMinMax(int minId,int maxId);
		Int2BoolMap getOnlineStateBatch(MyUtil::IntSeq ids);
		MyUtil::IntSeq getOnlineIdsFrom(MyUtil::IntSeq ids);
		bool isOnlineStateTrue(int userId);
		void updateOnlineState(int userId,int onlinestate);
		void updateOnlineStateBatch(MyUtil::Int2IntMap onlinestatemap);
	};
};
};

#endif
