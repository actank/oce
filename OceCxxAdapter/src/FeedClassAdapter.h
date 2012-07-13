
#ifndef __FEEDCLASSADAPTER_H_
#define __FEEDCLASSADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "RFeed.h"
#include "FeedClass.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedClassAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedClassAdapter> {
public:
  FeedClassAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

//  void AddMember(int memberid,int groupid);
//  void DelMember(int memberid,int groupid);
//  void DelGroup(int groupid);
  FeedDataSeq GetFeedDataOfFriendsInGroups(int uid, MyUtil::IntSeq groupids,int begin, int limit) ;

//  FeedDataSeq GetFocusFeedData(int user, int begin,int limit);
//  IntSeq GetFocuses(int user);

  FeedClassPrx getManager(int id);
  FeedClassPrx getManagerOneway(int id);

protected:
  virtual string name() {
    return "FeedClass";
  }
  virtual string endpoints() {
    return "@FeedClass";
  }
  virtual size_t cluster() {
    return 2;
  }

  vector<FeedClassPrx> _managersOneway;
  vector<FeedClassPrx> _managers;
};

}
;
}
;
#endif /* __FEEDFOCUSADAPTER_H_ */
