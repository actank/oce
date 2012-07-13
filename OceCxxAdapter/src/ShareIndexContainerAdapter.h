#include "Singleton.h"
#include "AdapterI.h"

#include <ShareSearch.h>
#include <ShareServer.h>


class ShareIndexContainerAdapter : public MyUtil::ReplicatedAdapterI,
  public MyUtil::AdapterISingleton<MyUtil::ShareSearchChannel, ShareIndexContainerAdapter>
{

    friend class MyUtil::AdapterISingleton<MyUtil::ShareSearchChannel, ShareIndexContainerAdapter>;

    typedef com::xiaonei::sharesearch::server::ShareIndexContainerPrx Prx;
  public:
    virtual ~ShareIndexContainerAdapter();
    bool update(xce::sharesearch::ShareIndexInfoSeq shareInfos);

  protected:
    ShareIndexContainerAdapter(int timeout = 2000);

    virtual string name() {
      return "ShareIndexContainer_SubA";
    }
    virtual string endpoints() {
      return "@ShareIndexContainer_SubA.ShareIndexContainer_SubA.ShareIndexContainer_SubA";
    }
    virtual size_t cluster() {
      return 1;
    }
    void setName(string name)  {
        _name = name;

    }
    void setEndPoints(string endpoint) {
        _endpoints = endpoint;
    }
    bool isValid(const Ice::ObjectPrx& proxy);
    Prx getManagerOneway(int id);
    
    std::vector<com::xiaonei::sharesearch::server::ShareIndexContainerPrx> managers_;
    std::vector<Prx> managers_oneway_;

    int timeout_;
};

