#ifndef _FRIENDOFFRIENDSEARCH_ADAPTER_PEOPLESEARCHMANAGERADAPTER_H
#define _FRIENDOFFRIENDSEARCH_ADAPTER_PEOPLESEARCHMANAGERADAPTER_H

#include "common.h"

#include "Singleton.h"
#include "AdapterI.h"

#include <SearchModel.h>
#include <SearchServer.h>

NAMESPACE_HEADER

class PeopleSearchManagerAdapter : public MyUtil::ReplicatedAdapterI,
  public MyUtil::AdapterISingleton<MyUtil::SearchChannel, PeopleSearchManagerAdapter>
{
  friend class MyUtil::AdapterISingleton<MyUtil::SearchChannel, PeopleSearchManagerAdapter>;

  typedef com::xiaonei::search2::server::IndexManagerPrx Prx;
  typedef com::xiaonei::search2::model::IndexResultPtr ResultPtr;
  typedef com::xiaonei::search2::model::IndexCondition Condition;

  public:
    virtual ~PeopleSearchManagerAdapter();

    ResultPtr SearchIndex(const Condition &condition, int begin, int limit);

  protected:
    PeopleSearchManagerAdapter(int timeout = 2000);

    virtual string name() {
      return "OPIM";
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

#endif // _FRIENDOFFRIENDSEARCH_ADAPTER_SEARCHMANAGERADAPTER_H
