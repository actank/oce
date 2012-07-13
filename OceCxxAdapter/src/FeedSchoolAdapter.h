#ifndef FEEDSCHOOLADAPTER_H_
#define FEEDSCHOOLADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "RFeed.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedSchoolAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedSchoolAdapter> {
public:
  FeedSchoolAdapter() {
    _managers.resize(cluster() >0 ? cluster() :1);
    _managersOneway.resize(cluster() > 0 ? cluster() :1);
  }

  void addFeed(const FeedItem& item, const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight);
protected:
  virtual string name() {
    return "FeedSchool";
  }
  virtual string endpoints() {
    return "@FeedSchool";
  }
  virtual size_t cluster() {
    return 2;
  }

  FeedItemManagerPrx getManager(int id);
  FeedItemManagerPrx getManagerOneway(int id);
  
  vector<FeedItemManagerPrx> _managersOneway;
  vector<FeedItemManagerPrx> _managers;
};
};
};
#endif
