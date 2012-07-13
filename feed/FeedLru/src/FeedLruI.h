/*
 * FeedQueueI.h
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#ifndef FeedLRUI_H_
#define FeedLRUI_H_

#include "FeedLru.h"
#include "Singleton.h"
#include <IceUtil/IceUtil.h>

#include "ObjectCacheI.h"
using namespace std;
using namespace MyUtil;

namespace xce {
namespace feed {



class MyFactory :  public MyUtil::ServantFactory{
public:
	virtual Ice::ObjectPtr create(Ice::Long id){
		LruItemPtr item = new LruItem;
		item->key = id;
		item->value = time(NULL);
		return item;
	}
};

class FeedLruI: public FeedLru, public MyUtil::Singleton<FeedLruI> {
public:
	virtual void Add(int key,
			const Ice::Current& = Ice::Current());
	virtual int Get(int key,const Ice::Current& = Ice::Current());
	virtual void RunTask(int start,const Ice::Current& = Ice::Current());
	virtual void Schedule(int seconds,const Ice::Current& = Ice::Current());
	virtual void InsertDb(int uid,int feature,const Ice::Current & = Ice::Current());
	virtual void SelectDb(int uid,const Ice::Current & = Ice::Current());
private:

};

class PrintTask:public MyUtil::Task{
public:
	PrintTask(int start):start_(start){}
	virtual void handle(){
		int orig = start_;
		for(unsigned i = 0; i < 5;++i){
			MCE_INFO("Start from " << orig << ", " << start_);
			++start_;
			sleep(1);
		}
	}
private:
	int start_;
};

class MyTimerTask:public MyUtil::Timer{
public:
	MyTimerTask(int delay):Timer(delay),delay_(delay){	}
	virtual void handle(){
		MCE_INFO("MyTimerTask::handle. delay:" << delay_);
	}
private:
	int delay_;
};


}
}
#endif /* FEEDQUEUEI_H_ */
