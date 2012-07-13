#ifndef XCE_ARCH_SERVICE_H__
#define XCE_ARCH_SERVICE_H__

#include <string>
#include <Ice/Ice.h>
#include <IceBox/IceBox.h>
#include <boost/asio.hpp>

#include "feed/base_feed/globalinit.h"
#include "feed/base_feed/once.h"
//#include "arch/config.h"

extern std::string config_name_;
namespace xce {

typedef Ice::ObjectPtr (*ServantCreator)();
typedef void (*ServantDestroyer)();

struct ServantEntry {
  const char* identity; // identity@TheAdapter

  ServantCreator creator;
  ServantDestroyer destroyer;

  ServantEntry()
    : identity(0)
    , creator(0), destroyer(0) {}

  ServantEntry(const char* id, ServantCreator c, ServantDestroyer d)
    : identity(id), creator(c), destroyer(d) {}
};

}

// 本伺服名称，一般为业务类名字，如 User
// 用于 配置等

/*
<application name="name_of_app">
  <node name="node1">
    <icebox id="xxx" exe="/opt/Ice/bin/icebox" activation="on-demand">
      // 此处的两处改动为service的name参数会作为程序中Adapter的名字进行创建，因此要求service和
      // adapter的name属性值要相同。另外，adapter的id属性不一定必须的。如果客户端请求为OBJID@ADPID的形式，
      // 就要明确指出adapter的id。否则只需要以OBJID进行请求即可。
      // <service name="name_of_service" entry="dbdesc_server:create">
      <service name="name_of_adapter" entry="dbdesc_server:create">
        // <adapter id="xxx" name="name_of_adapter" endpoints="tcp"/>
        <adapter name="name_of_adapter" endpoints="tcp"/>
      </service>
    </icebox>
  </node>
</application>


xce::ServantEntry servo_objects_[] = {
  {"TheShop", HowToCreateService},
  {0, 0}
};

BEGIN_SERVANT_MAP()
  SERVANT_ENTRY("TheShop", ServantFactory, Free)
END_SERVANT_MAP()

*/

// TODO: 改为单向链表，支持编译期添加? 运行期添加
#define BEGIN_SERVANT_MAP(name) \
  GLOBAL_INIT(config_name, {config_name_ = name;})
 // config_name_ = name;

// 早期写法，没有 destroyer，实在不负责
// #define SERVANT_ENTRY(name, to_create) {(name), (to_create), 0},
#define SERVANT_ENTRY(name, to_create, to_destroy) \
  bool f##name = xce::Service::AddServant(xce::ServantEntry(#name, to_create, to_destroy));

#define REGISTER_SERVANT_ENTRY(name, to_create, to_destroy) \
  SERVANT_ENTRY(name, to_create, to_destroy)

#ifndef XCE_BUILD_BOX
#define END_SERVANT_MAP() 

#else
#define END_SERVANT_MAP() \
  int main(int argc, char* argv[]) { \
    xce::box_main(argc, argv);       \
  }
#endif

namespace xce {

class Service : public IceBox::Service {
public:
  virtual void start(const ::std::string& name, const Ice::CommunicatorPtr& communicator
    , const ::Ice::StringSeq& args);
  virtual void stop();

  static Ice::ObjectAdapterPtr& Adapter() {
    static Ice::ObjectAdapterPtr adapter_;
    return adapter_;
  }
  static const std::string& name() {
    // name 可以通过Adapter()->getName得到，但是导致
    // 测试等难以为继，故使用static member成员
    return name_;
  }

  static bool AddServant(const ServantEntry&);
private:
  static std::string name_;
};

// 方便创建各种Ptr，for ServantCreator
// TODO: rename ServantFactory => ServantFatory ?
template<typename T>
struct ServantFactory {
private:
  static void Init() {
    t_ = new T();
  }

  // 注意: 模板静态成员
  static T* t_;
  static XceOnceType init_t_;

public:
  static Ice::ObjectPtr CreateServant() {
    XceOnceInit(&init_t_, &ServantFactory::Init);
    return t_;
  }

  static void Destroy() {
    delete t_;
    t_ = 0;
  }

  static T* Instance() {
    XceOnceInit(&init_t_, &ServantFactory::Init);
    return t_;
  }

#if 0
  static Ice::ObjectPtr CreateStormObserver() {
    xce::storm::Observer* p = new xce::StormObserver<T>(*t_);
    TopicManagerAdapter tm = CachedTopicManager();
    if (tm && p) {
      Ice::ObjectPrx prx = Service::Adapter()->addWithUUID(p);
      tm->subscribe(config_name_, storm::ObserverPrx::uncheckedCast(prx));
    }
    return p;
  }

  static Ice::ObjectPtr CreateServiceObserver() {
    MyUtil::SvcObserver* p = new xce::ServiceObserver<T>(t_);
    SvcDescriptorAdapter tm = CachedSvcDescriptor();
    if (tm && p) {
      Ice::ObjectPrx prx = Service::Adapter()->addWithUUID(p);
      tm->subscribe(config_name_, MyUtil::SvcObserverPrx::uncheckedCast(prx));
    }
    return p;
  }
#endif
#if 0
  static Ice::ObjectPtr CreateConfigListener() {
    ListenerOfArch<T>* p = new (std::nothrow) ListenerOfArch<T>(*t_);

    ConfigPrx config_server = CachedConfigPrx();
    ASSERT(config_server);
    if (config_server) {
      Ice::ObjectPrx base = Service::Adapter()->addWithUUID(p);
      ASSERT(base);
      ConfigListenerPrx listener_prx = ConfigListenerPrx::uncheckedCast(base);
      ASSERT(listener_prx);
      config_server->Subscribe(config_name_, listener_prx);      // "user"
      config_server->Subscribe(Service::name(), listener_prx);  // "user0"
    }
    return p;
  }
#endif
};

template<typename T> 
T* ServantFactory<T>::t_ = 0;

template<typename T> 
XceOnceType ServantFactory<T>::init_t_;

// TODO: split to file
namespace detail {

std::string PrintProperty(const Ice::PropertiesPtr& prop);

} // detail

// 并不是所有的程序都是 service
Ice::ObjectAdapterPtr CreateObjectAdapter(int argc, char* argv[]);

Ice::ObjectAdapterPtr CreateObjectAdapter(Ice::CommunicatorPtr c
    , const std::string& name, const std::string& host, int port);

Ice::ObjectAdapterPtr CreateObjectAdapter(const std::string& name
    , const std::string& host, int port);

int box_main(int argc, char* argv[]);

} // namespace xce

#endif // XCE_ARCH_SERVICE_H__
