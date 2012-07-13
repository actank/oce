#ifndef _RANK_H_
#define _RANK_H_

#include "../common/Common.h"
#include "../common/ActiveRank.h"
#include "../common/StaticRank.h"

class RankScore
{
	public:
		RankScore(USERID fid=0)
		{
			Init(fid);
		}
		~RankScore() {}
		void Init(USERID fid)
		{
			this->fid = fid;score = 0;
			scoreStatic = make_pair<float,string>(0,"");
			scoreActive = make_pair<float,string>(0,"");
		}
		void Print() const
		{
			printf("%d : %.2f\n", fid, score);
			printf("\t%.2f: %s\n", scoreStatic.first, scoreStatic.second.c_str());
			printf("\t%.2f: %s\n", scoreActive.first, scoreActive.second.c_str());
		}
		RankScore& operator=(const RankScore& other)
		{ 
			if(this == &other)
				return *this;

			this->fid = other.fid;
			this->score = other.score;
			this->scoreStatic = other.scoreStatic;
			this->scoreActive = other.scoreActive;

			return *this;
		} 

	public:
		USERID fid;
		float score;
		pair<float,string> scoreStatic;
		pair<float,string> scoreActive;
};

struct RankScoreComp_ByScore
{
	bool operator()(const RankScore& p1, const RankScore& p2) {
		return p1.score > p2.score;
	}
};
struct RankScoreComp_ByFid
{
	bool operator()(const RankScore& p1, const RankScore& p2) {
		return p1.fid < p2.fid;
	}
};

// not thread-safe
class Ranker
{
	public:
		Ranker();
		~Ranker();
		int Rank(const StaticRankHead& headStaticRank, const StaticRank* dataStaticRank,
				const ActiveRankHead& headActiveRank, const ActiveRank* dataActiveRank,
				vector<RankScore>& vecRank);

	private:
		int MergeRank(RankScore& rank, 
				const StaticRank& staticRank, const ActiveRank& activeRank);
		int SortRank(map<USERID,RankScore>& rank, vector<RankScore>& vecRank);
		int PrintRank(USERID uid, const vector<RankScore>& vecRank, const char* title);
};

#endif

