#include <signal.h>
#include <unistd.h>

#include "../common/Common.h"
#include "TaskManager.h"
#include "QueryRunner.h"
#include "util/cpp/IntSeqSerialize.h"
#include "FriendRankCacheAdapter.h" // FriendRankLoaderAdapter.h

using namespace com::xiaonei::xce;
using namespace MyUtil;

void SigRoutine(int dunno);
void Initial();
void Cleanup();
int WriteDatabase(FILE* fp);
int WriteDatabase(const map<int,vector<int> >& rankdatas);

int WriteFriendRank(void) // {{{
{
	MCE_INFO("[WriteFriendRank] String running ... ");
	time_t start = time(NULL);
	Initial();
	
	//ConnectionPoolManager::instance().initialize();
	TaskManager::instance().config(TASK_LEVEL_QUERY, ThreadPoolConfig(0, 50));

	FILE* fp = fopen("/data/xce/friendrank/data/friend_rank/friend_rank.dat", "rb");
	assert(fp!=NULL);
	WriteDatabase(fp);
	fclose(fp);

	long wait = 0;
	int task_size = 0;
	while ((task_size=TaskManager::instance().size(TASK_LEVEL_QUERY))>0)
	{
		wait+=10;
		sleep(10);
		//fprintf(stderr, "sleep %ld second. left %d tasks.\r", wait, task_size);
		MCE_INFO("Sleep "<<wait<<" second. left "<<task_size<<" tasks.\r");
	}
	MCE_INFO("bye-bye");

	Cleanup();
	time_t end = time(NULL);
	MCE_INFO("[WriteFriendRank] Time used "<<end-start<<" second");
	return 0;
}// }}}

void SigRoutine(int sig) // {{{
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

	Cleanup();
	exit(0);
}// }}}
void Initial() // {{{
{
	//PrintTime();
	//signal(SIGHUP, SigRoutine); //* 下面设置三个信号的处理方法
	//signal(SIGQUIT, SigRoutine);
	//signal(SIGINT, SigRoutine);
}// }}}
void Cleanup() // {{{
{
	MCE_INFO("Cleaning jobs: OK");
	//PrintTime();
}// }}}

int WriteDatabase(FILE* fp) // {{{
{
	int uid = 0;
	int fid = 0;
	int number = 0;
	int weight = 0;
	long linenum = 0;
	const long PRINTSTEP = 100000;
	const long WRITESTEP = 500;
	map<int,vector<int> > rankdatas;

	while (fread(&uid, sizeof(int), 1, fp)==1)
	{
		++linenum;
		if (linenum % PRINTSTEP == 0)
		{
			MCE_INFO("Writing (*"<<PRINTSTEP<<") : "<<linenum/PRINTSTEP<<"\r");
			//int task_size = 0;
			//while ((task_size=TaskManager::instance().size(TASK_LEVEL_QUERY))>100)
			//{
			//	MCE_INFO("Sleep 10s because of TaskSize="<<task_size<<" >100");
			//	sleep(10);
			//}
		}
		if (linenum % WRITESTEP == 0)
		{
			WriteDatabase(rankdatas);
			rankdatas.clear();
		}

		pair<map<int,vector<int> >::iterator, bool> result =
			rankdatas.insert(make_pair<int,vector<int> >(uid, vector<int>()));
		assert(result.second);
		map<int,vector<int> >::iterator iter = result.first;

		fread(&number, sizeof(int), 1, fp);
		for (int i=0; i<number; ++i)
		{
			fread(&fid, sizeof(int), 1, fp);
			fread(&weight, sizeof(int), 1, fp);
			iter->second.push_back(fid);
			iter->second.push_back(weight);
		}
	}

	WriteDatabase(rankdatas);
	rankdatas.clear();
	
	//fprintf(stderr, "\nTotal write %ld users.\n", linenum);
	MCE_INFO("[WriteFriendRank] Total write "<<linenum<<" users.");
	return 0;
}// }}}

int WriteDatabase(const map<int,vector<int> >& rankdatas) // {{{
{
	if (rankdatas.empty())
		return -1;
	//struct timeval start[3];
	//gettimeofday(&start[0], NULL);
	//long sqllen = 0;
	
	//for (map<int,vector<int> >::const_iterator iter = rankdatas.begin();
	//		iter != rankdatas.end(); ++iter)
	//{
	//	cout << iter->first << " : ";
	//	for (vector<int>::const_iterator rankIter = iter->second.begin();
	//			rankIter != iter->second.end(); ++rankIter)
	//	{
	//		cout << *rankIter << ", ";
	//	}
	//	cout << endl;
	//}
	//sleep(1);

	//------------------------------------
	//vector<int> uids;
	//char idtemp[20];
	//string idlist = "";
	char log[10];
	sprintf(log, "+%ld", rankdatas.size());

	string rankdata;
	Statement sql;
	sql << "INSERT INTO friend_rank (user_id, rank) VALUES ";
	//sqllen += strlen("REPLACE INTO friend_rank (user_id, rank) VALUES ");
	for (map<int,vector<int> >::const_iterator iter = rankdatas.begin();
			iter != rankdatas.end(); ++iter)
	{
		//uids.push_back(uids);
		//sprintf(idtemp, "%d,", iter->first);
		//idlist += idtemp;
		if (iter != rankdatas.begin()) {
			sql << ",";
			//sqllen += 1;
		}
		rankdata.assign(reinterpret_cast<const char*>(&(iter->second[0])), 
				iter->second.size()*sizeof(int));
		sql << "(" << iter->first <<", " <<	mysqlpp::quote << rankdata << ")";

		//sql << "(" << iter->first <<", " <<
		//	mysqlpp::quote << MyUtil::IntSeqSerialize::serialize(iter->second) << ")";
		//sqllen += 4 + iter->second.size()*sizeof(int);
	}
	sql << " ON DUPLICATE KEY UPDATE user_id=values(user_id),rank=values(rank)";

	try{
		//cout << sqllen << endl;
		//getchar();
		QueryRunner("friend_rank", CDbWServer).schedule(sql,log); // schedule(sql); execute(sql);
	}catch( mysqlpp::Exception e){
		cout << " mysqlpp-> " << e.what() << endl;
	}catch( Ice::Exception e){
		cout << " Ice-> " << e.what() << endl;
	}catch( std::exception e){
		cout << " std-> " << e.what() << endl;
	}catch( ... ){
		cout << " unknown " << endl;
	}

	//------------------------------------
	//gettimeofday(&start[1], NULL);
	// 方案1：调用FriendRankLoaderAdapter::instance().reload(uid); 最慢
	// 方案2：调用FriendRankLoaderAdapter::instance().reload(uids); 较快
	// 方案3：调用FriendRankCacheAdapter::instance().Load(rankdatas); 更快
	xce::socialgraph::FriendRankCacheAdapter::instance().Load(rankdatas);
	
	//gettimeofday(&start[2], NULL);
	//------------------------------------
	//for (int i=0; i<2; ++i)
	//{
	//  fprintf(stdout, "[%d:%ld]", i,
	//      (start[i+1].tv_sec-start[i].tv_sec)*1000000+start[i+1].tv_usec-start[i].tv_usec);
	//}
	//fprintf(stderr, "\n");  getchar();
	return 0;
}// }}}

