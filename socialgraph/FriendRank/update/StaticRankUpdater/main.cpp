#include <signal.h>
#include <unistd.h>
#include "Consumer.h"
#include "ModelFetcher.h"

void UpdateStaticRank_SigRoutine(int dunno);
void UpdateStaticRank_Initial();
void UpdateStaticRank_Cleanup();
int  LoadTestUser(set<USERID>& filter);
int  FillSlotTest(int slotpos, const set<USERID>& filter);
int  FillSlot(int slotpos, const time_t start_time, long& linenum);
bool IsInputFileOk();

const int      CONSUMER_NUM = 16;
Consumer*      worker[CONSUMER_NUM];
ThreadParam*   threadParam = NULL;

ActiveRankHead headActiveRank;
char*          dataActiveRank = NULL;
ModelFetcher*  fetcher = NULL;
FILE*          fpActiveRank = NULL;

time_t get_filemtime(char *filename) // {{{
{
	struct stat f_stat;
	if (stat(filename, &f_stat) == -1)
	{
		return -1;
	}
	return f_stat.st_mtime;
}// }}}

int copyfile(const char* oldfname, const char* newfname) // {{{
{
	if (oldfname==NULL || newfname==NULL)
		return -1;

	FILE* fpold = fopen(oldfname, "rb");
	if (fpold==NULL)
	{
		fprintf(stderr, "%s does not exist.\n", oldfname);
		return -1;
	}
	FILE* fpnew = fopen(newfname, "wb");
	if (fpold==NULL)
	{
		fprintf(stderr, "can not open file %s to write.\n", newfname);
		return -1;
	}

	size_t bsize = 1<<20;
	size_t nmemb = 0;
	char* buff = new char[bsize];
	while (true)
	{
		nmemb = fread(buff, 1, bsize, fpold);
		fwrite(buff, 1, nmemb, fpnew);

		if (nmemb<bsize)
			break;
	}
	delete[] buff;

	fclose(fpold);
	fclose(fpnew);
	return 0;
}// }}}

int UpdateStaticRank(void) // {{{
{
	if (!IsInputFileOk())
	{
		MCE_INFO("检查所需文件(active_rank.dat,user_models.dat_xx)是否完整");
		return -1;
	}

	time_t start = time(NULL);
	// DEBUG: 计算/data/xce/friendrank/data/userid_list中的用户
	//set<USERID> filter;
	//LoadTestUser(filter);
	// 得到昨天零点时间，做为本次计算的起始时间（增量计算）
	time_t start_time = GetYestodayZero(time(NULL));
	UpdateStaticRank_Initial();

	MCE_INFO("Start computing");
	const long PRINTSTEP = 100000;
	long linenum = 0;
	long linenum_last = linenum; // 使用相邻两次行数差，与PRINTSTEP相比，判断打印信息
	long validnum = 0; // 符合增量计算条件的用户数
	long waitTime = 0; // Producer等待时间，单位0.01秒
	while (true)
	{
		// get one free slot
		int slotpos = threadParam->GetFreeSlot();  // 0
		if (slotpos==-1)
		{
			++waitTime;
			usleep(10000);
			continue;
		}

		// product data
		//if (FillSlotTest(slotpos, filter)!=0) {
		if (FillSlot(slotpos, start_time, linenum)!=0)	{
			threadParam->FreeSlot(slotpos);
			break;
		} else {
			threadParam->FullSlot(slotpos);
		}

		++validnum;
		if (linenum-linenum_last>=PRINTSTEP)
		{
			linenum_last = linenum / PRINTSTEP * PRINTSTEP;
			MCE_INFO("\033[31m"<<validnum<<" / "<<linenum<<"\033[0m\tTimeUsed "<<time(NULL)-start
					<<"\tProducerWait "<<waitTime/100);
		}
	}

	UpdateStaticRank_Cleanup();
	time_t end = time(NULL);
	MCE_INFO("[UpdateStaticRank] Time used "<<end-start<<" second");
	return 0;
}//}}}

