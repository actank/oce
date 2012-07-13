#ifndef _MC_PROXY_PROXY_ADAPTER_H_
#define _MC_PROXY_PROXY_ADAPTER_H_

#include <boost/lexical_cast.hpp>

#include "AdapterI.h"
#include "FeedMemcProxy.h"
#include "Singleton.h"


namespace xce {
namespace feed {

using namespace MyUtil;

class FeedMemcProxyAdapter : public AdapterI,
                   public AdapterISingleton<MyUtil::McProxyChannel, FeedMemcProxyAdapter>
{
protected:
  virtual string name() {
    return "FeedMemcProxy";
  }
  virtual string endpoints() {
    return "@FeedMemcProxy";
  }
  virtual size_t cluster() {
    return 20;
  }

public:
  FeedMemcProxyAdapter() {
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
    _managers.resize(cluster() > 0 ? cluster() : 1);
  }

  string GetEndpoint(int index) {
    return getManager(index)->GetEndpoint();
  }
  
  ProxyInfoPtr GetProxyInfo(int index){
    return getManager(index)->GetProxyInfo();
  }

  size_t GetClusterSize() {
    return cluster();
  }

private:
  FeedMemcProxyPrx getManagerOneway(int id){
    return locate<FeedMemcProxyPrx>(_managersOneway, "M", id, ONE_WAY);
  }

  FeedMemcProxyPrx getManager(int id){
    return locate<FeedMemcProxyPrx>(_managers, "M", id, TWO_WAY);
  }

  vector<FeedMemcProxyPrx> _managersOneway;
  vector<FeedMemcProxyPrx> _managers;
};

}
}

#endif // _MC_PROXY_PROXY_ADAPTER_H_
