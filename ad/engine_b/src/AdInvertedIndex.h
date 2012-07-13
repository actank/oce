/*
 * AdInvertedIndex.h
 *
 *  Created on: Sep 1, 2010
 *      Author: antonio
 */

#ifndef ADINVERTEDINDEX_H_
#define ADINVERTEDINDEX_H_
#include <set>
#include "Util.h"
#include <IceUtil/RWRecMutex.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include "AdStruct.h"
#include "FeedMemcProxy/client/user_profile_client.h"

namespace xce {
namespace ad {
using namespace std;
using namespace MyUtil;
using namespace IceUtil;
using namespace __gnu_cxx;

class ShortIndex : public IceUtil::Shared {
public:
  bool Add(short key, AdGroupPtr group);
  bool Add(vector<short>& keys, AdGroupPtr group);
  bool Del(vector<short>& keys, AdGroupPtr group);
  set<AdGroupPtr> Get(const short key);
  set<AdGroupPtr> GetWithAll(const short key);
  void intersection(short key, set<AdGroupPtr>& other); 

private:
  map<short, set<AdGroupPtr> > index_;
  IceUtil::RWRecMutex mutex_;
};
typedef IceUtil::Handle<ShortIndex> ShortIndexPtr;
typedef ShortIndex GenderIndex;
typedef ShortIndex AgeIndex;
typedef ShortIndex StageIndex;
typedef ShortIndex GradeIndex;

class UIntIndex : public IceUtil::Shared {
public:
  virtual bool Add(uint64_t& key, AdGroupPtr group);
  virtual bool Add(vector<uint64_t>& keys, AdGroupPtr group);
  virtual bool Del(vector<uint64_t>& keys, AdGroupPtr group);
  set<AdGroupPtr> Get(const uint64_t key);
  virtual set<AdGroupPtr> GetWithAll(const uint64_t key);
  void intersection(string& this_key, UIntIndex& other, set<AdGroupPtr>& output); 

protected:
  map<uint64_t, set<AdGroupPtr> > index_;
  IceUtil::RWRecMutex mutex_;
};
typedef IceUtil::Handle<UIntIndex> UIntIndexPtr;

class AreaUIntIndex : public UIntIndex {
public:
  virtual set<AdGroupPtr> GetWithAll(const uint64_t key);
};

class AdInvertedIndex : public MyUtil::Singleton<AdInvertedIndex> {
public:
  AdInvertedIndex() : load_all_(true) {
    TaskManager::instance().scheduleRepeated(new ChangeFlagTimer());
  };
  bool Get(const UserProfile & profile, const Ice::Long zone, set<AdGroupPtr>& groups);
  bool Load(hash_map<Ice::Long, AdGroupPtr>& groups);
  bool TotalLoad(hash_map<Ice::Long, AdGroupPtr>& groups);
  bool IncLoad(hash_map<Ice::Long, AdGroupPtr>& groups);
  void set_load_all(bool load_all) {
    load_all_ = load_all;
  }

private:
  struct DimIndex : public IceUtil::Shared {
    DimIndex() {
      gender_index_ = new ShortIndex();
      stage_index_ = new ShortIndex();
      grade_index_ = new ShortIndex();
      age_index_ = new ShortIndex();
      area_index_ = new AreaUIntIndex();
      school_index_ = new UIntIndex();
    };
    ShortIndexPtr gender_index_;
    ShortIndexPtr stage_index_;
    ShortIndexPtr grade_index_;
    ShortIndexPtr age_index_;
    UIntIndexPtr area_index_;
    UIntIndexPtr school_index_;
  };
  typedef IceUtil::Handle<DimIndex> DimIndexPtr;
  class ChangeFlagTimer : virtual public MyUtil::Timer{
    public:
      ChangeFlagTimer() : Timer(24 * 60 * 60 * 1000) {//一天
      }
      virtual void handle() {
        AdInvertedIndex::instance().set_load_all(true);
      };
  };

  long last_update_time_;
  bool load_all_;
  map<Ice::Long, AdGroupPtr> cached_group_;
  map<Ice::Long, DimIndexPtr> zone_index_;
  IceUtil::RWRecMutex mutex_;
};

}
}

#endif /* ADINVERTEDINDEX_H_ */
