#ifndef __ADLOGS_ADAPTER_H__
#define __ADLOGS_ADAPTER_H__

#include "AdapterI.h"
#include "AdLog.h"

namespace xce{
namespace ad{
  using namespace MyUtil;
  class AdLogAdapter : public AdapterI, public AdapterISingleton<MyUtil::AdChannel, AdLogAdapter>{
  public:
    AdLogAdapter():index_(0){}
    void PvBatch(const ::MyUtil::StrSeq& logs);

    void ClickForBp(const string& log);
    void ClickForCharge(const string& log);
    void PvForCharge(const ::MyUtil::StrSeq& logs);

    int getCluster(){
      return _cluster;
    }
  protected:
    virtual string name(){
      return "AdLogS";
    }
    virtual string endpoints(){
      return "@AdLogS";
    }
    virtual size_t cluster(){
      return 4;
    }
    AdLogManagerPrx getManagerOneway(int);
    AdLogManagerPrx getManager(int);
  private:
    unsigned int index_;
    vector<AdLogManagerPrx> oneway_proxy_;
    vector<AdLogManagerPrx> twoway_proxy_;
  };
};
};
#endif
