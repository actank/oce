#ifndef _XCE_TALK_CACHE_CLIENT_H_
#define _XCE_TALK_CACHE_CLIENT_H_

#include "TalkCacheLoader.h"
#include "Singleton.h"
#include "MemcachedClientPool.h"

namespace xce {
namespace talk {

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::talk::cache::loader;
//typedef com::xiaonei::talk::cache::loader::TalkUser TalkUser;
//typedef com::xiaonei::talk::cache::loader::TalkUserPtr TalkUserPtr;
//typedef com::xiaonei::talk::cache::loader::TalkUserMap TalkUserMap;


class TimeStat{
  timeval _begin, _end;
  public:
  TimeStat(){
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



class TalkCacheClient : public Singleton<TalkCacheClient> {
public:
  ~TalkCacheClient();
  TalkCacheClient();

  TalkUserPtr GetUser(int id);
  TalkUserMap GetUsers(const MyUtil::IntSeq& ids);
  TalkUserPtr GetUserByIdWithLoad(int userid);
  TalkUserMap GetUserBySeqWithLoad(int uid, const MyUtil::IntSeq& ids);
  bool SetUser(const TalkUserPtr& user);
protected:
  bool SerializeTalkUser(const TalkUserPtr& talk_user, string * serialized);
private:
  PoolManager pool_;
};

}
}
#endif // _XCE_TALK_CACHE_CLIENT_H_
