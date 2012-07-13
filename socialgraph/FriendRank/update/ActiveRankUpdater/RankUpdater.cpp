#include "RankUpdater.h"

// UpdateRank {{{
int RankUpdater::UpdateRank(const char* fname)
{
	map<USERID,map<USERID,short> > pvInc;
	MCE_INFO("load inc active ...");
	LoadIncActive(fname, pvInc);
	MCE_INFO("load inc active ... OK");

	// merge inc to all rank
	MCE_INFO("batch update ...");
	MergeRank(pvInc);
	MCE_INFO("batch update ... OK");
	return 0;
}// }}}

// LoadIncActive {{{
int RankUpdater::LoadIncActive(const char* fname, map<USERID,map<USERID,short> >& pvInc)
{
	// open log file - fp;
	FILE* fp = fopen(fname, "rb");
	assert(fp!=NULL);

	map<USERID,map<USERID,short> >::iterator iter;
	USERID uid = 0;
	int num = 0;
	USERID fid = 0;
	short weight = 0;

	// read each line, and process each user's pv
	while (fread(&uid, sizeof(USERID), 1, fp)==1)
	{
		pair<map<USERID,map<USERID,short> >::iterator, bool> result =
			pvInc.insert(make_pair<USERID,map<USERID,short> >(uid, map<USERID,short>()));
		assert(result.second);
		iter = result.first;

		fread(&num, sizeof(int), 1, fp);
		for (int i=0; i<num; ++i)
		{
			fread(&fid, sizeof(USERID), 1, fp);
			fread(&weight, sizeof(short), 1, fp);

			iter->second.insert(make_pair<USERID,short>(fid, weight));
		}
	}

	fclose(fp);
	return 0;
}// }}}

// MergeRank {{{
int RankUpdater::MergeRank(map<USERID,map<USERID,short> >& pvInc)
{
	string datname = "/data/xce/friendrank/data/active_rank.dat";
	string tmpname = "/data/xce/friendrank/data/temp/active_rank.dat";
	if (access(datname.c_str(), 4)==-1)
	{
		FILE* fp = fopen(datname.c_str(), "wb");
		fclose(fp);
	}
	FILE* fpold = fopen(datname.c_str(), "rb");
	assert(fpold!=NULL);
	FILE* fpnew = fopen(tmpname.c_str(), "wb");
	assert(fpnew!=NULL);

	MergeOldRank(fpold, fpnew, pvInc);
	MergeNewRank(fpnew, pvInc);

	fclose(fpold);
	fclose(fpnew);
	
	// create new rank flag
	rename(tmpname.c_str(), datname.c_str());
	FILE* fpidx = fopen("/data/xce/friendrank/data/active_rank.idx", "wb");
	fclose(fpidx);
	return 0;
}// }}}

// MergeOldRank {{{
int RankUpdater::MergeOldRank(FILE* fpold, FILE* fpnew, map<USERID,map<USERID,short> >& pvInc)
{
	map<USERID,map<USERID,short> >::iterator iter;
	ActiveRankHead head;
	long step = 100000;
	long linenum = 0;

	// 逐个更新已有用户
	while (fread(&head, sizeof(ActiveRankHead), 1, fpold)==1)
	{
		fread(mActiveRankBuff, sizeof(ActiveRank), head.num, fpold);

		iter = pvInc.find(head.uid);
		if (iter!=pvInc.end())
		{
			UpdataRankBuff(head, iter->second);
			// delete this unit
			pvInc.erase(iter);
		}
		// write new file
		fwrite(&head, sizeof(ActiveRankHead), 1, fpnew);
		fwrite(mActiveRankBuff, sizeof(ActiveRank), head.num, fpnew);

		if (++linenum % step == 0)
			MCE_INFO("\tMergeOldRank(*"<<step<<") "<<linenum/step);
	}
	//fprintf(stderr, "\n");
	return 0;
}// }}}
// MergeNewRank {{{
int RankUpdater::MergeNewRank(FILE* fpnew, map<USERID,map<USERID,short> >& pvInc)
{
	map<USERID,map<USERID,short> >::iterator iter;
	ActiveRankHead head;
	long step = 10000;
	long linenum = 0;

	// 逐个附加新增用户
	for (map<USERID,map<USERID,short> >::const_iterator iter = pvInc.begin();
			iter != pvInc.end(); ++iter)
	{
		map<USERID,ActiveRank> pvNew;
		for (map<USERID,short>::const_iterator thisIter = iter->second.begin();
				thisIter != iter->second.end(); ++thisIter)
		{
			pair<map<USERID,ActiveRank>::iterator, bool> result =
				pvNew.insert(make_pair<USERID,ActiveRank>(
							iter->first, ActiveRank(thisIter->first)));
			result.first->second.IncreasePv(thisIter->second);
		}

		priority_queue<ActiveRank,vector<ActiveRank>,Comp_ByActiveRank> pqScore;
		int rmnum = iter->second.size() - BUFFLEN;
		int unitnum = 0;
		for (map<USERID,ActiveRank>::const_iterator pvIter = pvNew.begin();
				pvIter != pvNew.end(); ++pvIter)
		{
			pqScore.push(pvIter->second);
			if ((int)(pqScore.size())>rmnum)
			{
				mActiveRankBuff[unitnum++] = pqScore.top();
				pqScore.pop();
			}
		}

		head.uid = iter->first;
		head.date = time(NULL);
		head.num = unitnum;
		fwrite(&head, sizeof(ActiveRankHead), 1, fpnew);
		fwrite(mActiveRankBuff, sizeof(ActiveRank), head.num, fpnew);
		
		if (++linenum % step == 0)
			MCE_INFO("\tMergeNewRank(*"<<step<<") "<<linenum/step);
	}
	return 0;
}// }}}

