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
#include "../DistSearchCacheLogic/DbOperation.h"
namespace xce
{
  namespace dist
  {
    namespace searchcache
    {

      using namespace MyUtil;
      using namespace std;
      using namespace IceUtil;
      using namespace xce::searchcache::dboperation;
      using namespace com::xiaonei::xce;
      using namespace xce::distcache;


      class DistSearchCacheAdapter : public MyUtil::Singleton<DistSearchCacheAdapter> {
        public:
          void initialize(); 
          void logOut(const string& funStr, const SearchCacheMemDataPtr data, bool isDebug = true );
          bool setData(long userId, const SearchCacheMemDataPtr& res); 					
          bool setData(long userId, const string& res);
          vector<long> setData(const map<long, SearchCacheMemDataPtr>& res); 					
          SearchCacheMemDataPtr getData(long id);
          map<long, SearchCacheMemDataPtr> getData(vector<long> ids);	

        private:
          xce::distcache::DistCacheAdapterPtr distCacheAdapterPtr;
      };
    }
  }
}
#endif

