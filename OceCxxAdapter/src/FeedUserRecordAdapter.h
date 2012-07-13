#ifndef FEEDUSERRECORDADAPTER_H_
#define FEEDUSERRECORDADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedUserRecord.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedUserRecordAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedUserRecordAdapter> {
public:
  FeedUserRecordAdapter() {
    _managers.resize(cluster() >0 ? cluster() :1);
    _managersOneway.resize(cluster() > 0 ? cluster() :1);
  }
void Set(::Ice::Int uid, ::Ice::Long feedid, ::Ice::Int type, const Ice::Current& current = Ice::Current());
FeedUserRecordResPtr  Get(const int uid, const Ice::Current& current = Ice::Current());
protected:
  virtual string name() {
    return "FeedUserRecord";
  }
  virtual string endpoints() {
    return "@FeedUserRecord";
  }
  virtual size_t cluster() {
    return 1;
  }

  FeedUserRecordPrx getManager(int id);
  FeedUserRecordPrx getManagerOneway(int id);
  
  vector<FeedUserRecordPrx> _managersOneway;
  vector<FeedUserRecordPrx> _managers;
};
};
};
#endif
