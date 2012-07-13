#ifndef ADMATCHI_H_
#define ADMATCHI_H_

#include "AdEngine.h"
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include "TaskManager.h"
#include "Singleton.h"
#include "AdStruct.h"
#include <bits/stl_multimap.h>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/regex.hpp>
#include "FeedMemcProxy/client/user_profile_client.h"
//#include "AdEffect.h"

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace std;

/***********************************************************************************************************************/
struct LogParas{
public:
  LogParas() : append_info_("-"){
    cost_ = 0;
    pos_ = 0;
    max_price_ = 0;
    ad_count_ = 0;
    bid_unit_id_ = 0;
    creative_id_ = 0;
    zone_id_ = 0;
    pv_time_ = 0;
    create_time_ = 0;
    ubc_ = "";
    next_load_time_ = "";
    refresh_idx_ = "";
  }
  int cost_;
  int pos_;
  int max_price_;
  int ad_count_;
  Ice::Long bid_unit_id_;
  Ice::Long creative_id_;
  Ice::Long zone_id_;
  time_t pv_time_;
  time_t create_time_;
  UserProfile user_profile_;
  string ubc_;
  string next_load_time_;
  string refresh_idx_;
  string append_info_;
};

class LogHelper : public Singleton<LogHelper> {
public:
  string MakeText(const LogParas& para);
  LogParas ParseText(const string& text);
  void Process(string& str);
  string MakeUbc(const UserProfile& upf);
  string MakeHttp(const LogParas& para);
  string MakeIp(int ip);
  string FormatedClickTime(time_t t);
  string GetAppId(string refer);
};

/***********************************************************************************************************************/
class AdMatchHandler;
typedef IceUtil::Handle<AdMatchHandler> AdMatchHandlerPtr;

class AdMatchManagerI: public AdMatchManager, public Singleton<AdMatchManagerI> {
public:
  AdMatchManagerI();
  virtual AdResultMap GetAds(const string& userProfile, const string& append_para, int rotate_index,
      const Ice::Current& = Ice::Current());
  virtual ::xce::ad::AdZone2Creatives GetAdsByZoneInfo(const ::std::string&, const ::std::string&, ::Ice::Int, const ::xce::ad::ZoneInfoMap&, const ::Ice::Current& = ::Ice::Current()) {
    return AdZone2Creatives();
  } 
  virtual void Click(int uid, const string& ip, const string& text,const string& referer ,const string& click_url ,const Ice::Current& =
      Ice::Current());
  virtual void Close(::Ice::Int, ::Ice::Long, const ::Ice::Current& = ::Ice::Current()) {}
  virtual AdResultMap GetAdsTest(::Ice::Int uid, ::Ice::Int stage,
      ::Ice::Int gender, ::Ice::Int age, const ::std::string& currentArea,
      const ::std::string& ipArea, ::Ice::Long school, ::Ice::Int grade,
      int count, const string & referer, int rotate_index, const Ice::Current& = Ice::Current());
 virtual AdZone2Creatives GetAds4Monitor(Ice::Int, Ice::Int, Ice::Int, Ice::Int, const std::string&, const std::string&, Ice::Long, Ice::Int, Ice::Int, const std::string&, Ice::Int, const Ice::Current&){
	return AdZone2Creatives();
}

  virtual void BindUser(int user_id, Ice::Long creative_id, Ice::Long zone_id, const Ice::Current& = Ice::Current());
  virtual void UnbindUser(int user_id, Ice::Long zone_id, const Ice::Current& = Ice::Current());
  virtual void NotifyBindUser(int user_id, Ice::Long creative_id, Ice::Long zone_id, const Ice::Current& = Ice::Current());
  virtual void NotifyUnbindUser(int user_id, Ice::Long zone_id, const Ice::Current& = Ice::Current());

  void FilterBindUser(AdZoneDict& zone_dict, AdResultMap& ans, int userid);
  void Init();
  void InitOk(){
    init_ok_ = true;
  }
  virtual ::xce::ad::AdZone2Creatives GetAdsByZone(const ::std::string&, const ::std::string&, ::Ice::Int, const ::Ice::Current& = ::Ice::Current());
private:
  bool init_ok_;
  vector<AdMatchHandlerPtr> ad_match_handler_list_;

  IceUtil::RWRecMutex mutex_;
};

class InitTask : virtual public MyUtil::Task{
public:
  InitTask(){}
  virtual void handle();
};

/*class ReloadTimer : virtual public MyUtil::Timer{
public:
  ReloadTimer() : Timer(1 * 60 * 1000) {
  }
  virtual void handle();
};*/

class ReloadTask : virtual public MyUtil::Task{
public:
  ReloadTask(){}
  virtual void handle();
};

class Filter : public Singleton<Filter> {
public:
  void FilterCompete(int uid, set<AdGroupPtr> & groups, const set<SelectedAdGroupPtr> & selected);
};

}
}

#endif
