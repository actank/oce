#include "search/SearchCache2/src/Util/SearchCacheData.h"

namespace xce {
namespace searchcache {

class SearchCacheData;

class SearchCacheUtil: public MyUtil::Singleton<SearchCacheUtil> {
 public:
   SearchCacheUtil() { }
   bool ShowSearchCacheData(SearchCacheDataPtr& obj);
};

}
}
