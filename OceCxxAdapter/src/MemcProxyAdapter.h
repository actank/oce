#ifndef _MC_PROXY_PROXY_ADAPTER_H_
#define _MC_PROXY_PROXY_ADAPTER_H_

#include <boost/lexical_cast.hpp>

#include "AdapterI.h"
#include "MemcProxy.h"
#include "Singleton.h"


namespace xce {
namespace feed {

using namespace MyUtil;

class MemcProxyAdapter : public AdapterI,
                   public AdapterISingleton<MyUtil::McProxyChannel, MemcProxyAdapter>
{
protected:
  virtual string name() {
    return "MemcProxy";
  }
  virtual string endpoints() {
    return "@MemcProxy";
  }
  virtual size_t cluster() {
    return 10;
  }

public:
  MemcProxyAdapter() {
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
    _managers.resize(cluster() > 0 ? cluster() : 1);
  }

  string GetEndpoint(int index) {
    return getManager(index)->GetEndpoint();
  }

private:
  MemcProxyPrx getManagerOneway(int id){
    return locate<MemcProxyPrx>(_managersOneway, "M", id, ONE_WAY);
  }

  MemcProxyPrx getManager(int id){
    return locate<MemcProxyPrx>(_managers, "M", id, TWO_WAY,160);
  }

  vector<MemcProxyPrx> _managersOneway;
  vector<MemcProxyPrx> _managers;
};

}
}

#endif // _MC_PROXY_PROXY_ADAPTER_H_
