#include "Singleton.h"
#include "AdapterI.h"

#include <ContentSearchRmi.h>


class StateSubIndexAdapter : public MyUtil::ReplicatedAdapterI,
  public MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, StateSubIndexAdapter>
{

    friend class MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, StateSubIndexAdapter>;

    typedef com::xiaonei::search5::logicserver::RmiLogicPrx Prx;
  public:
    virtual ~StateSubIndexAdapter();
    bool update(int mod, com::xiaonei::search5::logicserver::IndexUpdateDataSeq blogInfos);

  protected:
    StateSubIndexAdapter(int timeout = 1000);

    virtual string name() {
      return "StateUpdateLogicRmi01";
    }
    virtual string endpoints() {
      return "@StateUpdateLogicRmi01.StateUpdateLogicRmi01.StateUpdateLogicRmi01";
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

