//#include <signal.h>
//#include <unistd.h>
//#include "Common.h"
//#include "RankUpdater.h"
//
//void SigRoutine(int dunno);
//void Initial();
//void Cleanup();
//
//RankUpdater* ranker = NULL;
//
//int main(int argc, char* argv[])
//{
//	time_t start = time(NULL);
//	Initial();
//
//	const char* dir = "../data/log/data/";
//	set<string> fnames;
//	TravesalDir(dir, fnames);
//	for (set<string>::const_iterator iter = fnames.begin();
//			iter != fnames.end(); ++iter)
//	{
//		if (strncmp(iter->c_str(), "action.", 7)!=0)
//			continue;
//		string fullname = dir+*iter;
//		printf("%s\n", fullname.c_str());
//		ranker->UpdateRank(fullname.c_str());
//		//:TODO: remove the log file
//		printf("\t>>> remove %s.\n", fullname.c_str());
//		//remove(fullname.c_str());
//		string destname = "../data/log/backup/" + *iter;
//		rename(fullname.c_str(), destname.c_str());
//	}
//
//	Cleanup();
//	time_t end = time(NULL);
//	printf("time used %ld second\n", end-start);
//	return 0;
//}
//
//// SigRoutine {{{
//void SigRoutine(int sig)
//{
//	switch (sig)
//	{
//		case 1:
//			printf("Get a signal -- SIGHUP \n");
//			break;
//		case 2:
//			printf("Get a signal -- SIGINT \n");
//			break;
//		case 3:
//			printf("Get a signal -- SIGQUIT \n");
//			break;
//	}
//
//	Cleanup();
//	exit(0);
//}// }}}
//// Initial & Cleanup {{{
//void Initial()
//{
//	PrintTime();
//	signal(SIGHUP, SigRoutine); //* 下面设置三个信号的处理方法
//	signal(SIGQUIT, SigRoutine);
//	signal(SIGINT, SigRoutine);
//
//	ranker = new RankUpdater;
//}
//void Cleanup()
//{
//	printf("\nCleaning jobs:\n");
//	delete ranker;
//
//	PrintTime();
//}// }}}
//

#include "../common/Common.h"
#include "RankUpdater.h"

int UpdateActiveRank(void)
{
	time_t start = time(NULL);
	RankUpdater* ranker = new RankUpdater;

	//---------------------------
	const char* dir = "/data/xce/friendrank/data/log/data/";
	set<string> fnames;
	TravesalDir(dir, fnames);
	for (set<string>::const_iterator iter = fnames.begin();
			iter != fnames.end(); ++iter)
	{
		if (strncmp(iter->c_str(), "action.", 7)!=0)
			continue;
		string fullname = dir+*iter;
		MCE_INFO(fullname.c_str());
		ranker->UpdateRank(fullname.c_str());
		//:TODO: remove the log file
		MCE_INFO("\t>>> remove "<<fullname.c_str());
		//remove(fullname.c_str());
		string destname = "/data/xce/friendrank/data/log/backup/" + *iter;
		rename(fullname.c_str(), destname.c_str());
	}

	//---------------------------
	delete ranker;
	time_t end = time(NULL);
	MCE_INFO("[UpdateActiveRank] Time used "<<end-start<<" second");
	return 0;
}

