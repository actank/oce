#ifndef _CONSUMER_H_
#define _CONSUMER_H_

#include "../common/Common.h"
#include "../common/UserModel.h"
#include "ThreadParam.h"
#include "Thread.h"

class Consumer : public Thread
{
#define MAXFRIENDNUM 2000   // 最大好友个数限制
#define IDLENGHT 1000000    // 计算共同好友时，使用后6位数字来标识该好友
	public:
		Consumer() {
			buffStaticRank = new StaticRank[MAXFRIENDNUM];
			friendbuff = new char[IDLENGHT];
			assert(friendbuff!=NULL);
			waitTime = 0;
		}
		virtual ~Consumer() {
			MCE_INFO("\tRelease Consumer resource.");
			delete[] friendbuff;
			friendbuff = NULL;
			delete[] buffStaticRank;
			buffStaticRank = NULL;
		}
		void PrintInfo() const {
		}

	private:
		virtual void Setup();
		virtual void Execute(void* arg);
		int ComputeRank(char**& buff, 
				StaticRankHead& headStaticRank, StaticRank* buffStaticRank);
		void SaveFriendRank(FILE* fpRank,
				StaticRankHead& headStaticRank, StaticRank* buffStaticRank);

	private:
		StaticRank* buffStaticRank;
		char* friendbuff;
		long waitTime;
};

#endif

