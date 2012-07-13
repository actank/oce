#ifndef XCE_ARCH_CONFIG_ADAPTER_H__
#define XCE_ARCH_CONFIG_ADAPTER_H__

#include "feed/arch_feed/adapter.h"
#include "feed/arch_feed/configserver.h"

namespace xce {

typedef ClientPtr<ConfigServerPrx> ConfigPrx;

// RemoteConfig 实在是一个太特殊的服务，肩负了很多使命
// 故把Communicator写在这个地方
inline Ice::CommunicatorPtr ConfigCommunicator() {
  const stringpair arr [] = {
    {"Ice.Override.ConnectTimeout", "1000"},
    {"Ice.ThreadPool.Client.StackSize", "65535"},
    {"Ice.ThreadPool.Server.StackSize", "65535"},
    {"Ice.MessageSizeMax", "20480"},
    {"Ice.Default.Host", Network::local_ip().c_str()},
#ifndef XCE_CONFIG_CUSTOM
    {"IceGrid.InstanceName", "ConfigGrid"},
    {"Ice.Default.Locator", "ConfigGrid/Locator:default -h 10.22.202.47 -p 4061"},
#else
    {"IceGrid.InstanceName", XCE_CONFIG_GRID_NAME},
    {"Ice.Default.Locator", XCE_CONFIG_GRID_LOCATOR},
#endif
    {0, 0} // 标记结束
  };
  return BuildCommunicator(arr);
}

inline ConfigPrx CreateConfigPrx() {
  RegistryLocator r(ConfigCommunicator());
  return ConfigPrx(r, "config@remoteconfig");
}

inline ConfigPrx& CachedConfigPrx() {
  static ConfigPrx p_ = CreateConfigPrx();
  return p_;
}

}
#endif // XCE_ARCH_CONFIG_ADAPTER_H__
