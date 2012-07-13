
#ifndef PhotoMemory_H_
#define PhotoMemory_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "PhotoMemory.h"


namespace MyUtil {

class FeedTestChannel : public Channel {
public:
  FeedTestChannel() {
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
namespace feed {

using namespace MyUtil;

class PhotoMemoryAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, PhotoMemoryAdapter> {
    // public MyUtil::AdapterISingleton<MyUtil::FeedTestChannel, PhotoMemoryAdapter> {

public:
  PhotoMemoryAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  PhotoMemoryManagerPrx getManager(int id);
  PhotoMemoryManagerPrx getManagerOneway(int id);


  PhotoContentSeq GetPhotoContentSeq(int userid, int begin, int end);
  void SetRead(int userid, long photoid);

protected:
  virtual string name() {
    return "PhotoMemory";
  }
  virtual string endpoints() {
    return "@PhotoMemory";
  }
  virtual size_t cluster() {
    return 5;
  }

  vector<PhotoMemoryManagerPrx> _managersOneway;
  vector<PhotoMemoryManagerPrx> _managers;
};

};

};
#endif /* PhotoMemory_H_ */
