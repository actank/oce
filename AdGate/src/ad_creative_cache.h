#ifndef _AD_CREATIVE_CACHE_H_
#define _AD_CREATIVE_CACHE_H_

#include <ctemplate/template.h>
#include <string>
#include <IceUtil/RWRecMutex.h>
#include "LogWrapper.h"
#include "TaskManager.h"
#include "AdMatch.h"

namespace xce {
namespace ad {

using namespace  MyUtil;

struct AdCreative {
  long id;

  long zone_id;
  long group_id;
  long widget_id;
  int type;
  time_t update_time;
  string name;

  string title;
  string content;
  string img_url;
  string dest_url;
};

class AdCreativeCache : public MyUtil::Singleton<AdCreativeCache> {
public:
  AdCreativeCache();
  ~AdCreativeCache();
  bool Reload();
  bool ReloadById(long id);

  void PushDefault(int uid, const string & ip, const string & referer, AdInfoSeq * ads, int count);
  void Get(const AdInfoSeq & ads, string * html) const;
  void GetJson(const AdInfoSeq & ads, string * html) const;
private:
  mutable IceUtil::RWRecMutex mutex_;

  typedef map<long, AdCreative> AdCache; // 以广告创意id为key
  AdCache ad_cache_;

  vector<long> default_ads_; // 默认广告
  mutable size_t default_index_;

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

