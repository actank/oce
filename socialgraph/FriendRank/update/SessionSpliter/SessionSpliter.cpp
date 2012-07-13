#include "SessionSpliter.h"

int GetSessionIndex(const time_t ltime)
{
	struct tm *myTm = localtime(&ltime);
	return (myTm->tm_hour * 4) + (myTm->tm_min / 15);
}

// SplitFile {{{
// 将保存一天日志的文件，按照Session长度分割为n个文件，存储在../data/log/temp目录下
int SessionSpliter::SplitFile(const string& dir, const string& fname)
{
	// open log file - fp;
	string inname = dir + "/" + fname;
	FILE* fp = fopen(inname.c_str(), "rt");
	assert(fp!=NULL);
	
	FILE* fpout[SESSION_NUM];
	char outname[256];
 	for (int i=0; i<SESSION_NUM; ++i)
	{
		sprintf(outname, "%s/temp/%s_%02d", dir.c_str(), fname.c_str(), i);
		fpout[i] = fopen(outname, "wb");
	}
	//----------------------------

	Weblog weblog;
	Session ss;
	long linenum = 0;
	// read each line, and process each user's pv
	while (weblog.ParseNextLog(fp)==0)
	{
		if (weblog.weight>0 && weblog.sender>0 && (weblog.accepter1>0 || weblog.accepter2>0))
		{
			ss.Init(weblog.date, weblog.weight, weblog.sender, 
					weblog.accepter1, weblog.accepter2);
			int si = GetSessionIndex(weblog.date);
			fwrite(&ss, sizeof(Session), 1, fpout[si]);
		}
		else
		{
			//fprintf(stderr, "Useless log\n");
			//weblog.Print();
			//getchar();
		}
		
		++linenum;
		if (linenum % 1000000 == 0)
			MCE_INFO("SessionSpliter::SplitFile: "<<linenum<<"\r");
	}

	//----------------------------
	fclose(fp);
	for (int i=0; i<SESSION_NUM; ++i)
	{
		fclose(fpout[i]);
	}
	return 0;
}// }}}

// MergeFile {{{
int SessionSpliter::MergeFile(const string& dir, const string& fname)
{
	char name[256];
	// merge each session file
	map<USERID,map<USERID,short> > pvDay;
	printf("\n");
 	for (int i=0; i<SESSION_NUM; ++i)
	{
		MCE_INFO("SessionSpliter::MergeFile : "<<i<<" / "<<SESSION_NUM<<"\r");
		// open log src file
		sprintf(name, "%s/temp/%s_%02d", dir.c_str(), fname.c_str(), i);
		FILE* fpin = fopen(name, "rb");
		MergeEachFile(fpin, pvDay);
		fclose(fpin);
		remove(name);
	}
	printf("\n");
	
	// write to dest file
	sprintf(name, "%s/data/%s", dir.c_str(), fname.c_str());
	FILE* fpout = fopen(name, "wt");
	assert(fpout!=NULL);

	USERID uid = 0;
	int num = 0;
	USERID fid = 0;
	short weight = 0;
	for (map<USERID,map<USERID,short> >::const_iterator iter = pvDay.begin();
			iter != pvDay.end(); ++iter)
	{
		uid = iter->first;
		fwrite(&uid, sizeof(USERID), 1, fpout);
		num = iter->second.size();
		fwrite(&num, sizeof(int), 1, fpout);
		for (map<USERID,short>::const_iterator friIter = iter->second.begin();
				friIter != iter->second.end(); ++friIter)
		{
			fid = friIter->first;
			weight = friIter->second;
			fwrite(&fid, sizeof(USERID), 1, fpout);
			fwrite(&weight, sizeof(short), 1, fpout);
		}
	}

	fclose(fpout);
	return 0;
}// }}}

// MergeEachFile {{{
int SessionSpliter::MergeEachFile(FILE* fp, map<USERID,map<USERID,short> >& pvDay)
{
	map<USERID,map<USERID,short> > pvSession;
	Session ss;
	// read each line, and process each user's pv
	while (fread(&ss, sizeof(Session), 1, fp)==1)
	{
		UpdateSession(pvSession, ss);
	}

	// merge this session to pvDay
	for (map<USERID,map<USERID,short> >::const_iterator iter = pvSession.begin();
			iter != pvSession.end(); ++iter)
	{
		map<USERID,map<USERID,short> >::iterator dayIter = pvDay.find(iter->first);
		if (dayIter == pvDay.end())
		{
			pvDay.insert(*iter);
		}
		else
		{
			for (map<USERID,short>::const_iterator accIter = iter->second.begin();
					accIter != iter->second.end(); ++accIter)
			{
				map<USERID,short>::iterator friIter = dayIter->second.find(accIter->first);
				if (friIter == dayIter->second.end())
					dayIter->second.insert(*accIter);
				else
					friIter->second += accIter->second;
			}
		}
	}
	return 0;
}// }}}

// UpdateSession {{{
int SessionSpliter::UpdateSession(map<USERID,map<USERID,short> >& pvSession, const Session& ss)
{
	map<USERID,map<USERID,short> >::iterator iter;
	map<USERID,short>::iterator fIter;

	// map the action of (a,b) and (a,c)
	// insert a
	iter = pvSession.find(ss.sender);
	if (iter == pvSession.end())
	{
		pair<map<USERID,map<USERID,short> >::iterator, bool> result =
			pvSession.insert(make_pair<USERID,map<USERID,short> >(ss.sender, map<USERID,short>()));
		iter = result.first;
	}
	map<USERID,short>& accepters = iter->second;
	// (a,b), keep the max one
	if (ss.accepter1>0)
	{
		fIter = accepters.find(ss.accepter1);
		if (fIter == accepters.end())
			accepters.insert(pair<USERID,short>(ss.accepter1,ss.weight));
		else if (fIter->second < ss.weight)
			fIter->second = ss.weight;
	}
	// (a,c), keep the max one
	if (ss.accepter2>0)
	{
		fIter = accepters.find(ss.weight);
		if (fIter == accepters.end())
			accepters.insert(pair<USERID,short>(ss.accepter2,ss.weight));
		else if (fIter->second < ss.weight)
			fIter->second = ss.weight;
	}
	return 0;
}// }}}

