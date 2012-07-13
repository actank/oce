#ifndef _AD_CREATIVE_CACHE_H_
#define _AD_CREATIVE_CACHE_H_

#include <ctemplate/template.h>
#include <string>
#include <IceUtil/RWRecMutex.h>
#include "LogWrapper.h"
#include "TaskManager.h"
#include "AdEngine.h"
#include "AdGateI.h"

namespace xce {
namespace ad {

using namespace  MyUtil;

class AdCreative : public IceUtil::Shared {
public:
  long id;
  long zone_id;
  long group_id;
  long widget_id;
  string widget;
  string widget_version;
  long member_id;
  int type;
  time_t update_time;
  string name;

  string title;
  string content;
  string img_url;
  string dest_url;
};

typedef IceUtil::Handle<AdCreative> AdCreativePtr;


class AdCreativeCache : public MyUtil::Singleton<AdCreativeCache> {
public:
  AdCreativeCache();
  ~AdCreativeCache();
  bool Reload();
  bool ReloadById(long id);

  void PushDefault(const int uid, const int stage, const string & ip, const string & referer, AdResultMap * ad_result_map) const;
  void Get(const AdResultMap & ads, string * html) const;
//  void GetJson(const string & referer, int refresh_source, int refresh_idx, const string & emtag, const AdResultMap & ads, string * html) const;

  void GetJson(const string & referer, int refresh_source, int refresh_idx,
      const map<long, EngineTypes> & self_zones, const AdResultMap & ads, string * html) const;
  void Get(const AdZone2Creatives & ads, string * html) const;
//  void GetJson(const string & referer, int refresh_source, int refresh_idx, const string & emtag, const AdResultMap & ads, string * html) const;

  void GetJson(const string & referer, int refresh_source, int refresh_idx,
      const map<long, EngineTypes> & self_zones, const AdZone2Creatives  & ads, string * html) const;
  void GetZoneDict(const set<Ice::Long>& zone_ids, map<int64_t, int16_t>& zone_count_map);
private:
  void GetZonesByReferer(const string & referer, set<long> * zones) const;

  mutable IceUtil::RWRecMutex mutex_;

  typedef map<long, AdCreativePtr> AdCache; // 以广告创意id为key
  AdCache ad_cache_;

  // 各个广告位对应的默认广告, key 为 zone id, value 为默认广告列表
  map<long, vector<long> > default_ads_;
  // 各个广告位对应的默认广告 index, key 为 zone id, value 为默认广告index
  mutable map<long, size_t> default_ad_indices_;

  multimap<string, long> url_zone_map_;
  map<long, size_t> zone_count_map_;
  map<int64_t, int16_t> zone_bp_count_map_;

  mutable IceUtil::RWRecMutex tpl_mutex_;
  ctemplate::Template * main_template_;
  ctemplate::Template * json_template_;
};

class ReloadCreativeTask : public Task {
public:
  ReloadCreativeTask(long id) : creative_id_(id) {}
  virtual void handle();
private:
  long creative_id_;
};

}
}

#endif // _AD_CREATIVE_CACHE_H_

