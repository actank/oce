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
#include <ext/hash_map>
#include <ext/hash_set>
namespace xce
{
namespace ad
{
using namespace std;
using namespace MyUtil;
using namespace IceUtil;
using namespace __gnu_cxx;

class DimIndex:public IceUtil::Shared{
public:
	virtual ~DimIndex(){};
  virtual set<AdGroupPtr> GetMatchedGroup(const uint64_t& key) = 0;
  void Add(const uint64_t& key, const AdGroupPtr& group, bool first = false);
  void Add(const vector<uint64_t> & key ,const AdGroupPtr & group,bool first = false);
  void Remove(const uint64_t& key, const AdGroupPtr& group);
  void FilterInactive(set<AdGroupPtr> & groups);
  static set<AdGroupPtr> MergeGroupSet(const set<AdGroupPtr>& g1, const set<AdGroupPtr>& g2){
    set<AdGroupPtr> merge_set;
    set<AdGroupPtr>::iterator it1, it2;
    it1 = g1.begin();
    it2 = g2.begin();

    while( it1 != g1.end() && it2 != g2.end()){
      if(*it1 < *it2){
        ++it1;
      }else if(*it1 > *it2){
        ++it2;
      }else{
        merge_set.insert(*it1);
        ++it1;
        ++it2;
      }
    }
    return merge_set;
  }
protected:
  set<AdGroupPtr> Get(const uint64_t& key );
  map<uint64_t,set<AdGroupPtr> > index_set_;
  IceUtil::RWRecMutex mutex_;
};
typedef IceUtil::Handle<DimIndex> DimIndexPtr;

class MultiDimIndex : public DimIndex{
public:
  virtual set<AdGroupPtr> GetMatchedGroup(const uint64_t& key);
};

class AreaIndex : public DimIndex{
public:
  virtual set<AdGroupPtr> GetMatchedGroup(const uint64_t& key);
};

class SchoolIndex : public DimIndex{
public:
  virtual set<AdGroupPtr> GetMatchedGroup(const uint64_t& key);
};



class DimIndexMatcher : public MyUtil::Singleton<DimIndexMatcher>{
public:
  void Load(map<Ice::Long, AdGroupPtr> & groups);
  set<AdGroupPtr> Get(Ice::Long zone, const string & pro_str);
private:
  User MakeUser(UserProfile & profile);
  class DimIndexGroup{
	public:
		DimIndexGroup(){
			multi_dim_index = new MultiDimIndex();	
			area_index = new AreaIndex();
			school_index = new SchoolIndex();
		}
    DimIndexPtr multi_dim_index;
    DimIndexPtr     area_index;
    DimIndexPtr   school_index;
  };

  map<Ice::Long,DimIndexGroup> zone_index_;
  IceUtil::RWRecMutex mutex_;


};

class AreaUtil : public MyUtil::Singleton<AreaUtil>{
public:

  uint64_t GetProvince(uint64_t area);
  uint64_t GetCity(uint64_t area);
  bool IsArea(uint64_t area_or_school);
  uint64_t GetChina();

//  const uint64_t AreaUtil::kChina = 86000000000000;
};




}
}


#endif /* ADINVERTEDINDEX_H_ */
