#ifndef _FRIENDOFFRIENDSEARCH_ADAPTER_EXTERNALSEARCHMANAGERADAPTER_H
#define _FRIENDOFFRIENDSEARCH_ADAPTER_EXTERNALSEARCHMANAGERADAPTER_H

#include "common.h"

#include "Singleton.h"
#include "AdapterI.h"

#include <SearchModel.h>
#include <SearchServer.h>

NAMESPACE_HEADER

class ExternalSearchManagerAdapter : public MyUtil::ReplicatedAdapterI,
  public MyUtil::AdapterISingleton<MyUtil::SearchChannel, ExternalSearchManagerAdapter>
{
  friend class MyUtil::AdapterISingleton<MyUtil::SearchChannel, ExternalSearchManagerAdapter>;

  typedef com::xiaonei::search2::server::IndexManagerPrx Prx;
  typedef com::xiaonei::search2::model::IndexResultPtr ResultPtr;
  typedef com::xiaonei::search2::model::IndexCondition Condition;

  public:
    virtual ~ExternalSearchManagerAdapter();

    ResultPtr SearchIndex(const Condition &condition, int begin, int limit);

  protected:
    ExternalSearchManagerAdapter(int timeout = 2000);

    virtual string name() {
      return "ESIM";
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
