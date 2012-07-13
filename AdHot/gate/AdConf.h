#ifndef _XCE_AD_HOT_CONF_H_
#define _XCE_AD_HOT_CONF_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "Singleton.h"

using namespace std;

namespace xce {
namespace ad {

struct AdConf : public IceUtil::Shared {
  AdConf(Ice::Long id) : id_(id) {}

  Ice::Int id_;
  string pic_url_;
  string pic_src_;
  string message1_;
  string message2_;
  string join_button_;
  string join_title_;
  string not_join_title_;

  time_t start_time_;
  time_t end_time_;

  vector<pair<string, string> > related_posts_; // 相关链接, first 是title, second 是 url

  void ParseRelatedPosts(const string & src);
};
typedef IceUtil::Handle<AdConf> AdConfPtr;

class AdConfCache : public MyUtil::Singleton<AdConfCache> {
public:
  void Reload();
  AdConfPtr Get(Ice::Int ad_id) const;
  set<Ice::Int> GetAdIds() const;
private:
  typedef map<Ice::Int, AdConfPtr> AdConfMap;
  AdConfMap ad_cache_;
  set<Ice::Int> ad_ids_;
  mutable IceUtil::RWRecMutex mutex_;
}; 

}
}

#endif // _XCE_AD_HOT_CONF_H_
