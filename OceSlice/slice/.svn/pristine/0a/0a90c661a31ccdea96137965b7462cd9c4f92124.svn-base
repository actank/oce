#ifndef _FRIEND_RANK_ICE_
#define _FRIEND_RANK_ICE_

#include <ObjectCache.ice>

module xce {
	module socialgraph {
		// dictionary<friendId,weight>, weight:[1,100]
		struct RankData
		{
			int userid;
			int weight;
		};
		sequence<RankData> RankSeq;
		dictionary<int,int> RankMap;
		dictionary<int,MyUtil::IntSeq> RankSeqMap;

		class FriendRankData
		{
			RankSeq data;
		};

		interface FriendRankCache
		{
			RankSeq GetRank(int userId);
			RankMap GetRevRank(int hostId, MyUtil::IntSeq guestIds);
			void SetRank(int userId, FriendRankData data);  // for loader invoke
			void SetRanks(MyUtil::ObjectResult datas);  // for loader invoke
			// 带着数据直接加载到Cache中
			void Load(RankSeqMap datas);  // for db writer invoke
			bool isValid();
			void setValid(bool valid);
			void setData(MyUtil::ObjectResult datas);
		};

		interface FriendRankLoader
		{
			// 从DB中加载指定ID到Cache
			void reload(int userId);  // for cache invoke
			//void reload(sequence<int> userIds);  // for cache invoke
		};
	};
};

#endif

