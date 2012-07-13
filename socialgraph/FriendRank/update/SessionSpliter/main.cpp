//#include <signal.h>
//#include <unistd.h>
//#include "SessionSpliter.h"
//
//void SigRoutine(int dunno);
//void Initial();
//void Cleanup();
//
//// main {{{
//int main(int argc, char* argv[])
//{
//	time_t start = time(NULL);
//	Initial();
//
//	const char* dir = "../data/log";
//	set<string> fnames;
//	TravesalDir(dir, fnames);
//
//	// 循环处理每一天的log
//	for (set<string>::const_iterator iter = fnames.begin();
//			iter != fnames.end(); ++iter)
//	{
//		if (strncmp(iter->c_str(), "action.", 7)!=0)
//			continue;
//		string fullname = dir + string("/") + *iter;
//		printf("%s\n", fullname.c_str());
//
//		SessionSpliter::SplitSession(dir, iter->c_str());
//		
//		printf("\t>>> remove %s\n", fullname.c_str());
//		remove(fullname.c_str());
//	}
//
//	Cleanup();
//	time_t end = time(NULL);
//	printf("time used %ld second\n", end-start);
//	return 0;
//}// }}}
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
//	signal(SIGHUP, SigRoutine); // 设置三个信号的处理方法
//	signal(SIGQUIT, SigRoutine);
//	signal(SIGINT, SigRoutine);
//}
//void Cleanup()
//{
//	printf("\nCleaning jobs:\n");
//	PrintTime();
//}// }}}
//

#include "SessionSpliter.h"

int SplitSession(void)
{
	time_t start = time(NULL);
	const char* dir = "/data/xce/friendrank/data/log";
	set<string> fnames;
	TravesalDir(dir, fnames);

	//---------------------------
	// 循环处理每一天的log
	for (set<string>::const_iterator iter = fnames.begin();
			iter != fnames.end(); ++iter)
	{
		if (strncmp(iter->c_str(), "action.", 7)!=0)
			continue;
		string fullname = dir + string("/") + *iter;
		MCE_INFO(fullname.c_str());

		SessionSpliter::SplitSession(dir, iter->c_str());
		
		MCE_INFO("\t>>> remove "<<fullname.c_str());
		remove(fullname.c_str());
	}

	//---------------------------
	time_t end = time(NULL);
	MCE_INFO("[SplitSession] Time used "<<end-start<<" second");
	return 0;
}

