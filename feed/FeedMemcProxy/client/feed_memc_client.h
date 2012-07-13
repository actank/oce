
#ifndef _XCE_FEED_MEMC_CLIENT_H_
#define _XCE_FEED_MEMC_CLIENT_H_

#include "RFeed.h"
#include "Singleton.h"
#include "MemcachedClientPool.h"


class TimeStat1{
        timeval _begin, _end;
public:
        TimeStat1(){
                reset();
        }
        void reset(){
                gettimeofday(&_begin, NULL);
        }
        float getTime(){
                gettimeofday(&_end, NULL);
                float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
                                -_begin.tv_usec;
                timeuse/=1000;
                return timeuse;
        }
};

namespace xce {
namespace feed {

using namespace std;
using namespace MyUtil;

class FeedMemcClient : public Singleton<FeedMemcClient> {
public:
  ~FeedMemcClient();
  FeedMemcClient();

  FeedContentPtr GetFeed(Ice::Long id);
  FeedContentDict GetFeedDict(const MyUtil::LongSeq& ids);
  bool SetFeed(const FeedContentPtr& content);
  bool SetEmptyFeed(Ice::Long id);

  // 串行化的结果
  bool SerializeFeed(const FeedContentPtr& content, string * serialized);
protected:
  PoolManager pool_;
};

}
}
#endif //_XCE_FEED_MEMC_CLIENT_H_
