#ifndef _XCE_FEED_MEMC_LOADER_H_
#define _XCE_FEED_MEMC_LOADER_H_

#include "Singleton.h"
#include "RFeed.h"
#include "FeedMemcProxy.h"

namespace xce {
namespace feed {

using namespace std;
using namespace MyUtil;

// 从memc 调用. 作为取得 content 的例子
class FeedMemcContentI : public FeedMemcContent, public Singleton<FeedMemcContentI> {
public:
	virtual FeedContentPtr GetFeed(Ice::Long id, const Ice::Current& = Ice::Current());
  virtual FeedContentDict GetFeedDict(const MyUtil::LongSeq & ids, const Ice::Current& = Ice::Current());

  //virtual FeedContentDict LocateFeedDict(int uid,const MyUtil::LongSeq & fids, const Ice::Current&) ;
  virtual FeedDataSeq GetFeedDataByIds(int uid,const MyUtil::LongSeq & fids, const Ice::Current& = Ice::Current()) ;
private:
  void CheckLoad(long first, const vector<Ice::Long> & fids, FeedContentDict & dict) ;
  virtual void Test(const MyUtil::LongSeq & ids, const Ice::Current& = Ice::Current());
  virtual void Test2(const MyUtil::IntSeq & ids, const Ice::Current& = Ice::Current());
};

}
}

#endif // _XCE_FEED_MEMC_LOADER_H_
