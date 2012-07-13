#ifndef  _FRIENDRANK_CREATOR_H_
#define  _FRIENDRANK_CREATOR_H_

#include "../common/Common.h"
#include "Ranker.h"

class FriendRankCreator
{
	public:
		FriendRankCreator();
		virtual ~FriendRankCreator();
		int CreateFriendRank(const char* fname);

	private:
		int LoadActiveRankIndex(FILE* fp);
		bool GetNextStaticRank(FILE* fp);
		void SaveFriendRank(int uid, vector<RankScore>& result);

	private:
		FILE* fpFriendRank;
		FILE* fpActiveRank;

		Ranker* ranker;
		hash_map<USERID,long> mActiveRankPos;  // pair<active_rank,static_rank>
		
		ActiveRankHead headActiveRank;
		ActiveRank* dataActiveRank;
		StaticRankHead headStaticRank;
		StaticRank* dataStaticRank;
};

#endif

