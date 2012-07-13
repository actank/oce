/// 
/// @file SearchMemCacheAdapter.h
/// @brief 
/// @author zhigang.wu@opi-corp.com
/// @date 2010-04-16
/// 
#ifndef __SEARCH_MEM_CACHE_ADAPTER_H_
#define __SEARCH_MEM_CACHE_ADAPTER_H_

#include "TopicI.h"
#include "AdapterI.h"
#include "TaskManager.h"
#include <IceUtil/RWRecMutex.h>
#include <SearchCache.h>

namespace xce {
  namespace searchcache {
    class SearchMemCacheAdapter: public MyUtil::ReplicatedAdapterI,
    public MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, SearchMemCacheAdapter> {
     public:
       SearchMemCacheAdapter();
       bool doRemoveMemCacheData(const MyUtil::LongSeq& userIds);
       bool doUpdateMemCacheData(const Int2ByteSeq& id2Map);
       Int2SearchCacheResultMap getSearchCacheMap(const MyUtil::IntSeq& userIds);
       Int2ByteSeq getSearchCacheMap2(const MyUtil::IntSeq& userIds); 
       bool SetValid(const int mod, bool status);
       bool IsValid(const int mod);
     protected:
       bool isValid(const Ice::ObjectPrx& proxy);
     private:
       vector<SearchCacheManagerPrx> _managers;

     public:
       virtual string name() {
         return "SearchCache";
       }
       virtual string endpoints() {
         return "@SearchCache";
       }
       virtual size_t cluster() {
         return 4;
       }
    };

  }
}

#endif /* SEARCHCAHCEADAPTER_H_ */