bool IsInputFileOk() // {{{
{
	char fname[128];

	//----------------------------
	// 检查是否有新的user_model.dat_xx
	bool newmodel = true;
	for (int i=0; i<WORKERNUM; ++i)
	{
		sprintf(fname, "/data/xce/friendrank/data/user_models.dat_%i", i);
		if (access(fname, R_OK)==0)
		{
			if (time(NULL)-get_filemtime(fname)<3600)
			{
				MCE_INFO("File " << fname << " 's the last modify time is not over 1 hour");
				newmodel = false;
				break;  //  存在，但还未生成完
			}
		}
		else
		{
			newmodel = false;
			break;  // 不存在
		}
	}
	if (newmodel)
	{
		remove("/data/xce/friendrank/data/friend_rank/user_models.idx");
		char dstfname[128];
		for (int i=0; i<WORKERNUM; ++i)
		{
			sprintf(fname, "/data/xce/friendrank/data/user_models.dat_%i", i);
			sprintf(dstfname, "/data/xce/friendrank/data/friend_rank/user_models.dat_%i", i);
			rename(fname, dstfname);
		}
	}

	//----------------------------
	// 检查是否有新的active_rank.idx
	sprintf(fname, "/data/xce/friendrank/data/active_rank.idx");
	if (access(fname, R_OK)==0)
	{
		MCE_INFO("[UpdateStaticRank] copy new file 'active_rank.idx' ... ");
		rename(fname, "/data/xce/friendrank/data/friend_rank/active_rank.idx");
		copyfile("/data/xce/friendrank/data/active_rank.dat",
				"/data/xce/friendrank/data/friend_rank/active_rank.dat");
		MCE_INFO("[UpdateStaticRank] copy new file 'active_rank.idx' ... OK");
	}

	//----------------------------
	// 检查所需文件是否齐全
	sprintf(fname, "/data/xce/friendrank/data/friend_rank/active_rank.dat");
	if (access(fname, R_OK)!=0)
		return false;
	for (int i=0; i<WORKERNUM; ++i)
	{
		sprintf(fname, "/data/xce/friendrank/data/friend_rank/user_models.dat_%i", i);
		if (access(fname, R_OK)!=0)
			return false;
	}
	return true;
}// }}}

int LoadTestUser(set<USERID>& filter) //{{{
{
	FILE* fp = fopen("/data/xce/friendrank/data/userid_list", "rt");
	assert(fp!=NULL);

	USERID uid = 0;
	char line[256];
	fgets(line, 256, fp);
	while (!feof(fp))
	{
		uid = atoi(line);
		if (uid>0)
			filter.insert(uid);
		fgets(line, 256, fp);
	}

	fclose(fp);
	return 0;
}// }}}

int FillSlotTest(int slotpos, const set<USERID>& filter) // {{{
{
	bool fileend = false;
	do	{
		if (fread(&headActiveRank, sizeof(ActiveRankHead), 1, fpActiveRank)!=1)
		{
			fileend = true;
			break;
		}
		assert(headActiveRank.num<=1000);
		if ((int)fread(dataActiveRank, sizeof(ActiveRank), headActiveRank.num, fpActiveRank)
				!=headActiveRank.num)
		{
			fileend = true;
			break;
		}
	} while (filter.find(headActiveRank.uid) == filter.end());

	if (fileend)
		return -1;

	char**& pUserModels = threadParam->dataUserModels[slotpos];
	fetcher->FetchModel(headActiveRank.uid, pUserModels/*, 10<<20*/);
	return 0;
}// }}}
int FillSlot(int slotpos, const time_t start_time, long& linenum) // {{{
{
	// 增量计算：当前周期内(ActiveRank的计算周期)有交互行为的用户
	bool fileend = false;
	while (true/*!fileend*/)
	{
		++linenum;
		// 读一条ActiveRank
		if (fread(&headActiveRank, sizeof(ActiveRankHead), 1, fpActiveRank)!=1)
		{
			fileend = true;
			break;
		}
		assert(headActiveRank.num<=1000);
		if ((int)fread(dataActiveRank, sizeof(ActiveRank), headActiveRank.num, fpActiveRank)
				!=headActiveRank.num)
		{
			fileend = true;
			break;
		}
		// 判断是否有效
		if ( (headActiveRank.date >= start_time)
				/*DEBUG: && (headActiveRank.uid % 10000 == 0)*/)
		{
			//cout << headActiveRank.uid << endl;
			char**& pUserModels = threadParam->dataUserModels[slotpos];
			int ret = fetcher->FetchModel(headActiveRank.uid, pUserModels);
			if (ret>=0)
				break;  // 读数据正常，退出；否则继续读下一条
		}
	}

	//------------------------------
	if (fileend) {
		return -1;   // 文件结尾，处理完毕
	} else {
		return 0;
	}
}// }}}

