#include "Consumer.h"

void Consumer::Setup()
{
	// Do any setup here
	// 不允许pthread_cancel机制取消，使用自定义标志位取消机制
	int code = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	if (code!=0)
	{
		MCE_INFO("Error ("<<strerror(code)<<") pthread_setcancelstate in Thread::Start!");
	}

	//// 允许取消：收到cancel信号后设为cancle态
	//int code = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	//if (code!=0)
	//{
	//	fprintf(stderr, "Error (%s) pthread_setcancelstate in Thread::Start!\n", strerror(code));
	//}
	//// 收到cancel信号后推迟到取消点处理
	//code = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	//if(code != 0)
	//{
	//	fprintf(stderr, "Error (%s) pthread_setcanceltype in Thread::Start!\n", strerror(code));
	//}
}

void Consumer::Execute(void* arg)
{
	// Your code goes here
	ThreadParam* threadParam = (ThreadParam*)arg;
	StaticRankHead headStaticRank;
	
	// 为了更大的并行计算，输出文件采用一个线程一个文件的方式
	char fname[256];
	sprintf(fname, "/data/xce/friendrank/data/friend_rank/static_rank/static_rank.dat_%ld",
			pthread_self());
	FILE* fpRank = fopen(fname, "wb");
	assert(fpRank!=NULL);
	
	int span = 0;
	int slotpos = 0;
	while (true)
	{
		slotpos = threadParam->GetFullSlot();
		if (slotpos==-1)	{
			//threadParam->Print();
			//pthread_testcancel();   // 取消点, sem_wait也是取消点
			if (threadParam->GetDone())   // 自定义取消点机制，系统取消点实现原理也是如此
			{
				MCE_INFO("\tConsumer "<<pthread_self()<<" receive exit signal");
				break;
			}

			if (++span % 10000 == 0)
			{
				waitTime += span / 100;
				span = 0;
				MCE_INFO("\t\t\t\t\t\tConsumer "<<pthread_self()<<" wait "<<waitTime<<"s");
			}
			usleep(10000);  // microseconds
		}	else {
			ComputeRank(threadParam->dataUserModels[slotpos], headStaticRank, buffStaticRank);
			threadParam->FreeSlot(slotpos);
			SaveFriendRank(fpRank, headStaticRank, buffStaticRank);
		}
	}
	
	fclose(fpRank);
	MCE_INFO("\tConsumer "<<pthread_self()<<" exit");
}

// ComputeRank {{{
//   buffModels     : [in]存储用户及其好友的静态资料在Cache中的指针
//   headStaticRank : [out]{uid,num}
//   buffStaticRank : [out]存储好友的StaticRank
int Consumer::ComputeRank(char**& buffModels, 
		StaticRankHead& headStaticRank, StaticRank* buffStaticRank)
{
	//int friendnum = *(int*)buffModels;
	//char* pp = buffModels + sizeof(int);

	//UserModel meModel(0);
	////UserModel frModel(0);
	//meModel.FromBuffer(pp+sizeof(int), *(int*)pp);
	//pp += sizeof(int) + *(int*)pp;

	//memset(friendbuff, 0, IDLENGHT);
	//for (set<int>::const_iterator iter = meModel.friends.begin();
	//		iter != meModel.friends.end(); ++iter)
	//	friendbuff[*iter%IDLENGHT] = 1;

	// 拷贝用户模型数据
	//int realnum = 0;
	//for (int i=0; i<friendnum && i<MAXFRIENDNUM; ++i)
	//{
	//	//frModel.FromBuffer(pp+sizeof(int), *(int*)pp);
	//	//meModel.ComputeRelevance(frModel, buffStaticRank[i]);
	//	meModel.ComputeRelevanceByBuff(pp+sizeof(int), *(int*)pp, buffStaticRank[i], friendbuff);
	//	pp += sizeof(int) + *(int*)pp;
	//	++realnum;
	//}
	//headStaticRank.Init(meModel.uid, realnum);
	
	// 使用用户模型数据的指针
	UserModel meModel(0);
	UserModelHead* head = (UserModelHead*)(buffModels[0]);
	meModel.FromBuffer(buffModels[0]+sizeof(UserModelHead), head->len);
	memset(friendbuff, 0, IDLENGHT);
	for (set<int>::const_iterator iter = meModel.friends.begin();
			iter != meModel.friends.end(); ++iter)
		friendbuff[*iter%IDLENGHT] = 1;

	int realnum = 0;  // 实际好友数
	char* data = NULL;// 用户数据首指针
	int size = 0;     // 用户数据长度
	while (buffModels[realnum+1]!=NULL)
	{
		head = (UserModelHead*)(buffModels[realnum+1]);
		size = head->len;
		data = (char*)head + sizeof(UserModelHead);
		meModel.ComputeRelevanceByBuff(data, size, buffStaticRank[realnum],
				friendbuff, IDLENGHT);
		++realnum;
	}
	headStaticRank.Init(meModel.uid, realnum);
	
	//printf("uid=%d\n", meModel.uid);
	//for (int i=0; i<realnum; ++i)
	//{
	//	buffStaticRank[i].Print();
	//	printf("\n");
	//}
	//getchar();
	return 0;
}// }}}

// SaveFriendRank {{{
void Consumer::SaveFriendRank(FILE* fpRank,
		StaticRankHead& headStaticRank, StaticRank* buffStaticRank)
{
	fwrite(&headStaticRank, sizeof(StaticRankHead), 1, fpRank);
	fwrite(buffStaticRank, sizeof(StaticRank), headStaticRank.num, fpRank);
}// }}}

