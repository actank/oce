/*
 * =====================================================================================
 *
 *       Filename:  DistSearchCacheAdapter.h
 *
 *    Description:  Adapter for communicating with DistCache
 *
 *        Version:  1.0
 *        Created:  2010年12月04日 17时14分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef __DIST_SEARCHCACHE_ADAPTER_H__
#define __DIST_SEARCHCACHE_ADAPTER_H__ 
#include "util/cpp/TimeCost.h"
#include <IceUtil/IceUtil.h>
#include "distcache/DistCacheAdapter.h"
#include <sys/time.h>
#include <set>
#include <map>
#include <vector> 
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "search/SearchCache2/src/DistSearchCacheLogic/DbOperation.h"
namespace xce
{
  namespace dist
  {
    namespace searchcacheadapter
    {

      using namespace MyUtil;
      using namespace std;
      using namespace IceUtil;
      using namespace xce::searchcache::dboperation;
      using namespace com::xiaonei::xce;
      using namespace xce::distcache;

      const int GROUP_SIZE = 100;
      const int OPER_INTERVAL_TIME = 2000;		

      class DistSearchCacheAdapter : public MyUtil::Singleton<DistSearchCacheAdapter> {
        public:
          void initialize(); 
          SearchCacheMemDataPtr getData(long id);
          map<long, SearchCacheMemDataPtr> getData(vector<long> ids);	
          bool setData(long userId, const SearchCacheMemDataPtr& res);
          void setData(const map<long, SearchCacheMemDataPtr>& res);

        private:
          long getTimeMicroSec(); 
          xce::distcache::DistCacheAdapterPtr distCacheAdapterPtr;
          vector<hash_map<int, long> > last_oper_time_;
          vector<multimap<long, int> > time_to_userid_;
          IceUtil::Monitor<IceUtil::Mutex> mutex_[GROUP_SIZE];
      };
    }
  }
}
#endif

