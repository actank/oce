#include "Thread.h"
#include <errno.h>
#include <string>
#include "UtilCxx/src/LogWrapper.h"  // for MCE_INFO
using namespace MyUtil;

Thread::Thread() {}
Thread::~Thread() {}

int Thread::Start(void * arg)
{
	Arg(arg); // store user data
	/* Initialize and set thread detached attribute */
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int code = pthread_create(&ThreadId_, &attr, Thread::EntryPoint, this);
	if (code!=0)
	{
		MCE_INFO("Error ("<<strerror(code)<<") pthread_create in Thread::Start!");
	}

	return code;
}

int Thread::Run(void * arg)
{
	Setup();
	Execute(arg);
	return 0;
}

/*static */
void * Thread::EntryPoint(void * pthis)
{
	Thread * pt = (Thread*)pthis;
	pt->Run( pt->Arg() );
	return NULL;
}

void Thread::Setup()
{
	// Do any setup here
}

void Thread::Execute(void* arg)
{
	// Your code goes here
}

// 根据不同函数实现，系统很多函数都不确定是否为取消点
// 若对子线程何时终止敏感，为了避免死锁，建议不要使用该函数
//   使用自定义标志位来解决主线程通知子线程终止的情形
int Thread::CancelThread()
{
	int status = pthread_cancel(ThreadId_);
	if(status == 0){
		//printf("cancel thread [%ld], success.\n", ThreadId_);
		MCE_INFO("cancel thread ["<<ThreadId_<<"], success.");
	}
	return 0;
}

int Thread::WaitForEnd()
{
	int status;
	int rc = pthread_join(ThreadId_, (void **)&status);
	if (rc)
	{
		//fprintf(stderr, "[Error] Thread [%ld] return (%d:%s) from pthread_join()\n", 
		//		ThreadId_, rc, strerror(rc));
		MCE_INFO("[Error] Thread ["<<ThreadId_<<"] return ("
				<<rc<<":"<<strerror(rc)<<") from pthread_join()");
		return -1;
	}
	//fprintf(stderr, "Completed join with thread %ld status= %d\n", ThreadId_, status);
	MCE_INFO("Completed join with thread "<<ThreadId_<<" status= "<<status);
	return 0;
}

