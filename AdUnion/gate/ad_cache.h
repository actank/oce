#ifndef _AD_UNION_AD_CACHE_H_
#define _AD_UNION_AD_CACHE_H_

#include <ctemplate/template.h>
#include <string>
#include <IceUtil/RWRecMutex.h>
#include "LogWrapper.h"
#include "QueryRunner.h"
#include "ObjectCache.h"
#include "TaskManager.h"

using namespace  MyUtil;

namespace xce {
namespace ad {

struct UnionAd : virtual public Ice::Object {
  Ice::Long id;
  Ice::Int type;
  Ice::Long tpl_type; // 显示模板类型
  string name;

  Ice::Long owner_id; // 投放者id
  Ice::Long group_id;
  Ice::Long campaign_id;

  bool valid;   // 是否生效

  Ice::Int budget; // 广告总的预算
  Ice::Int daily_budget; // 广告每天的预算
  Ice::Int click_price;
  Ice::Int click_count;

  string title;
  string content;
  string img_url;
  string dest_url;

  mutable IceUtil::RWRecMutex mutex_;
};

typedef IceUtil::Handle<UnionAd> UnionAdPtr;

//class LoadAdsResultHandler;
//
// 某个广告模板的所有广告创意信息
class AdTpl : virtual public Ice::Object {
public:
  AdTpl(Ice::Int tpl_type);

  static string GetTplNameByType(Ice::Int tpl_type);
  void UpdateNuomiCreative(Ice::Long id, const string& title, const string& content, const string& img_url, const string& dest_url);

  void GetAd(Ice::Int ad_type, Ice::Int slot, int ad_count, const string & dest_appendix, string * html, 
      Ice::Int zone_id, Ice::Int site_id, const string & ip, const string & http_referer);

  bool UpdateNuomiAd(Ice::Long id, const string & title, const string & content, const string & img_url, const string & dest_url);

  void ClickAd(Ice::Long ad_id, Ice::Int zone_id, Ice::Int site_id, const string & ip, bool cheat);

  void SuspendAd(UnionAdPtr ad);

  enum { TPL_TYPE_TEXT = 1, TPL_TYPE_IMAGE, TPL_TYPE_RICH, TPL_TYPE_RENREN_SEARCH };
  static Ice::Int MatchTplType(UnionAdPtr ad);
  bool ReloadTemplate();

protected:
  bool BuildSubDict(Ice::Long ad_id, const string & appendix, ctemplate::TemplateDictionary* dict, 
    Ice::Int zone_id, Ice::Int site_id, const string & ip, const string & http_referer) const;

  Ice::Int tpl_type_;

  typedef map<Ice::Int, vector<Ice::Long> > AdTypeIndex; // 以广告type为key, value 是 ad id 数组
  AdTypeIndex ad_type_index_;
  mutable IceUtil::RWRecMutex index_mutex_;

  typedef map<Ice::Long, UnionAdPtr> AdCache; // 以广告创意id为key
  AdCache ad_cache_;

  vector<Ice::Long> default_ads_;
  vector<Ice::Long> default_nuomi_ads_;

  ctemplate::Template * template_;

  time_t load_time_;
  friend class LoadAdsResultHandler;
};
typedef IceUtil::Handle<AdTpl> AdTplPtr;
typedef map<Ice::Int, AdTplPtr> AdTplMap;

class UnionAdCache : public MyUtil::Singleton<UnionAdCache> {
public:
  UnionAdCache();
  ~UnionAdCache();
  bool Reload();
  void GetAd(Ice::Int ad_type, Ice::Int tpl_type, Ice::Int slot, int ad_count, const string & dest_appendix, string * html, 
      Ice::Int zone_id, Ice::Int site_id, const string & ip, const string & http_referer);

  void ClickAd(Ice::Long ad_id, Ice::Int tpl_type, Ice::Int zone_id, Ice::Int site_id, const string & ip, bool cheat);
  bool UpdateNuomiAd(Ice::Long id, Ice::Int tpl_type, const string & title, const string & content, const string & img_url, const string & dest_url);

  void UpdateNuomiCreative(Ice::Long id, const string& title, const string& content, const string& img_url, const string& dest_url);
private:
  AdTplPtr GetAdTpl(Ice::Int tpl_type);

  AdTplMap ad_tpl_map_;

  mutable IceUtil::RWRecMutex mutex_;
};

class LoadAdsResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
  LoadAdsResultHandler(const MyUtil::ObjectResultPtr & results) : results_(results) {}
  virtual bool handle(mysqlpp::Row & row) const;
  void Swap(AdTplMap & ad_tpl_map);
private:
  MyUtil::ObjectResultPtr results_;
  mutable AdTplMap ad_tpl_map_;
};


class ClickSyncTask : public MyUtil::Task {
public:
  ClickSyncTask(Ice::Long ad_id, Ice::Int click_count, Ice::Int task_level)
    : Task(task_level)
    , ad_id_(ad_id)
    , click_count_(click_count) {
  }
  virtual void handle();
private:
  Ice::Long ad_id_;
  Ice::Int click_count_;
};

class ReloadNuomiCreativeTask: public MyUtil::Timer{
public:
  ReloadNuomiCreativeTask():Timer(5000),creative_update_time_(0), sale_count_(0){};
  virtual void handle();
private:
  time_t creative_update_time_;
  Ice::Int sale_count_;
};


}
}

#endif // _AD_UNION_AD_CACHE_H_

