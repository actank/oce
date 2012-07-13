#ifndef AD_MEMORY_ADAPTER_H_
#define AD_MEMORY_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "AdMemory.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdMemoryAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::AdChannel, AdMemoryAdapter> {
public:
  AdMemoryAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  MemoryInfoMap GetMemoryWithWeight(int uid) {
    return getManager(0)->GetMemoryWithWeight(uid);
  }

  void PvBatch(int uid, MyUtil::LongSeq groups) {
    getManagerOneway(0)->PvBatch(uid, groups);
  }
  void Click(int uid, long group) {
    getManagerOneway(0)->Click(uid, group);
  }
  void Reload() {
    getManagerOneway(0)->Reload();
  }

private:
  AdMemoryPrx getManager(int id);
  AdMemoryPrx getManagerOneway(int id);

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@AdMemory";
  }
  virtual size_t cluster() {
    return 1; 
  }

  vector<AdMemoryPrx> _managersOneway;
  vector<AdMemoryPrx> _managers;
};

}
}


#endif // AD_MEMORY_ADAPTER_H_
