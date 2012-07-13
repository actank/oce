
/*
 * FeedExtInfoCacheI.h
 *
 *  Created on: 2010-10-20
 *      Author: ld
 */
#ifndef __FEED_EXT_INFO_CACHEI_H__
#define __FEED_EXT_INFO_CACHEI_H__


#include <string>
#include <map>
#include <boost/lexical_cast.hpp>
#include <Ice/Ice.h>
#include "IceUtil/RWRecMutex.h"
#include <IceUtil/IceUtil.h>
#include "ServiceI.h"
#include "Evictor.h"
#include "TaskManager.h"
#include "QueryRunner.h"
#include "IceLogger.h"
#include "Singleton.h"
#include "FeedExtInfoCache.h"


namespace xce {
namespace feed {
namespace count {

const int kDefaultEvictorSize = 1000000;
const unsigned short kMaxCountPerFeed = 10;

class EvictorObj;
typedef IceUtil::Handle<EvictorObj> EvictorObjPtr;

class EvictorObj: public IceUtil::Shared {

public:

  EvictorObj(::Ice::Int actor, ::Ice::Long count, ::Ice::Byte index)
      :actor_(actor), count_(count), index_(index), next_(NULL) {}
 
  void set_actor(::Ice::Int actor) {
    actor_ = actor;
  }
 
  void set_count(::Ice::Long count) {
    count_ = count;
  }

  void set_index(::Ice::Byte index) {
    index_ = index;
  }

  void set_next (EvictorObjPtr next) {
    next_ = next;
  }

  ::Ice::Int actor() {
    return actor_;
  }

  ::Ice::Int count() { 
    return count_;
  }

  ::Ice::Byte index() {
    return index_;
  }

  EvictorObjPtr next() {
    return next_;
  }
  

  ~EvictorObj() {
      next_ = NULL;
  }

private:
  ::Ice::Int actor_;
  ::Ice::Int count_;
  ::Ice::Byte index_;
  EvictorObjPtr next_;

};

class DbItem {
public:
  DbItem(::Ice::Long source, ::Ice::Int stype, ::Ice::Int actor, ::Ice::Byte index, ::Ice::Int count)
      : source_(source), stype_(stype), actor_(actor), count_map_() {

    count_map_[index] = count;
        
  }
  
  DbItem(::Ice::Long source = 0 , ::Ice::Int stype = 0, ::Ice::Int actor = 0)
      : source_(source), stype_(stype), actor_(actor), count_map_() {
        
  }


  /*DbItem(::Ice::Long source, ::Ice::Int stype, ::Ice::Int actor, const std::vector<std::string>& count_vec)
      : source_(source), stype_(stype), actor_(actor) {
    
    std::vector<Ice::Long>::size_type vec_size = count_vec.size(); 
    for (std::vector<Ice::Long>::size_type i = 0; i < vec_size; i++) {
      ::Ice::Byte index = i;
      ::Ice::Long n = boost::lexical_cast< ::Ice::Long >(count_vec[i]);      
      count_map_.insert(make_pair(index, n));
    }
  
  } */
 


  ::Ice::Int actor() const {
    return actor_;
  }

  std::map< ::Ice::Byte, ::Ice::Int >  count_map() const {
    return count_map_;
  }

  ::Ice::Long source() const {
    return source_;
  }

  ::Ice::Int stype() const{
    return stype_;
  }

  void SetCount(::Ice::Byte index, ::Ice::Int count) {
    count_map_[index] = count;
  }

private:
  ::Ice::Long source_;
  ::Ice::Int actor_;
  ::Ice::Int stype_;
  std::map< ::Ice::Byte, ::Ice::Int > count_map_;
 
};

class DirtyCache {

public:
  typedef std::map<Ice::Long, std::vector<DbItem> > DbItemMap; // 索引 source
  void Update(const DbItem& new_item);
  void Clear(); 

  void pf();
    
private:

  void SaveDb(short db_table_no, const DbItemMap& db_item_map);


private:
  std::map< short,  DbItemMap > dirty_map_;  // 索引为 source % DbHelper::kTableCount
  IceUtil::Mutex dirty_mutex_;
};


class ClearDirtyCacheTimer: public MyUtil::Timer, public MyUtil::Singleton<ClearDirtyCacheTimer> {
public:
  ClearDirtyCacheTimer(): Timer(3 * 1000) {}
  virtual void handle();

};

class FeedExtInfoCacheI: public FeedExtInfoCache, public MyUtil::Singleton<FeedExtInfoCacheI> {

public:
  
  typedef MyUtil::SizedEvictor< ::Ice::Long, EvictorObjPtr > CountEvictor;  

  virtual void SetCount(::Ice::Long, ::Ice::Int, ::Ice::Int, ::Ice::Byte, ::Ice::Int, const ::Ice::Current& = ::Ice::Current());

  virtual ::Ice::Int GetCount(::Ice::Long, ::Ice::Int, ::Ice::Int, ::Ice::Byte, const ::Ice::Current& = ::Ice::Current());


  virtual ::Ice::Long GetEvictorSize(::Ice::Int, const ::Ice::Current& = ::Ice::Current());

  virtual bool SetEvictorSize(::Ice::Int, ::Ice::Int, const ::Ice::Current& = ::Ice::Current());

  void ClearDirtyCache();
  
 
  ~FeedExtInfoCacheI() {
    for (std::map< ::Ice::Int, CountEvictor* >::iterator it = evictor_map_.begin();
        it != evictor_map_.end(); it++) {
      delete it->second;
      it->second = NULL;
    }

    for (std::map< ::Ice::Int, IceUtil::RWRecMutex*>::iterator it = rwmutex_map_.begin();
        it != rwmutex_map_.end(); it++) {
      delete it->second;
      it->second = NULL;
    } 
 
  }

private:
  pair<bool, DbItem> QueryDb(Ice::Long source, Ice::Int stype, Ice::Int actor);
  ::Ice::Int QueryDbAndUpdateEvictor(::Ice::Long source, ::Ice::Int stype, ::Ice::Int actor, ::Ice::Byte index);
  void SetCount(DbItem& item); 
private:
  std::map< ::Ice::Int, CountEvictor* > evictor_map_;
  IceUtil::Mutex evictor_map_mutex_;  
  std::map< ::Ice::Int, IceUtil::RWRecMutex* > rwmutex_map_;
  DirtyCache dirty_cache_;

};


}
}
}

#endif

