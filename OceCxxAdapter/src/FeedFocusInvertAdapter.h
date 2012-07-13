
#ifndef __FEEDFOCUSINVERTADAPTER_H_
#define __FEEDFOCUSINVERTADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedFocusInvert.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedFocusInvertAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedFocusInvertAdapter> {
public:
  FeedFocusInvertAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }


  IntSeq GetSources(int target){
  	if(target < 0){
  		MCE_WARN("FeedFocusInvertAdapter::GetSources. wrong param.  target:" << target);
  		return IntSeq();
  	}
  	return getManager(target)->GetSources(target);
  }
  void AddFocus(int source,int target){
  	if(source < 0 || target < 0){
  		MCE_WARN("FeedFocusInvertAdapter::AddFocus. wrong param. source:" << source << " target:" << target);
  		return;
  	}
  	getManager(target)->AddFocus(source,target);
  }
  void DelFocus(int source,int target){
  	if(source < 0 || target < 0){
  		MCE_WARN("FeedFocusInvertAdapter::DelFocus. wrong param. source:" << source << " target:" << target);
  		return;
  	}
  	getManager(target)->DelFocus(source,target);
  }
  FeedFocusInvertPrx getManager(int id);
  FeedFocusInvertPrx getManagerOneway(int id);

protected:
  virtual string name() {
    return "FeedFocusInvert";
  }
  virtual string endpoints() {
    return "@FeedFocusInvert";
  }
  virtual size_t cluster() {
    return 5;
  }

  vector<FeedFocusInvertPrx> _managersOneway;
  vector<FeedFocusInvertPrx> _managers;
};

}
;
}
;
#endif /* __FeedFocusInvertAdapter_H_ */
