/**********************************|
|***** Author:tinghaier   *********|
|***** Created on:Dec 8.2011*******|
|**********************************/
#include <ctime>
#include "feed/base_feed/ptime.h"
#include "feed/feedview/ambcache.h"
#include "LogWrapper.h"
#include "QueryRunner.h"
#include "UtilCxx/src/Date.h"

using namespace com::xiaonei::xce;
namespace xce{
namespace feed{
int AmbCache::Get(int uid){
  IceUtil::RWRecMutex::RLock lock(cache_rw_mutex_);
  map<int,int>::iterator iter=amb_map.find(uid); 
  return iter!=amb_map.end()?(*iter).second:0;
}

void AmbCache::LoadAll(){
  struct timeval start, end;
  gettimeofday(&start, NULL);
  Statement sql;
  sql << "SELECT uid,type FROM feed_ambassador limit 50000";
  mysqlpp::StoreQueryResult res = QueryRunner("feed_db", CDbRServer, "feed_ambassador").store(sql);
  map<int,int> temp_map;
  int uid,type;
  for (size_t i = 0; i < res.size(); ++i) {
    uid = (int) res.at(i)["uid"];
    type= (int) res.at(i)["type"];
    temp_map.insert(pair<int,int>(uid,type));
  }
  gettimeofday(&end, NULL);
  double timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
  MCE_INFO("AmbCache::LoadAll -->LoadFrom DB used time : "<<timeuse/1000000);
  IceUtil::RWRecMutex::WLock lock(cache_rw_mutex_);
  amb_map.swap(temp_map);
  MCE_INFO("AmbCache::LoadAll--> Load done.Add: " << amb_map.size());
}
void AmbLoadTask::handle(){
  MyUtil::Date d=MyUtil::Date(time(NULL));
  if(d.hour()%24==2){//2:00
    AmbCache::instance().LoadAll();
  }
}
void AmbInitTask::handle(){
  MyUtil::Date d=MyUtil::Date(time(NULL));
  if(d.hour()%24 != 2){
    AmbCache::instance().LoadAll();
  }
}
}
}
