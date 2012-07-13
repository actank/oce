#ifndef XCE_ARCH_RPCURI_H__
#define XCE_ARCH_RPCURI_H__

#include <Ice/Ice.h>
#include <IceBox/IceBox.h>

#include "base/common.h"
#include "base/network.h"

namespace xce {

// ice某些部分设计过于复杂，希望用一个普通字符串表示一个服务
// 如：channel|host:port/server?timeout=1000
// 更优雅一点形式： rpc://host:port/server?timeout=1000

// 折衷为：
// 为了能从一个字符串创建一个Ice::ProxyPtr, 需要表示集群，特增加channel
// rpcuri为以下2种形式
// channel|object proxy
// proxy

// 如：
// -------------- proxy -----------
// Cache:tcp -h xx.xx.xx.xx -p 1234

// -------------- channel ---------------------|-proxy-
// CacheGrid/Locator:tcp -h xx.xx.xx.xx -p 1234|Cache


// --Ice.Default.Locator="CacheGrid/Locator:tcp -h xx.xx.xx.xx -p 1234"

inline Ice::CommunicatorPtr CreateChannel(const std::string& default_locator);
inline Ice::ObjectPrx CreateObjectById(Ice::CommunicatorPtr cp, const std::string& object_id);

inline Ice::ObjectPrx CreateObject(const std::string& uri) {
  std::string::size_type pos_splash = uri.find_first_of('/');
  if (pos_splash == std::string::npos)
    return CreateObjectById(0, uri);

  std::string::size_type pos_2 = uri.find_first_of('|', pos_splash);
  if (pos_splash == std::string::npos)
    return 0;

  std::string default_locator = uri.substr(0, pos_2);
  std::string object_id = uri.substr(pos_2 + 1);
  Ice::CommunicatorPtr cp = CreateChannel(default_locator);
  if (cp) {
    return CreateObjectById(cp, object_id);
  }
}

struct stringpair {
  const char * key;
  const char * value;
};

// 最后一个条目key 必须是 0
inline 
Ice::CommunicatorPtr BuildCommunicator(const stringpair* arr) {
  Ice::PropertiesPtr prs = Ice::createProperties();
  for (const stringpair* sp = arr; sp->key; sp++) {
    prs->setProperty(sp->key, sp->value);
  }
  Ice::InitializationData id;
  id.properties = prs;
  return Ice::initialize(id);
}

Ice::CommunicatorPtr CreateChannel(const std::string& default_locator) {
  std::string::size_type pos = default_locator.find_first_of('/');
  ASSERT(pos != std::string::npos);
  const std::string name = default_locator.substr(0, pos);
  const xce::stringpair arr [] = {
    {"Ice.Override.ConnectTimeout", "300"},
    {"IceGrid.InstanceName", name.c_str()},
    {"Ice.MessageSizeMax", "20480"}, // TODO: it's important
    {"Ice.Default.Host", xce::Network::local_ip().c_str()},
    {"Ice.Default.Locator", default_locator.c_str()},
    {0, 0} // 标记结束
  };
  return xce::BuildCommunicator(arr);
}

inline Ice::ObjectPrx CreateObjectById(Ice::CommunicatorPtr cp, const std::string& object_id) {
  // 创建一个CommunicatorPtr
  if (!cp) {
    int argc = 0;
    char* argv[] = {0};
    cp = Ice::initialize(argc, argv);
  }

  Ice::ObjectPrx base;
  try {
    base = cp->stringToProxy(object_id);
  } catch(Ice::Exception&) {}
  return base;
}

} // namespace xce
#endif // XCE_ARCH_RPCURI_H__
