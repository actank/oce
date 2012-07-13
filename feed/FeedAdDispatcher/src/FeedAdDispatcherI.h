#ifndef __FEED_FEED_AD_DISPATCHER_I_H__
#define __FEED_FEED_AD_DISPATCHER_I_H__
#include "Evictor.h"
#include "FeedAdDispatcher.h"
#include "ServiceI.h"
#include "IceUtil/RWRecMutex.h"
#include "QueryRunner.h"
#include "RFeed.h"
#include <map>
namespace xce {
namespace feed {
using namespace MyUtil;
class AdConfigure: public IceUtil::Shared {
public:
  Ice::Int stype_;
  MyUtil::Str2StrMap content_data_;
  MyUtil::Str2StrMap extra_props_;
};
typedef IceUtil::Handle<AdConfigure> AdConfigurePtr;
class FeedAdDispatcherI : public  FeedAdDispatcher, public MyUtil::Singleton<FeedAdDispatcherI> {
public:
  FeedAdDispatcherI();
  void Dispatch(Ice::Int ad_id, const MyUtil::Str2StrMap& params ,const Ice::Current& = Ice::Current());  
private:
  xce::feed::FeedSeedPtr _BuildSeed(Ice::Int ad_id, const MyUtil::Str2StrMap& params, const AdConfigurePtr& ad_ptr);
  AdConfigurePtr _LoadConfigureFromDB(Ice::Int ad_id);
  AdConfigurePtr _FormatAdConfigure(const mysqlpp::Row& row);
private:
  SizedEvictor< Ice::Int, AdConfigurePtr > evict_map_;
  //IceUtil::RWRecMutex mutex_;
};
}
}
#endif
