#include "ThreadParam.h"

// Constructor & Desstructor {{{
ThreadParam::ThreadParam()
{
	_done = false;
	pthread_rwlock_init(&_rwlockDone, NULL);

	pthread_mutex_init(&_mutexPool, NULL);
	//sem_init(&_occupied, 0, 0);
	//sem_init(&_empty, 0, POOLSIZE);

	for (int i=0; i<POOLSIZE; ++i)
	{
		dataUserModels[i] = new char*[MAXFRIENDNUM+2];  // 我+2000个好友+结束符NULL
		_idle[i] = -1;
	}
}
ThreadParam::~ThreadParam()
{
	for (int i=0; i<POOLSIZE; ++i)
	{
		delete[] dataUserModels[i];
		dataUserModels[i] = NULL;
		_idle[i] = -1;
	}

	//sem_destroy(&_occupied);
	//sem_destroy(&_empty);
	pthread_mutex_destroy(&_mutexPool);
	pthread_rwlock_destroy(&_rwlockDone);
}// }}}

// FreeSlot {{{
void ThreadParam::FreeSlot(int i)
{
	_idle[i] = -1;
}// }}}
// FullSlot {{{
void ThreadParam::FullSlot(int i)
{
	_idle[i] = 1;
}// }}}

// GetFreeSlot {{{
// 返回空闲槽的位置，如果没有空闲槽，返回-1
int ThreadParam::GetFreeSlot()
{
	int pos = -1;
	pthread_mutex_lock(&_mutexPool);
	//sem_wait(&_empty);
	for (int i=0; i<POOLSIZE; ++i)
	{
		if (_idle[i]==-1) // -1 空闲块
		{
			pos = i;
			_idle[i] = 0;   // 正在填充
			break;
		}
	}
	pthread_mutex_unlock(&_mutexPool);
	//sem_post(&_occupied);
	return pos;
}// }}}
// GetFullSlot {{{
// 返回满槽的位置，如果没有满槽，返回-1
int ThreadParam::GetFullSlot()
{
	int pos = -1;
	pthread_mutex_lock(&_mutexPool);
	//sem_wait(&_occupied);
	for (int i=0; i<POOLSIZE; ++i)
	{
		if (_idle[i]==1)  // +1 有数据需要处理
		{
			pos = i;
			_idle[i] = 0;   // 正在处理
			break;
		}
	}
	pthread_mutex_unlock(&_mutexPool);
	//sem_post(&_empty);
	return pos;
}// }}}

// Print {{{
void ThreadParam::Print() const
{
	printf("\n");
	//pthread_mutex_lock(&_mutexPool);  // 为了性能考虑，允许读出脏数据
	for (int i=0; i<POOLSIZE; ++i)
	{
		switch (_idle[i])
		{
			case -1:
				printf("0");
				break;
			case 1:
				printf("1");
				break;
			case 0:
				printf("-");
				break;
			default:
				printf("?");
				break;
		}
	}
	//pthread_mutex_unlock(&_mutexPool);
	printf("\n");
	fflush(stdout);
}// }}}

