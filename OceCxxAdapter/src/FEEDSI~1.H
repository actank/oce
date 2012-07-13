
#ifndef __FEEDFOCUSINVERTADAPTER_H_
#define __FEEDFOCUSINVERTADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedSink.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedSinkAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedSinkAdapter> {
public:
  FeedSinkAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  void AddFeed(const FeedItem& fitem,
      const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight){
  	if(id2weight.empty()){
  		return;
  	}
  	int first = id2weight.begin()->first;
  	FeedSinkPrx::uncheckedCast(getManagerOneway(first)->ice_datagram())->AddFeed(fitem,config,id2weight);
  }

  FeedSinkPrx getManager(int id) {
    return locate<FeedSinkPrx> (_managers, "M", id, TWO_WAY);
  }

  FeedSinkPrx getManagerOneway(int id) {
    return locate<FeedSinkPrx> (_managersOneway, "M", id, ONE_WAY);
  }

protected:
  virtual string name() {
    return "FeedSink";
  }
  virtual string endpoints() {
    return "@FeedSink";
  }
  virtual size_t cluster() {
    return 1;
  }

  vector<FeedSinkPrx> _managersOneway;
  vector<FeedSinkPrx> _managers;
};

}
;
}
;
#endif /* __FeedSinkAdapter_H_ */