void UpdateStaticRank_SigRoutine(int sig) // {{{
{
	switch (sig)
	{
		case 1:
			printf("Get a signal -- SIGHUP \n");
			break;
		case 2:
			printf("Get a signal -- SIGINT \n");
			break;
		case 3:
			printf("Get a signal -- SIGQUIT \n");
			break;
	}

	UpdateStaticRank_Cleanup();
	exit(0);
}// }}}
void UpdateStaticRank_Initial() // {{{
{
	//PrintTime();
	//signal(SIGHUP, UpdateStaticRank_SigRoutine); // 下面设置三个信号的处理方法
	//signal(SIGQUIT, UpdateStaticRank_SigRoutine);
	//signal(SIGINT, UpdateStaticRank_SigRoutine);

	// 删除旧的静态分值文件
	char* rankdir = "/data/xce/friendrank/data/friend_rank/static_rank/";
	set<string> fnames;
	TravesalDir(rankdir, fnames);
	for (set<string>::const_iterator iter = fnames.begin();
			iter != fnames.end(); ++iter)
	{
		string fullname = rankdir + *iter;
		remove(fullname.c_str());
	}

	for (int i=0; i<CONSUMER_NUM; ++i)
	{
		worker[i] = NULL;
	}

	fpActiveRank = fopen("/data/xce/friendrank/data/friend_rank/active_rank.dat", "rb");
	assert(fpActiveRank!=NULL);
	
	const int ACTIVE_NUM = 1000;
	dataActiveRank = new char[ACTIVE_NUM*sizeof(ActiveRank)];

	fetcher = new ModelFetcher;
	threadParam = new ThreadParam;
	for (int i=0; i<CONSUMER_NUM; ++i)
	{
		worker[i] = new Consumer;
		worker[i]->Start(threadParam);
	}
}// }}}
void UpdateStaticRank_Cleanup() // {{{
{
	// 先结束线程，然后释放共享变量。
	MCE_INFO("[UpdateStaticRank] Cleanup()\n");

	// 使用标志位通知线程结束
	// 不能使用pthread_cancel机制，因为很多函>数不能确定是否取消点，容易造成锁阻塞
	threadParam->SetDone(true); sleep (5);
	MCE_INFO("[UpdateStaticRank] Cancel thread ...\n");
	for (int i=0; i<CONSUMER_NUM; ++i)
	{
		if (worker[i]!=NULL)
		{
			//worker[i]->CancelThread();
			worker[i]->WaitForEnd(); // TODO TODO 应该执行这句的
			delete worker[i];
			worker[i] = NULL;
		}
	}
	
	MCE_INFO("[UpdateStaticRank] Close file point ...\n");
	if (fpActiveRank!=NULL)
	{
		fclose(fpActiveRank);
		fpActiveRank = NULL;
	}
	MCE_INFO("[UpdateStaticRank] Release buffer ...\n");
	delete[] dataActiveRank;
	dataActiveRank = NULL;
	delete fetcher;
	fetcher = NULL;
	
	MCE_INFO("[UpdateStaticRank] Release threadParam ...\n");
	delete threadParam;
	threadParam = NULL;
	//PrintTime();
}// }}}

