#ifndef _RANK_UPDATER_H_
#define _RANK_UPDATER_H_

#include "../common/ActiveRank.h"

// not thread-safe
class RankUpdater
{
	public:
		RankUpdater() : BUFFLEN(1000)
		{
			mActiveRankBuff = new ActiveRank[BUFFLEN];

			// TODO: remove this test code
			//FILE* fp = fopen("../data/userid_list", "rt");
			//assert(fp!=NULL);
			//char line[256];
			//fgets(line, 256, fp);
			//USERID uid = 0;
			//while (!feof(fp))
			//{
			//	uid = atoi(line);
			//	if (uid>0)
			//		mTestUids.insert(uid);
			//	fgets(line, 256, fp);
			//}
			//fclose(fp);
		}
		virtual ~RankUpdater()
	 	{
			delete[] mActiveRankBuff;

			//FILE* fp = fopen("../data/userid_incr", "wt");
			//for (hash_set<int>::const_iterator iter = mIncUids.begin();
			//		iter != mIncUids.end(); ++iter)
			//{
			//	fprintf(fp, "%d\n", *iter);
			//}
			//fclose(fp);
		}
		int UpdateRank(const char* fname);

	private:
		int LoadIncActive(const char* fname, map<USERID,map<USERID,short> >& pvInc);

		int MergeRank(map<USERID,map<USERID,short> >& pvInc);
		int MergeOldRank(FILE* fpold, FILE* fpnew, map<USERID,map<USERID,short> >& pvInc);
		int MergeNewRank(FILE* fpnew, map<USERID,map<USERID,short> >& pvInc);
		int MergeRankByUserId(map<USERID,ActiveRank>& pvOld,
				const map<USERID,short>& pvInc);
		int UpdataRankBuff(ActiveRankHead& head, map<USERID,short>& thePvInc);

	private:
		ActiveRank* mActiveRankBuff;
		const size_t BUFFLEN;
		//hash_set<int> mIncUids;  // 需要更新资料分值的uid list（新好友、交互较多的用户）
		set<USERID> mTestUids;
};

#endif

