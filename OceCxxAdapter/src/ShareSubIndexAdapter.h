#include "Singleton.h"
#include "AdapterI.h"

#include <ContentSearchRmi.h>


class ShareSubIndexAdapter : public MyUtil::ReplicatedAdapterI,
  public MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, ShareSubIndexAdapter>
{

    friend class MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, ShareSubIndexAdapter>;

    typedef com::xiaonei::search5::logicserver::RmiLogicPrx Prx;
  public:
    virtual ~ShareSubIndexAdapter();
    bool update(int mod, com::xiaonei::search5::logicserver::IndexUpdateDataSeq shareInfos);

  protected:
    ShareSubIndexAdapter(int timeout = 1000);

    virtual string name() {
      return "ShareUpdateLogicRmi01";
    }
    virtual string endpoints() {
      return "@ShareUpdateLogicRmi01.ShareUpdateLogicRmi01.ShareUpdateLogicRmi01";
    }
    virtual size_t cluster() {
      return 1;
    }
    bool isValid(const Ice::ObjectPrx& proxy);
    Prx getManagerOneway(int id);
    
    std::vector<com::xiaonei::search5::logicserver::RmiLogicPrx> managers_;
    std::vector<Prx> managers_oneway_;

    int timeout_;
};

