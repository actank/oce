
#ifndef __FEEDFOCUSADAPTER_H_
#define __FEEDFOCUSADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "RFeed.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedFocusAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedFocusAdapter> {
public:
  FeedFocusAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  FeedDataSeq GetFocusFeedData(int user, int begin,int limit);
  FeedDataSeq GetFocusFeedDataByStype(int user, int begin,int limit,const vector<int> & stypes);
  IntSeq GetFocuses(int user);
  int GetFocusFeedIncCount(int user);
/*
  void AddFocuses(int user, const ::MyUtil::IntSeq& focuses);
  void DelFocuses(int user, const ::MyUtil::IntSeq& focuses);
*/
  FocusManagerPrx getManager(int id);
  FocusManagerPrx getManagerOneway(int id);

protected:
  virtual string name() {
    return "FeedFocus";
  }
  virtual string endpoints() {
    return "@FeedFocus";
  }
  virtual size_t cluster() {
    return 5;
  }

  vector<FocusManagerPrx> _managersOneway;
  vector<FocusManagerPrx> _managers;
};

}
;
}
;
#endif /* __FEEDFOCUSADAPTER_H_ */
