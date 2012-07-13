#include "AdInvIdx.h"
#include <algorithm> 

using namespace xce::ad;
using namespace std;
using namespace xce::ad;
using namespace boost;

template<typename KEY>
void AdInvIdx<KEY>::Add(KEY key, Ice::Long gid) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  map_[key].insert(gid);
}

template<typename KEY>
void AdInvIdx<KEY>::Add(const vector<KEY> & keys, Ice::Long gid) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  for (typename vector<KEY>::const_iterator vit = keys.begin(); vit
      != keys.end(); ++vit) {
    map_[*vit].insert(gid);
  }
}

template<typename KEY>
void AdInvIdx<KEY>::Filter(KEY key, set<Ice::Long> & gids) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  typename map<KEY, set<Ice::Long> >::iterator mit = map_.find(key);
  if (mit == map_.end()) {
    gids.clear();
  } else {
    set<Ice::Long> tmp;
    set_intersection(mit->second.begin(), mit->second.end(), gids.begin(),
        gids.end(), inserter(tmp, tmp.begin()));
    gids.swap(tmp);
  }
}

template<typename KEY>
void AdInvIdx<KEY>::Filter(const set<KEY> & keys, set<Ice::Long> & gids) {
  vector<set<Ice::Long> > g_sets;
  IceUtil::RWRecMutex::WLock lock(mutex_);
  for (typename set<KEY>::const_iterator kit = keys.begin(); kit != keys.end(); ++kit) {
    typename map<KEY, set<Ice::Long> >::iterator mit = map_.find(*keys);
    if (mit == map_.end()) {
      continue;
    } else {
      set<Ice::Long> tmp;
      set_intersection(mit->second.begin(), mit->second.end(), gids.begin(),
          gids.end(), inserter(tmp, tmp.begin()));
      g_sets.push_back(tmp);
    }
  }
  if(g_sets.empty()){
    gids.clear();
    return;
  }else if(g_sets.size() == 1){
    gids.swap(g_sets.at(0));
    return;
  }
  set<Ice::Long> result;
  result.swap(g_sets.at(0));
  for(vector<set<Ice::Long> >::iterator vit = ++(g_sets.begin());vit != g_sets.end();++vit){
    set<Ice::Long> tmp;
    set_union(result.begin(),result.end(),(*vit).begin(),(*vit).end(),inserter(tmp,tmp.begin()));
    result.swap(tmp);
  }
  gids.swap(result);
}

template<typename KEY>
set<Ice::Long> AdInvIdx<KEY>::Get(KEY key) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  typename map<KEY, set<Ice::Long> >::const_iterator sit = map_.find(key);
  if (sit == map_.end()) {
    set<Ice::Long> tmp;
    return tmp;
  } else {
    return sit->second;
  }
}
//--------------------------------------------------------------------------------------------

AdGroupPtr RoGroup::Get(Params params) {
  IceUtil::RWRecMutex mutex_;
  //TODO
}

//--------------------------------------------------------------------------------------------

RoGroupPtr RoArray::Get(int index) {
  IceUtil::RWRecMutex mutex_;
  index = index / array_.size();
  return array_.at(index);
}

void Matcher::GetAds(const string & pro_str) {
//  AdInfoSeq out;
//  UserProfile profile;
//  UserProfileClient::instance().Deserialize(pro_str, &profile);
//  int uid = profile.id();
//  if (!ready_) {
//    return out;
//  }
//  Params params;
//  Preprocess(profile, params);
//  set<Ice::Long> res = stage_idx.Get(params.stage);

}
