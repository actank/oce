#ifndef __EDM_FEED_EDM_SENDER_ADAPTER_H__
#define __EDM_FEED_EDM_SENDER_ADAPTER_H__
#include "FeedEdmSender.h"
#include "Singleton.h"
#include "AdapterI.h"
namespace MyUtil {

class FeedEdmTestChannel : public Channel {
public:
  FeedEdmTestChannel() {
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
class FeedEdmSenderAdapter: public MyUtil::AdapterI,
    //public MyUtil::AdapterISingleton<MyUtil::FeedEdmTestChannel, FeedEdmSenderAdapter> { 
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedEdmSenderAdapter> {
public:
  FeedEdmSenderAdapter(){
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }
  void Receive(Ice::Int user_id, const MyUtil::Str2StrMap& params);
  void ReceiveForPhone(Ice::Int user_id, const MyUtil::Str2StrMap& params);
  void NoCheckReceive(Ice::Int user_id, const MyUtil::Str2StrMap& params);
	void ClearFrequencyCacheByUser(int userid);
  void FlushCache();
protected:
  virtual std::string name() {
    return "FeedEdmSender";
  }
  virtual std::string endpoints() {
    return "@FeedEdmSender";
  }
  virtual size_t cluster() {
    return 2;
  }

  FeedEdmSenderPrx getManager(int id);
  FeedEdmSenderPrx getManagerOneway(int id);

  std::vector<FeedEdmSenderPrx> _managersOneway;
  std::vector<FeedEdmSenderPrx> _managers;



};
}
}
#endif
