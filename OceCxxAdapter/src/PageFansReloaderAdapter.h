#ifndef __PAGEFANS_RELOADER_ADAPTER_H__
#define __PAGEFANS_RELOADER_ADAPTER_H__

#include <IceUtil/RWRecMutex.h>
#include <PageFans.h>
#include <AdapterI.h>

namespace xce {
namespace buddy {

class PageFansReloaderAdapter : public MyUtil::ReplicatedClusterAdapterI<PageFansReloaderPrx>, public MyUtil::Singleton<PageFansReloaderAdapter> {
public:
  PageFansReloaderAdapter();
  
  void reload(int id);
};

}
}
#endif
