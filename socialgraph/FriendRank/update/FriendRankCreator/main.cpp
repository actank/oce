#include <signal.h>
#include <unistd.h>
#include "FriendRankCreator.h"

void CreateFriendRank_SigRoutine(int sig);
void CreateFriendRank_Initial();
void CreateFriendRank_Cleanup();

FriendRankCreator* creator = NULL;

int CreateFriendRank(void) // {{{
{
	time_t start = time(NULL);
	CreateFriendRank_Initial();

	// 循环处理每一个静态分值文件
	const char* dir = "/data/xce/friendrank/data/friend_rank/static_rank";
	set<string> fnames;
	TravesalDir(dir, fnames);
	for (set<string>::const_iterator iter = fnames.begin();
			iter != fnames.end(); ++iter)
	{
		string fullname = dir + string("/") + *iter;
		creator->CreateFriendRank(fullname.c_str());
	}

	CreateFriendRank_Cleanup();
	time_t end = time(NULL);
	//printf("time used %ld second\n", end-start);
	MCE_INFO("[CreateFriendRank] Time used "<<end-start<<" second");
	return 0;
}// }}}

void CreateFriendRank_SigRoutine(int sig) // {{{
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

	CreateFriendRank_Cleanup();
	exit(0);
}// }}}
void CreateFriendRank_Initial() // {{{
{
	//PrintTime();
	//signal(SIGHUP, CreateFriendRank_SigRoutine); //* 下面设置三个信号的处理方法
	//signal(SIGQUIT, CreateFriendRank_SigRoutine);
	//signal(SIGINT, CreateFriendRank_SigRoutine);

	creator = new FriendRankCreator;
}
void CreateFriendRank_Cleanup()
{
	//printf("\nCleaning jobs:\n");
	MCE_INFO("Cleaning jobs:");
	delete creator;

	//PrintTime();
}// }}}

