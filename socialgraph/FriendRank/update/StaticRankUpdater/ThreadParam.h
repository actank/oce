#ifndef _TRHEAD_PARAM_H_
#define _TRHEAD_PARAM_H_

#include "../common/Common.h"
#include "../common/StaticRank.h"
#include "../common/ActiveRank.h"
#include <semaphore.h>

class ThreadParam
{
#define MAXFRIENDNUM 2000  // 最大好友个数限制2000
#define POOLSIZE 1000      // 缓冲区最大数量
	public:
		ThreadParam();
		virtual ~ThreadParam();
		
		void SaveFriendRank(StaticRankHead& headStaticRank, StaticRank* buffStaticRank);
		void FreeSlot(int i);
		void FullSlot(int i);
		// 返回空闲槽的位置，如果没有空闲槽，返回-1
		int GetFreeSlot();
		// 返回满槽的位置，如果没有满槽，返回-1
		int GetFullSlot();
		void Print() const;
		void SetDone(bool done)
		{
			pthread_rwlock_wrlock(&_rwlockDone);
			this->_done = done;
			pthread_rwlock_unlock(&_rwlockDone);
		}
		bool GetDone(void)
		{
			bool done = false;
			pthread_rwlock_rdlock(&_rwlockDone);
			done = this->_done;
			pthread_rwlock_unlock(&_rwlockDone);
			return done;
		}

	public:
		char** dataUserModels[POOLSIZE];
	private:
		// 使用semphone也可以实现Producer-Consumer Model
		// 这里线程结束的条件是必须把事情处理完，所以不能使用sem_wait做为取消
		//sem_t _occupied;
		//sem_t _empty;
		int _idle[POOLSIZE];  // [-1] 空闲; [+1] 有数据; [0] 正在进行
		pthread_mutex_t _mutexPool;  // for _idle
		
		bool _done;   // true:数据处理完毕，可以结束；false:数据还未处理完
		pthread_rwlock_t  _rwlockDone;
};

#endif

