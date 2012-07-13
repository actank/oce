#ifndef __EDM_FEED_EDM_CACHE_ADAPTER_H__
#define __EDM_FEED_EDM_CACHE_ADAPTER_H__
#include "FeedEdmCache.h"
#include "Singleton.h"
#include "AdapterI.h"
namespace MyUtil {

class FeedEdmTestChannel2 : public Channel {
public:
  FeedEdmTestChannel2() {
    Ice::PropertiesPtr properties = Ice::createProperties();
    properties->setProperty("Ice.Override.ConnectTimeout", "150");
    properties->setProperty("IceGrid.InstanceName", "XceFeed");

    properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
    properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");

    properties->setProperty("Ice.Default.Host", IpAddr::inner());

    properties->setProperty(
                  "Ice.Default.Locator",
                  "XceFeed/Locator:default -h 10.3.17.127 -p 14300");
    Ice::InitializationData id;
    id.properties = properties;
    _communicator = Ice::initialize(id);

    _query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("XceFeed/Query"));
  }
};

}
namespace xce {
namespace edm {
class FeedEdmCacheAdapter: public MyUtil::AdapterI,
    //public MyUtil::AdapterISingleton<MyUtil::FeedEdmTestChannel2, FeedEdmCacheAdapter> {
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedEdmCacheAdapter> {
public:
  FeedEdmCacheAdapter(){
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }
  FeedSeedInfoPtr SelectEdm(Ice::Int user_id, const MyUtil::Str2StrMap& params, const FeedUserInfoPtr& user);
  FeedSeedInfoPtr SelectEdmForPhone(Ice::Int user_id, const MyUtil::Str2StrMap& params, const FeedUserInfoPtr& user);
  FeedSeedInfoPtr SelectEdmById(int edmid);
  FeedSeedInfoPtr SelectEdmByType(int user_id, int type);
  void SetSendingFlag(Ice::Int edm_id);
  void SetSuspendFlag(Ice::Int edm_id);
  void UpdateEdmInfo(Ice::Int edmId);
  void SetForComplete();
  std::string LookUpEdm(Ice::Int edm_Id);
  
   //void SetSendingFlag(int type, int edm_id);

  //void SetSuspendFlag(Ice::Int type, Ice::Int edm_id);
  //void IncreaseConsume(Ice::Int type, Ice::Int id);
  //void IncreaseConsumeInMinite(Ice::Int id);
protected:
  virtual std::string name() {
    return "FeedEdmCache";
  }
  virtual std::string endpoints() {
    return "@FeedEdmCache";
  }
  virtual size_t cluster() {
    return 1;
  }

  FeedEdmCachePrx getManager(int id);
  FeedEdmCachePrx getManagerOneway(int id);

  std::vector<FeedEdmCachePrx> _managersOneway;
  std::vector<FeedEdmCachePrx> _managers;



};
}
}
#endif
