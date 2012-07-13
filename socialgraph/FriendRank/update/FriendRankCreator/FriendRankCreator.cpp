#include "FriendRankCreator.h"

// Constructor & Destructor {{{
FriendRankCreator::FriendRankCreator()
{
	fpActiveRank = fopen("/data/xce/friendrank/data/friend_rank/active_rank.dat", "rb");
	if (fpActiveRank==NULL)
	{
		MCE_ERROR("Check file 'active_rank.dat' exist!");
		exit(-1);
	}

	dataActiveRank = new ActiveRank[1000];
	dataStaticRank = new StaticRank[2000];

	LoadActiveRankIndex(fpActiveRank);
	fpFriendRank = fopen("/data/xce/friendrank/data/friend_rank/friend_rank.dat", "wb");
	
	ranker = new Ranker;
}
FriendRankCreator::~FriendRankCreator()
{
	delete[] dataActiveRank;
	delete[] dataStaticRank;
	delete ranker;
	fclose(fpActiveRank);
	fclose(fpFriendRank);
}// }}}

// LoadActiveRankIndex {{{
int FriendRankCreator::LoadActiveRankIndex(FILE* fp)
{
	long linenum = 0;
	long start = 0;
	time_t start_time = GetYestodayZero(time(NULL));
	hash_map<USERID,long>::iterator iter;

	MCE_INFO("Load active_rank index (*1000000): ");
	while (true)
	{
		start = ftell(fp);
		if (fread(&headActiveRank, sizeof(ActiveRankHead), 1, fp)!=1)
			break;
		if (fread(dataActiveRank, sizeof(ActiveRank), headActiveRank.num, fp)
				!=(size_t)headActiveRank.num)
			break;
		//printf("\n---------------- %d ----------------\n", headActiveRank.uid);
		//for (int i=0; i<headActiveRank.num; ++i)
		//{
		//  dataActiveRank[i].Print(1);
		//}
		//getchar();

		if (++linenum % 1000000 == 0)
			MCE_INFO("\t"<<linenum/1000000);

		// 只更新最近一天有ActiveLog的用户
		if (headActiveRank.date >= start_time/*true*/)
		{
			iter = mActiveRankPos.find(headActiveRank.uid);
			if (iter != mActiveRankPos.end())
				iter->second = start;
			else
				mActiveRankPos.insert(make_pair<USERID,long>(headActiveRank.uid,start));
		}
	}
	MCE_INFO("Load active_rank index ... OK");
	return 0;
}// }}}

// CreateFriendRank
int FriendRankCreator::CreateFriendRank(const char* fname)
{
	FILE* fpStaticRank = fopen(fname, "rb");
	assert(fpStaticRank!=NULL);

	const long PRINTSTEP = 10000;
	vector<RankScore> vecRank;
	long linenum = 0;
	hash_map<USERID,long>::const_iterator iter;

	//---------------------------------
	MCE_INFO(fname);
	MCE_INFO("CreateFriendRank [*"<<PRINTSTEP<<"]: ");
	while (GetNextStaticRank(fpStaticRank))
	{
		iter = mActiveRankPos.find(headStaticRank.uid);
		if (iter == mActiveRankPos.end())
			continue;   // 最近1天没有产生ActiveLog

		// 读取交互分值
		fseek(fpActiveRank, iter->second, SEEK_SET);
		if (fread(&headActiveRank, sizeof(ActiveRankHead), 1, fpActiveRank)!=1)
			return -1;
		if (fread(dataActiveRank, sizeof(ActiveRank), headActiveRank.num, fpActiveRank)
				!=(size_t)headActiveRank.num)
			return -1;

		// 合并资料分值与交互分值
		ranker->Rank(headStaticRank, dataStaticRank, headActiveRank, dataActiveRank, vecRank);
		
		// 保存综合分值
		SaveFriendRank(headStaticRank.uid, vecRank);
		vecRank.clear();
		if (++linenum % PRINTSTEP == 0)
			MCE_INFO("\t"<<linenum/PRINTSTEP);
	}

	//fprintf(stderr, "\n");
	fclose(fpStaticRank);
	return 0;
}

// GetNextStaticRank {{{
bool FriendRankCreator::GetNextStaticRank(FILE* fp)
{
	bool valid = true;
	while (true)
	{
		if (fread(&headStaticRank, sizeof(StaticRankHead), 1, fp)!=1)
		{
			valid = false;
			break;
		}
		if (fread(dataStaticRank, sizeof(StaticRank), headStaticRank.num, fp)
				!=(size_t)(headStaticRank.num))
		{
			valid = false;
			break;
		}
		//printf("\n---------------- %d ----------------\n", headStaticRank.uid);
		//for (int i=0; i<headStaticRank.num; ++i)
		//{
		//	dataStaticRank[i].Print();
		//	printf("\n");
		//}
		//getchar();
		break;
	}
	return valid;
}// }}}

// SaveFriendRank {{{
void FriendRankCreator::SaveFriendRank(int uid, vector<RankScore>& vecRank)
{
	//int rank_data[200];
	//int rank_data_len = 200 * sizeof(int);
	// get top 100
	if (vecRank.size()>100)
		vecRank.erase(vecRank.begin()+100, vecRank.end());
	sort(vecRank.begin(), vecRank.end(), RankScoreComp_ByFid());

	float maxScore = 0;
	float minScore = 10000;
	for (vector<RankScore>::const_iterator iter = vecRank.begin();
			iter != vecRank.end(); ++iter)
	{
		if (maxScore < iter->score)
			maxScore = iter->score;
		if (minScore > iter->score)
			minScore = iter->score;
	}

	int userid = 0;
	int weight = 0;
	int number = 0;

	//cout << ">>>>>>>>> " << uid << " <<<<<<<<<<" << endl;
	fwrite(&uid, sizeof(int), 1, fpFriendRank);
	number = vecRank.size();
	fwrite(&number, sizeof(int), 1, fpFriendRank);
	for (vector<RankScore>::const_iterator iter = vecRank.begin();
			iter != vecRank.end(); ++iter)
	{
		userid = iter->fid;
		weight = 10 + (int)(90*(0.001+iter->score-minScore)/(0.001+maxScore-minScore));
		fwrite(&userid, sizeof(int), 1, fpFriendRank);
		fwrite(&weight, sizeof(int), 1, fpFriendRank);
		//printf("<%d,%d>", userid, weight);
	}
	//printf("\n");	getchar();
}// }}}

