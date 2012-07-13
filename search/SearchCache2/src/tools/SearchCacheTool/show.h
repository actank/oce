#ifndef _SHOW_H_
#define _SHOW_H_
#include <IceUtil/IceUtil.h>
#include <SearchCache.h>
#include "search/SearchCache2/src/Util/SearchCacheData.h"
#include "search/SearchCache2/src/DistSearchCacheLogic/DbOperation.h"
//#include "search/SearchCache2/src/DistSearchCacheLogic/DistSearchCacheAdapter.h"
#include "OceCxxAdapter/src/DistSearchCacheAdapter.h"
#include "search/SearchCache2/src/Util/SearchMemCacheData.h"
using namespace xce::searchcache;
using namespace xce::searchcache::dboperation;
bool ShowSearchMemCacheData(const SearchMemCacheData& s);
bool ShowSearchCacheData(const SearchCacheData& s);
bool ShowSearchCacheResult(const map<long, SearchCacheMemDataPtr>& map);
bool ShowSearchCacheResult(const Int2ByteSeq& map);

#endif

