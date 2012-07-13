#ifndef  _THREAD_H_
#define  _THREAD_H_

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

class Thread
{
	public:
		Thread();
		virtual ~Thread();
		int Start(void * arg);
		int WaitForEnd();
		int CancelThread();
	protected:
		int Run(void * arg);
		static void * EntryPoint(void* pthis);
		virtual void Setup();
		virtual void Execute(void* arg);
		void * Arg() const {return Arg_;}
		void Arg(void* a){Arg_ = a;}
	private:
		pthread_t ThreadId_;
		void * Arg_;
};

#endif

