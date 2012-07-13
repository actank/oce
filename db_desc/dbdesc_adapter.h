#ifndef XCE_DB_DESCRIPTOR_ADAPTER_H__
#define XCE_DB_DESCRIPTOR_ADAPTER_H__

#include "arch/adapter.h"
#include "dbdesc/dbdescriptor.h"

inline Ice::CommunicatorPtr test() {
  const xce::stringpair arr [] = {
    {"Ice.Override.ConnectTimeout", "300"},
    {"Ice.ThreadPool.Client.StackSize", "65535"},
    {"Ice.ThreadPool.Server.StackSize", "65535"},
    {"Ice.MessageSizeMax", "20480"},
    {"Ice.Default.Host", xce::Network::local_ip().c_str()},
    // {"Ice.Default.Host", IpAddr::inner()},

    // {"Ice.Default.Locator", "XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"},
    {"Ice.Default.Locator", "IceGrid/Locator:tcp -h 10.2.18.202 -p 4061"},
    // {"Ice.Trace.Network", "1"},
    {"Ice.Trace.Protocol", "1"},
    {0, 0} // 标记结束
  };
  return xce::BuildCommunicator(arr);
}

namespace xce {

typedef ClientPtr<com::xiaonei::xce::DbDescriptorPrx> DbDescAdapter;

inline 
DbDescAdapter CreateDbDescAdapter() {
#if 0
  RegistryLocator r(test());
  DbDescAdapter p_(r);
  bool f = p_.Create("DbDescriptor@db2");
  return p_;
#else

  RegistryLocator r(RegistryLocator::Default());
  DbDescAdapter p_(r);
  bool f = p_.Create("DCS@DbDescriptor");
  return p_;

#endif
}

inline 
DbDescAdapter CachedDbDescAdapter() {
  static DbDescAdapter p_ = CreateDbDescAdapter();
  return p_;
}

}
#endif // XCE_DB_DESCRIPTOR_ADAPTER_H__
