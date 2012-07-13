#include "Singleton.h"
#include "AdapterI.h"

#include <ShareSearch.h>
#include <ShareServer.h>


class ShareIndexContainerSubBAdapter : public MyUtil::ReplicatedAdapterI,
  public MyUtil::AdapterISingleton<MyUtil::ShareSearchChannel, ShareIndexContainerSubBAdapter>
{

    friend class MyUtil::AdapterISingleton<MyUtil::ShareSearchChannel, ShareIndexContainerSubBAdapter>;

    typedef com::xiaonei::sharesearch::server::ShareIndexContainerPrx Prx;
  public:
    virtual ~ShareIndexContainerSubBAdapter();
    bool update(xce::sharesearch::ShareIndexInfoSeq shareInfos);

  protected:
    ShareIndexContainerSubBAdapter(int timeout = 2000);

    virtual string name() {
      return "ShareIndexContainer_SubB";
    }
    virtual string endpoints() {
      return "@ShareIndexContainer_SubB.ShareIndexContainer_SubB.ShareIndexContainer_SubB";
    }
    virtual size_t cluster() {
      return 1;
    }
    bool isValid(const Ice::ObjectPrx& proxy);
    Prx getManagerOneway(int id);
    
    std::vector<com::xiaonei::sharesearch::server::ShareIndexContainerPrx> managers_;
    std::vector<Prx> managers_oneway_;

    int timeout_;
};

