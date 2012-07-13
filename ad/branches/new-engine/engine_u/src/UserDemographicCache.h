/*
 * UserDemographicCache.h
 *
 *  Created on: Apr 19, 2011
 *      Author: kingsfield
 */

#ifndef USERDEMOGRAPHICCACHE_H_
#define USERDEMOGRAPHICCACHE_H_

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <sstream>
#include <Ice/Ice.h>
#include "Date.h"
#include "Singleton.h"
#include "FeedMemcProxy/client/user_profile_client.h"
#include "AdStruct.h"
#include "AdDemoQuality.h"
#include "IceLogger.h"

using namespace xce::ad;
using namespace std;
class UserDemographicCache : public MyUtil::Singleton<UserDemographicCache> {
public:
  pair<double,int> GetUserAdGroupsCtr(const UserProfile & user_profile, Ice::Long zone_id ,int pos_id, Ice::Long group_id,Ice::Long sponsor_id,string app_name);
  void Load();
private:
  static const int NO_USE = -1;
  static const int FLAG_GENDER = 1;
  static const int FLAG_STAGE = 2;

  double GetTwoFeatureCtr(string zone_id, Ice::Long sponserID, Ice::Long adGroupID, int pos ,int gender,int stage);

  pair<double,int> GetDemoCtr(string zone_id,Ice::Long sponserID,  Ice::Long adGroupID, int pos, int gender,int stage, int uid);

  void MY_MCE_DEBUG(string zone_id, Ice::Long sp_id ,Ice::Long group_id,int pos ,int gender,int stage, double ctr, int uid, int state);

  string GetKey(string zone_id, Ice::Long sp_id ,Ice::Long group_id,int pos ,int gender,int stage){
    sp_id = sp_id * 10 + pos;
    group_id = group_id * 10 + pos;
    ostringstream sin;
    sin << zone_id <<"_"<< sp_id <<"_"<< group_id <<"_" <<gender<<"_"<<stage;
    return sin.str();
  };  

  string GetKey(Ice::Long zone_id, Ice::Long sp_id ,Ice::Long group_id,int pos ,int gender,int stage){
    sp_id = sp_id * 10 + pos;
    group_id = group_id * 10 + pos;
    ostringstream sin;
    sin << zone_id <<"_"<< sp_id <<"_"<< group_id <<"_" <<gender<<"_"<<stage;
    return sin.str();
  };  



  double GetCtr(string zone_id, Ice::Long sp_id ,Ice::Long group_id,int pos ,int gender,int stage){
    string key = GetKey(zone_id,sp_id,group_id,pos ,gender,stage);
    AdQualityMap::iterator iter;
    IceUtil::RWRecMutex::RLock lock(mutex_);    
    iter = demographTable.find(key);
    if(iter != demographTable.end())
    {
        return iter->second;
    }
    else
    {
        return -1.0;
    }
  };



  AdQualityMap demographTable;
  IceUtil::RWRecMutex mutex_;

};

class UserDemographicCacheTimer : public MyUtil::Timer {
public:
  UserDemographicCacheTimer() :
    Timer(3 * 60 * 1000) { //60分钟重新计算一次
  }
  virtual void handle() {
    UserDemographicCache::instance().Load();
  }
};


#endif /* USERDEMOGRAPHICCACHE_H_ */
