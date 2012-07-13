#include "Ranker.h"

// Constructor & Destructor {{{
Ranker::Ranker()
{
}

Ranker::~Ranker()
{
}// }}}

// 返回按照Rank从高到底排序的好友列表
int Ranker::Rank(const StaticRankHead& headStaticRank, const StaticRank* dataStaticRank,
		const ActiveRankHead& headActiveRank, const ActiveRank* dataActiveRank,
		vector<RankScore>& vecRank)
{
	// Get rank from BDB, and sort it.
	map<USERID,const ActiveRank*> mapActiveRank;
	for (int i=0; i<headActiveRank.num; ++i)
	{
		mapActiveRank.insert(make_pair<USERID,const ActiveRank*>(
					dataActiveRank[i].GetFid(), dataActiveRank+i));
	}

	// 合并两个Rank，去掉非好友的PV
	map<USERID,RankScore> mapRank;
	const ActiveRank emptyActiveRank;
	USERID fid = 0;
	RankScore rank(fid);

	for (int i=0; i<headStaticRank.num; ++i)
	{
		fid = dataStaticRank[i].fid;
		rank.Init(fid);
		map<USERID,const ActiveRank*>::const_iterator aIter = mapActiveRank.find(fid);
		if (aIter!=mapActiveRank.end())
			MergeRank(rank, dataStaticRank[i], *(aIter->second));
		else
			MergeRank(rank, dataStaticRank[i], emptyActiveRank);
		mapRank.insert(make_pair<USERID,RankScore>(fid, rank));
	}

	//-------------------------------
	SortRank(mapRank, vecRank);
	//PrintRank(headStaticRank.uid, vecRank, "friend by active and static");
	return 0;
}

// MergeRank {{{
int Ranker::MergeRank(RankScore& rank, 
		const StaticRank& staticRank, const ActiveRank& activeRank)
{
	rank.scoreStatic.first = staticRank.GetScore(rank.scoreStatic.second);
	rank.scoreStatic.first = rank.scoreStatic.first;
	rank.scoreActive.first = activeRank.GetScore(rank.scoreActive.second);
	rank.scoreActive.first = 2*sqrt(rank.scoreActive.first+3);
	return 0;
}// }}}

// SortRank {{{
int Ranker::SortRank(map<USERID,RankScore>& rank, vector<RankScore>& vecRank)
{
	for (map<USERID,RankScore>::iterator iter = rank.begin();
			iter != rank.end(); ++iter)
	{
		RankScore rs = iter->second;
		rs.score = /*0.8**/rs.scoreActive.first + /*0.2**/rs.scoreStatic.first;
		vecRank.push_back(rs);
	}
	//for (vector<RankScore>::const_iterator iter = vecRank.begin();
	//		iter != vecRank.end(); ++iter)
	//{
	//	iter->Print();
	//}
	//getchar();
	sort(vecRank.begin(), vecRank.end(), RankScoreComp_ByScore());
	return 0;
}// }}}

// PrintRank {{{
int Ranker::PrintRank(USERID uid, const vector<RankScore>& vecRank, const char* title)
{
	//printf("\033[31m%s\n\033[0m", title);
	//printf("\033[31m%d, %ld friends\n\033[0m", uid, vecRank.size());

	//BasicFetcher* objBasic = new BasicFetcher("../conf/database.ini");
	//string userinfo[5];

	//int num = 0;
	//for (vector<RankScore>::const_iterator iter = vecRank.begin();
	//		iter != vecRank.end(); ++iter)
	//{
	//	objBasic->Fetch(iter->fid, userinfo);
	//	printf("#%d#[%s(%.2f)]\t#%.2f# %s\t#%.2f# %s\n", 
	//			++num, userinfo[1].c_str(), iter->score, 
	//			iter->scoreActive.first, iter->scoreActive.second.c_str(),
	//		 	iter->scoreStatic.first, iter->scoreStatic.second.c_str());
	//}
	//printf("\n\n");
	//delete objBasic;
	//getchar();
	return 0;
}// }}}

