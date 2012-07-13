#ifndef __PAGEFANS_CACHE_ADAPTER_H__
#define __PAGEFANS_CACHE_ADAPTER_H__

#include <IceUtil/RWRecMutex.h>
#include <PageFans.h>
#include <AdapterI.h>

namespace xce {
namespace buddy {

class PageFansCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<PageFansManagerPrx>, public MyUtil::Singleton<PageFansCacheAdapter> {
public:
  PageFansCacheAdapter();
  
  int addFan(PageEntryPtr pe);
  int removeFan(int pageId, int userid);
  int updateFanWithState(PageEntryPtr pe);
  int updateFansWithState(int pageId, int oriState, int tarState);
 
  int addBlock(int pageId, int userId, int operatorId, int timestamp);
  int removeBlock(int pageId, int userId);
  
  void reload(const ::MyUtil::ObjectResultPtr& data );
};

}
}
#endif
