#ifndef ADINVIDX_H_
#define ADINVIDX_H_


#include "Util.h"
#include "LogWrapper.h"
#include "AdUtil.h"
#include <map>
#include <cmath>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include <IceUtil/RWRecMutex.h>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <bits/stl_multimap.h>

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace std;
using namespace xce::ad;
using namespace boost;

class Creative {
public:
  Creative() :
    id(0),update_time(0) {
  }
  ;
  Ice::Long id;
  time_t update_time;
};
//-----------------------------------------------------------------------
class CreativeSet: public IceUtil::Shared {
public:
  map<Ice::Long, Creative> set_;
};
//-----------------------------------------------------------------------

typedef IceUtil::Handle<CreativeSet> CreativeSetPtr;

class AdGroup: public IceUtil::Shared {
public:
  AdGroup() {
    creatives_ = new CreativeSet;
  }
  Ice::Long member_id_;
  Ice::Long campaign_id_;
  Ice::Long group_id_;
  Ice::Long bid_id_;
  Ice::Long adzone_id_;

  int price_;

  CreativeSetPtr creatives_;

  BidUnitPtr bid_unit_;
};
typedef IceUtil::Handle<AdGroup> AdGroupPtr;
//-----------------------------------------------------------------------
template<class KEY>
class AdInvIdx{
public:
  void Add(KEY key,Ice::Long gid);
  void Add(const vector<KEY> & keys,Ice::Long gid);
  void Filter(KEY key,set<Ice::Long> & gids);
  void Filter(const set<KEY> & keys, set<Ice::Long> & gids);
  set<Ice::Long>  Get(KEY key);
private:
  IceUtil::RWRecMutex mutex_;
  map<KEY,set<Ice::Long> > map_;
};

//-----------------------------------------------------------------------
//轮播组
class RoGroup:public IceUtil::Shared{
public:
  AdGroupPtr Get(Params params);
private:
  IceUtil::RWRecMutex mutex_;
  map<Ice::Long,AdGroupPtr> groups_;
};
typedef IceUtil::Handle<RoGroup> RoGroupPtr;
//-----------------------------------------------------------------------
//轮播队列
class RoArray{
public:
  RoGroupPtr Get(int index);
private:
  IceUtil::RWRecMutex mutex_;
  vector<RoGroupPtr> array_;
};
//------------------------------------------------------------------------
class Matcher: public Singleton<Matcher> {
public:
  void Load();
  void GetAds(const string & pro_str);
private:
  void FilterByIdx();
  AdInvIdx<int> age_idx,stage_idx,gender_idx,grade_idx;
  AdInvIdx<Ice::Long> area_idx,school_idx;

};

};
};


#endif /* ADINVIDX_H_ */
