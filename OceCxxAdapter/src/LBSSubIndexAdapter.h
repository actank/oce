#include "Singleton.h"
#include "AdapterI.h"

#include <ContentSearchRmi.h>


class LBSSubIndexAdapter : public MyUtil::ReplicatedAdapterI,
  public MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, LBSSubIndexAdapter>
{
    friend class MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, LBSSubIndexAdapter>;
    typedef com::xiaonei::search5::logicserver::RmiLogicPrx Prx;
  public:
    virtual ~LBSSubIndexAdapter();
    bool update(int mod, com::xiaonei::search5::logicserver::IndexUpdateDataSeq blogInfos);

    LBSSubIndexAdapter(int timeout = 1000);
  protected:

    virtual string name() {
      return "LBSUpdateLogicRmi01";
    }
    virtual string endpoints() {
     // return "default -h 10.22.200.56";
      return "@LBSUpdateLogicRmi01.LBSUpdateLogicRmi01.LBSUpdateLogicRmi01";
    }
    virtual size_t cluster() {
      return 0;
    }
    bool isValid(const Ice::ObjectPrx& proxy);
    Prx getManagerOneway(int id);
    
    std::vector<com::xiaonei::search5::logicserver::RmiLogicPrx> managers_;
    std::vector<Prx> managers_oneway_;

    int timeout_;
};

