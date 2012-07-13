#ifndef _NOTIFY_CONTENT_H_
#define _NOTIFY_CONTENT_H_

#include "Notify.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "NotifyNotePool.h"


namespace xce {
namespace notify { 

class NotifyContentManagerI: 
		public NotifyContentManager,
		public MyUtil::Singleton<NotifyContentManagerI>{
public:	
	virtual NotifyContentDict getNotifyDict(const MyUtil::LongSeq& notifyIds,
									   const Ice::Current& = Ice::Current());
	virtual void addNotify(const NotifyContentPtr& notify,
						  const Ice::Current& = Ice::Current());
	virtual void removeNotifies(const MyUtil::LongSeq &NotifyIds,
			const Ice::Current& = Ice::Current());
};

class CleanNotifyCacheTimer: public MyUtil::Timer {
public:
	CleanNotifyCacheTimer() :
		MyUtil::Timer(3600 * 1000) {
	}
	virtual void handle();

};


class TimeStat{
  timeval _begin, _end;
  bool _log;
public:
  TimeStat(bool log=false){
    _log = log;
    reset();
  }
  ~TimeStat(){
    if(_log){
      MCE_INFO("######### " << getTime());
    }
  }
  void reset(){
    gettimeofday(&_begin, NULL);
  }
  float getTime(bool clear = true){
    gettimeofday(&_end, NULL);
    float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
    -_begin.tv_usec;
    timeuse/=1000;
    if(clear){
      reset();
    }
    return timeuse;
  }
};

class PrintHelper{
public:
  template<typename T>
  static string Print(const T & container,int limit = 100) { //将容器内容打印成字符串
    ostringstream ios;
    ios << "(";
    bool first_item = true;
    int i = 0;
    int cnt = 0;
    for (typename T::const_iterator itr = container.begin(); itr
        != container.end(); ++itr,++i) {
      if (first_item == true) {
        first_item = false;
      } else {
        ios << ",";
      }
      ios << *itr;
      if(cnt >= limit){
        break;
      }
    }
    ios << ")";
    return ios.str();
  }
};

}
}

#endif //_NOTIFY_CONTENT_H_


