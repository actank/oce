#ifndef ADMATCHI_H_
#define ADMATCHI_H_

#include "AdEngine.h"
#include "Util.h"
#include "TaskManager.h"
#include "Singleton.h"
#include "AdStruct.h"
#include <bits/stl_multimap.h>
#include "FeedMemcProxy/client/user_profile_client.h"
namespace xce {
namespace ad {
using namespace MyUtil;
using namespace std;

extern string DB_INSTANCE;

class TimeStat{
        timeval _begin, _end;
public:
        TimeStat(){
                reset();
        }
        void reset(){
                gettimeofday(&_begin, NULL);
        }
        float getTime(){
                gettimeofday(&_end, NULL);
                float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
                                -_begin.tv_usec;
                timeuse/=1000;
                return timeuse;
        }
};


struct LogParas{
public:
  LogParas(){
    server_index_ = 0;
    cost_ = 0;
    click_pos_ = 0;
    max_price_ = 0;
    ad_count_ = 0;
    user_key_ = 0;
    bid_unit_key_ = 0;
    bid_unit_id_ = 0;
    creative_id_ = 0;
    zone_id_ = 0;
    pv_time_ = 0;
    create_time_ = 0;
    ubc_ = "";
  }
  int server_index_;
  int cost_;
  int click_pos_;
  int max_price_;
  int ad_count_;
  uint64_t user_key_;
  uint64_t bid_unit_key_;
  Ice::Long bid_unit_id_;
  Ice::Long creative_id_;
  Ice::Long zone_id_;
  time_t pv_time_;
  time_t create_time_;
  UserProfile user_profile_;
  string ubc_;
};

class LogHelper : public Singleton<LogHelper> {
public:
  string MakeText(const LogParas& para);
  LogParas PaserText(const string& text);
  void CookieEmptyCheck(UserProfile& upf);
  void Process(string& str);
  string MakeUbc(const UserProfile& upf);
  string MakeHttp(const LogParas& para);
  string MakeIp(int ip);
  string FormatedClickTime(time_t t);
};

struct UserBind{
  int userid;
  Ice::Long zoneid;
  bool operator >(const UserBind& ul) const {
    if(userid != ul.userid)
      return userid > ul.userid;
    return zoneid > ul.zoneid;
  }
  bool operator ==(const UserBind& ul) const {
    return ( (userid == ul.userid) && (zoneid == ul.zoneid));
  }
  bool operator <(const UserBind& ul) const {
    if(userid != ul.userid)
      return userid < ul.userid;
    return zoneid < ul.zoneid;
  }
};

class AdMatchManagerI: public AdMatchManager, public Singleton<AdMatchManagerI> {
public:
  AdMatchManagerI(){
    init_ok_ = false;
  }
  virtual AdResultMap GetAds(const string& userProfile, const string& referer, int rotate_index,
      const Ice::Current& = Ice::Current());
  virtual void Click(int uid, const string& ip, const string& text,const string& referer ,const string& click_url ,const Ice::Current& =
      Ice::Current());
  virtual AdResultMap GetAdsTest(::Ice::Int uid, ::Ice::Int stage,
      ::Ice::Int gender, ::Ice::Int age, const ::std::string& currentArea,
      const ::std::string& ipArea, ::Ice::Long school, ::Ice::Int grade,
      int count, const string & referer, int rotate_index, const Ice::Current& = Ice::Current());

  virtual void BindUser(int userid, Ice::Long creativeid, Ice::Long zoneid, const Ice::Current& = Ice::Current());
  virtual void UnbindUser(int userid, Ice::Long zoneid, const Ice::Current& = Ice::Current());
  virtual void NotifyBindUser(int userid, Ice::Long creativeid, Ice::Long zoneid, const Ice::Current& = Ice::Current());
  virtual void NotifyUnbindUser(int userid, Ice::Long zoneid, const Ice::Current& = Ice::Current());

  void FilterBindUser(AdZoneDict& zone_dict, AdResultMap& ans, int userid);
  void UserBindCacheInit();
  void PvLog(const set<AdGroupPtr>& zone_selected, const UserProfile& profile, uint64_t user_key, AdInfoSeq& ad_info_seq, const string& referer, const AdZonePtr& zone_ptr, const map<Ice::Long, int>& group_costs, int rotate_index);
  //void ProcessUrl(string& str);
  void Init();
  void InitOk(){
    init_ok_ = true;
  }
  void SetServerIndex(int server_index){
    server_index_ = server_index;
  }
  int ServerIndex(){
    return server_index_;
  }
private:
  bool init_ok_;
  int server_index_;


  IceUtil::RWRecMutex mutex_;
  map<UserBind, Ice::Long> user_bind_cache_;
  typedef map<UserBind, Ice::Long> UserBindMap;
};

class InitTask : virtual public MyUtil::Task{
public:
  InitTask(){}
  virtual void handle();
};

class ReloadTimer : virtual public MyUtil::Timer{
public:
  ReloadTimer() : Timer(5 * 60 * 1000) {
  }
  ;
  virtual void handle();
};

}
}

#endif
