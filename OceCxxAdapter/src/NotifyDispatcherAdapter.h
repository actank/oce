#ifndef _NOTIFY_DISPATCHER_ADAPTER_H_
#define _NOTIFY_DISPATCHER_ADAPTER_H_

#include "AdapterI.h"
#include "Notify.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "TaskManager.h"


namespace xce {
namespace notify {

using namespace MyUtil;

class NotifyDispatcherAdapter : public AdapterI,
                   public AdapterISingleton<MyUtil::NotifyChannel, NotifyDispatcherAdapter>
{

protected:
  virtual string name() {
    return "NotifyDispatcher";
  }
  virtual string endpoints() {
    return "@NotifyDispatcher";
  }
  virtual size_t cluster() {
    return 1;
  }

public:
  NotifyDispatcherAdapter(){
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }
  void dispatch(const MyUtil::Str2StrMap& props) {
    getManagerOneway(0)->dispatch(props);
  }
  void deliverWebpager(const MyUtil::Str2StrMap& props) {
    getManagerOneway(0)->deliverWebpager(props);
  }

  bool reloadSchema() {
    for(size_t i = 0; i < cluster(); ++i) {
      getManager(i)->reloadSchema();
    }
    return true;
  }
private:
  NotifyDispatcherPrx getManagerOneway(int id) {
    return locate<NotifyDispatcherPrx> (_managersOneway, "M", id, TWO_WAY);
  }
  NotifyDispatcherPrx getManager(int id) {
    return locate<NotifyDispatcherPrx> (_managers, "M", id, TWO_WAY);
  }

  vector<NotifyDispatcherPrx> _managersOneway;
  vector<NotifyDispatcherPrx> _managers;
};

}
}

#endif // _NOTIFY_DISPATCHER_ADAPTER_H_
