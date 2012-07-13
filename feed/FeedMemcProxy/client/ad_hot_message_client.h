
#ifndef _XCE_AD_HOT_MESSAGE_CLIENT_H_
#define _XCE_AD_HOT_MESSAGE_CLIENT_H_

#include <Ice/Ice.h>

#include "MemcachedClientPool.h"
#include "Singleton.h"
// #include "user_profile.pb.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdHotMessageClient : public Singleton<AdHotMessageClient> {
public:
  AdHotMessageClient();
  ~AdHotMessageClient();

  bool Set(Ice::Int uid, Ice::Int ad_id, const string & message);
  bool Get(Ice::Int uid, Ice::Int ad_id, string * message);
protected:
  PoolManager pool_;
};

}
}
#endif // _XCE_AD_HOT_MESSAGE_CLIENT_H_
