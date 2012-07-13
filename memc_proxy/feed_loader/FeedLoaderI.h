#ifndef _XCE_FEED_MEMC_LOADER_H_
#define _XCE_FEED_MEMC_LOADER_H_

#include <set>

#include "Singleton.h"
#include "MemcProxy.h"
#include "TaskManager.h"

namespace xce {
namespace feed {

using namespace std;
using namespace MyUtil;


class LoadFeedTask: public MyUtil::Task {
public:
  LoadFeedTask(Ice::Long id) : id_(id) {}
  virtual void handle();

private:
  Ice::Long id_;
};

class MemcFeedLoaderI : public MyUtil::Timer, public MemcFeedLoader, public Singleton<MemcFeedLoaderI> {
public:
  MemcFeedLoaderI();
  void LoadFeed(Ice::Long id, const Ice::Current& = Ice::Current());
  void LoadFeedSeq(const Ice::LongSeq & ids, const Ice::Current& = Ice::Current());

  void LoadFeedByKey(const string & key, const Ice::Current& = Ice::Current());
  void LoadFeedSeqByKey(const MyUtil::StrSeq & keys, const Ice::Current& = Ice::Current());

  MyUtil::Str2StrMap GetFeedSeqByKey(const MyUtil::StrSeq & keys, const Ice::Current& = Ice::Current());
private:
  set<Ice::Long> recent_loaded_ids_;
  IceUtil::Mutex mutex_;

  virtual void handle();
};

}
}

#endif // _XCE_FEED_MEMC_LOADER_H_
