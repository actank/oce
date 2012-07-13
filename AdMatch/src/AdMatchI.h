#ifndef ADMATCHI_H_
#define ADMATCHI_H_

#include "AdMatch.h"
#include "Util.h"
#include "AdInvertedIndex.h"
#include "TaskManager.h"
#include "Singleton.h"
#include <bits/stl_multimap.h>
#include "FeedMemcProxy/client/user_profile_client.h"
namespace xce {
namespace ad {
using namespace MyUtil;
using namespace std;

unsigned gServiceIndex;

class AdMatchI: public AdMatch, public Singleton<AdMatchI> {

  virtual AdInfoSeq GetAds(const string& userProfile, ::Ice::Int count,const string& referer,
      const Ice::Current& = Ice::Current());
  virtual AdInfoSeq GetAdsTest(::Ice::Int uid, ::Ice::Int stage,
      ::Ice::Int gender, ::Ice::Int age, const ::std::string& currentArea,
      const ::std::string& ipArea, ::Ice::Long school, ::Ice::Int grade,
      ::Ice::Int count, const string& referer,const Ice::Current& = Ice::Current());
  virtual void
  Click(int uid, const string& ip, const string& text,const string& referer ,const string& click_url ,const Ice::Current& =
      Ice::Current());

  virtual void
  LoadOneGroup(Ice::Long gid, const Ice::Current& = Ice::Current());
  virtual void
  ReloadOneGroup(Ice::Long gid, const Ice::Current& = Ice::Current());
  virtual void
  DelGroup(Ice::Long gid, const Ice::Current& = Ice::Current());
  //

  virtual void ReportDefault(int uid,const string & ip,int adCount,const DefaultSeq & ads,
      const Ice::Current& = Ice::Current());

  //测试用
  virtual bool SetClickInfo(Ice::Long gid, int key, int click, int pv,
      const Ice::Current& = Ice::Current());

  virtual bool GetAvailableAds(const ::MyUtil::LongSeq& ids,
      const Ice::Current& = Ice::Current());

};
//------------------------------------------------------------------------------
struct FormatParam{
  int click;
  int pv;
  int cost;
  int max_price;
};
//------------------------------------------------------------------------------
class Matcher: public Singleton<Matcher> {
public:
  Matcher();
  void Init();
  AdInfoSeq GetAds(const string & pro_str, int count, string & referer);
  AdInfoSeq GetAdsTest(::Ice::Int uid, ::Ice::Int stage, ::Ice::Int gender,
      ::Ice::Int age, const ::std::string& currentArea,
      const ::std::string& ipArea, ::Ice::Long school, ::Ice::Int grade,
      int count,const string & referer);
  void Load(bool is_first, const Ice::LongSeq& group_ids = Ice::LongSeq());
  bool DelGroup(Ice::Long gid);
  void ReloadOneGroup(Ice::Long gid);
  void Click(int uid, const string & ip, const string& text, string & referer, string & click_url);
  void SetServiceIndex();
  //  void Suspend(Ice::Long gid);
  //  void Resume(Ice::Long gid);
  void GetAvailableAds(const MyUtil::LongSeq & ids);

  void ScanChangedPrices();
  void ScanChangedBidUnits();
  void ScanChangedCreatives();
  bool ready();

  //测试用，设置点击率
  bool SetClickInfo(Ice::Long gid, int key, int click, int pv);

  void  ReportDefault(int uid, const string & ip, int ad_count, const DefaultSeq & ads);
private:
  void ProcessUrl( string & str);

  bool Preprocess(UserProfile & profile, Params & params);
  int CountGroups();
  //  AdInfoSeq Format(AdGroupSeq & groups, int m_key);
  AdInfoSeq Format(vector<pair<AdGroupPtr, FormatParam> > & group_info,
      int m_key, int ad_count, UserProfile & profile,string & referer);



  BidParamsSeq LoadBidUnits(const Ice::LongSeq& group_id_seq);
  map<Ice::Long, CreativeSetPtr> LoadCreatives(const Ice::LongSeq& group_ids);

  vector<pair<AdGroupPtr, int> >
  FilterAdGroup(const AdSetPtr& s1, const AdSetPtr& s2, int ad_count);
  vector<pair<AdGroupPtr, FormatParam > >
  FilterAdGroupB(const AdSetPtr& s1, const AdSetPtr& s2, int ad_count);
  MultiDimIndex m_idx_;
  SchoolIndex s_idx_;
  AreaIndex a_idx_;
  map<Ice::Long, set<Ice::Long> > compete_ref_;
  map<Ice::Long, AdGroupPtr> group_set_;
  IceUtil::RWRecMutex groups_mutex_;

  bool ready_;
  bool allow_scanning_;
  time_t price_time_, bid_time_, crtv_time_;
  long tester_;//测试的member_id;
};
//-----------------------------------------------------------------------------------
class InitTask: public Task {
public:
  InitTask() {
  }
  virtual void handle();
private:
};
//-----------------------------------------------------------------------------------

class ClickTask: public Task {
public:
  ClickTask(int uid, string ip, const string& text,const string & referer,const string & click_url) :
    uid_(uid), ip_(ip), text_(text),referer_(referer) ,click_url_(click_url){
  }
  virtual void handle();
private:
  int uid_;
  string ip_;
  string text_;
  string referer_;
  string click_url_;
};

//-----------------------------------------------------------------------------------
class GetAvailableAdsTask: public Task {
public:
  GetAvailableAdsTask(const MyUtil::LongSeq & ids) :
    ids_(ids) {
    MCE_DEBUG("GetAvailableAdsTask:GetAvailableAdsTask --> ids size = " << ids.size());
  }
  virtual void handle();
private:
  MyUtil::LongSeq ids_;
};
//-----------------------------------------------------------------------------------
class LoadOneGroupTask: public Task {
public:
  LoadOneGroupTask(Ice::Long gid) :
    gid_(gid) {
  }
  virtual void handle();
private:
  Ice::Long gid_;
};
//-----------------------------------------------------------------------------------
class DelGroupTask: public Task {
public:
  DelGroupTask(Ice::Long gid) :
    gid_(gid) {
  }
  virtual void handle();
private:
  Ice::Long gid_;
};
//-----------------------------------------------------------------------------------
class ReloadOneGroupTask: public Task {
public:
  ReloadOneGroupTask(Ice::Long gid) :
    gid_(gid) {
  }
  virtual void handle();
private:
  Ice::Long gid_;
};

class ScanTimer: public Timer {
public:
  ScanTimer() :
    Timer(30 * 1000) {
    ;
  }
  virtual void handle();
  //  time_t price_time_;
  //  time_t bid_time_;
  //  time_t crtv_time_;
};

//-----------------------------------------------------------------------------------
class NextGroup: public IceUtil::Shared {
public:
  Ice::Long gid;
  int price;
  int quality;
  SubGroupPtr sub_group;
};

typedef IceUtil::Handle<NextGroup> NextGroupPtr;
//-----------------------------------------------------------------------------------
}
}

#endif