// UpdataRankBuff {{{
int RankUpdater::UpdataRankBuff(ActiveRankHead& head, map<USERID,short>& thePvInc)
{
	// update
	map<USERID,ActiveRank> pvOld;
	for (int i=0; i<head.num; ++i)
	{
		pvOld.insert(make_pair<USERID,ActiveRank>(
					mActiveRankBuff[i].GetFid(), mActiveRankBuff[i]));
	}
	MergeRankByUserId(pvOld, thePvInc);

	int rmnum = pvOld.size() - BUFFLEN;
	int unitnum = 0;
	if (rmnum>0)
	{
		priority_queue<ActiveRank,vector<ActiveRank>,Comp_ByActiveRank> pqScore;
		for (map<USERID,ActiveRank>::const_iterator pvIter = pvOld.begin();
				pvIter != pvOld.end(); ++pvIter)
		{
			pqScore.push(pvIter->second);
			if ((int)(pqScore.size())>rmnum)
			{
				mActiveRankBuff[unitnum++] = pqScore.top();
				pqScore.pop();
			}
		}
	}
	else
	{
		for (map<USERID,ActiveRank>::const_iterator pvIter = pvOld.begin();
				pvIter != pvOld.end(); ++pvIter)
		{
			mActiveRankBuff[unitnum++] = pvIter->second;
		}
	}

	head.num = (short)unitnum;
	head.date = time(NULL);
	return 0;
}// }}}
// MergeRankByUserId {{{
// 此处是增量更新
int RankUpdater::MergeRankByUserId(map<USERID,ActiveRank>& pvOld,
	 	const map<USERID,short>& pvInc)
{
	// 合并增量PV（pvOld不存在好友PV，需要新增）
	for (map<USERID,short>::const_iterator iter = pvInc.begin();
			iter != pvInc.end(); ++iter)
	{
		//printf("%d(%d), ", iter->first, iter->second);
		USERID fid = iter->first;
		//if (friends.find(fid)==friends.end())
		//	continue;
		map<USERID,ActiveRank>::iterator oldIter = pvOld.find(fid);
		if (oldIter==pvOld.end())
		{
			pair<map<USERID,ActiveRank>::iterator, bool> result =
				pvOld.insert(make_pair<USERID,ActiveRank>(fid, ActiveRank(fid)));
			oldIter = result.first;
		}
		oldIter->second.IncreasePv(iter->second); 
	}

	// 如果当前单元累计PV超过500，就向后移动一个单元
	int curPvTotal = 0;
	for (map<USERID,ActiveRank>::iterator iter = pvOld.begin();
			iter != pvOld.end(); ++iter)
	{
		curPvTotal += iter->second.GetCurUnit();
	}
	if (curPvTotal>500)  // TODO: read '500' using iniparser
	{
		for (map<USERID,ActiveRank>::iterator iter = pvOld.begin();
				iter != pvOld.end(); ++iter)
		{
			iter->second.Attenuate();
		}
	}
	return 0;
}// }}}

