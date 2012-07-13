/*************************************|
|***** Author:tinghaier     **********|
|***** Created on:Dec 8,2011**********|
|*************************************/
#ifndef AMB_CACHE_H_
#define AMB_CACHE_H_
#include <map>
#include <IceUtil/IceUtil.h>
#include "TaskManager.h"
#include "Singleton.h"

namespace xce{
namespace feed{

class AmbCache:public MyUtil::Singleton<AmbCache>{
public:
  int Get(int uid);
  void LoadAll();//load all from feed_ambassador
  AmbCache(){}
  ~AmbCache(){
    amb_map.clear();
  }
private:
  IceUtil::RWRecMutex cache_rw_mutex_;
  map<int,int> amb_map;
};
class AmbLoadTask:public MyUtil::Timer{
public:
  AmbLoadTask():Timer(1000*60*60){}//1 hours
  virtual void handle();
};
class AmbInitTask:public MyUtil::Task{
public:
  AmbInitTask(){}
  virtual void handle();
};

}
}
#endif
