#ifndef _FRIENDOFFRIENDSEARCH_ADAPTER_PAGESEARCHMANAGERADAPTER_H
#define _FRIENDOFFRIENDSEARCH_ADAPTER_PAGESEARCHMANAGERADAPTER_H

#include "common.h"

#include "Singleton.h"
#include "AdapterI.h"

#include <SearchModel.h>
#include <SearchServer.h>

NAMESPACE_HEADER

class PageSearchManagerAdapter : public MyUtil::ReplicatedAdapterI,
  public MyUtil::AdapterISingleton<MyUtil::SearchChannel, PageSearchManagerAdapter>
{
  friend class MyUtil::AdapterISingleton<MyUtil::SearchChannel, PageSearchManagerAdapter>;

  typedef com::xiaonei::search2::server::IndexManagerPrx Prx;
  typedef com::xiaonei::search2::model::IndexResultPtr ResultPtr;
  typedef com::xiaonei::search2::model::IndexCondition Condition;

  public:
    virtual ~PageSearchManagerAdapter();

    ResultPtr SearchIndex(const Condition &condition, int begin, int limit);

  protected:
    PageSearchManagerAdapter(int timeout = 2000);

    virtual string name() {
      return "PAGEIM";
    }
    virtual string endpoints() {
      return "@SearchManager";
    }
    virtual size_t cluster() {
      return 0;
    }
    bool isValid(const Ice::ObjectPrx& proxy);
    Prx getManagerOneway(int id);
    
    std::vector<Prx> managers_;
    std::vector<Prx> managers_oneway_;

    int timeout_;
};


NAMESPACE_FOOTER

#endif 
