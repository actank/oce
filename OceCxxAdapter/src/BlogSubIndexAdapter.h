#include "Singleton.h"
#include "AdapterI.h"

#include <ContentSearchRmi.h>


class BlogSubIndexAdapter : public MyUtil::ReplicatedAdapterI,
  public MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, BlogSubIndexAdapter>
{

    friend class MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, BlogSubIndexAdapter>;

    typedef com::xiaonei::search5::logicserver::RmiLogicPrx Prx;
  public:
    virtual ~BlogSubIndexAdapter();
    bool update(int mod, com::xiaonei::search5::logicserver::IndexUpdateDataSeq blogInfos);

  protected:
    BlogSubIndexAdapter(int timeout = 1000);

    virtual string name() {
      return "BlogUpdateLogicRmi01";
    }
    virtual string endpoints() {
      return "@BlogUpdateLogicRmi01.BlogUpdateLogicRmi01.BlogUpdateLogicRmi01";
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

