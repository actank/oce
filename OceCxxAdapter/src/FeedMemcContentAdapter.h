#ifndef _MC_PROXY_CONTENT_ADAPTER_H_
#define _MC_PROXY_CONTENT_ADAPTER_H_

#include <boost/lexical_cast.hpp>

#include "AdapterI.h"
#include "FeedMemcProxy.h"
#include "Singleton.h"


namespace xce {
namespace feed {

using namespace MyUtil;

class FeedMemcContentAdapter : public AdapterI,
                   public AdapterISingleton<MyUtil::McProxyChannel, FeedMemcContentAdapter>
{
protected:
  virtual string name() {
    return "FeedMemcContent";
  }
  virtual string endpoints() {
    return "@FeedMemcContent";
  }
  virtual size_t cluster() {
    return 1;
  }

public:
  FeedMemcContentAdapter() {
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
    _managers.resize(cluster() > 0 ? cluster() : 1);
  }
  void Test(const LongSeq& ids);
  void Test2(const IntSeq& ids);
private:
  FeedMemcContentPrx getManagerOneway(int id){
    return locate<FeedMemcContentPrx>(_managersOneway, "M", id, ONE_WAY);
  }

  FeedMemcContentPrx getManager(int id){
    return locate<FeedMemcContentPrx>(_managers, "M", id, TWO_WAY, 200);
  }

  vector<FeedMemcContentPrx> _managersOneway;
  vector<FeedMemcContentPrx> _managers;
};

}
}

#endif // _MC_PROXY_LOADER_ADAPTER_H_
