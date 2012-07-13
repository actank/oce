#include "MemcProxyI.h"

#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>

#include "key_mapping.h"
#include "proxy_server.h"

#include "MemcFeedLoaderAdapter.h"
#include "ServiceI.h"
#include "LogWrapper.h"

#include "client/linux/handler/exception_handler.h"

using namespace xce::feed;
using namespace MyUtil;

bool MyCallback(const char *dump_path,
    const char *minidump_id,
    void *context,
    bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}

void MyUtil::initialize() {
	MCE_DEBUG("begin initialize...");

  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
  
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&MemcProxyI::instance(), service.createIdentity("M", ""));

  if (!InitCacheClusters()) {
	  MCE_WARN("InitCacheClusters() error.");
    return;
  }

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string listen_socket = props->getPropertyWithDefault("Service." + service.getName() + ".Listen", "127.0.0.1:11311");

  MemcProxyI::instance().SetEndpoint(listen_socket);

  size_t pos = listen_socket.find(':');
  unsigned short port = boost::lexical_cast<unsigned short>(listen_socket.c_str() + pos + 1);
  listen_socket.resize(pos);
  ip::tcp::endpoint endpoint(ip::address::from_string(listen_socket), port);

	MCE_INFO("MemcProxy listen on : " << endpoint);

  ProxyServer * server = new ProxyServer(endpoint);
  boost::thread t(boost::bind(&ProxyServer::Run, server));
  t.detach();
}

bool MemcProxyI::SetUpstreamList(const ::std::string & cfg, const Ice::Current&) {
  return false;
}
  // 取得McProxy 的 memcache 监听端口
string MemcProxyI::GetEndpoint(const Ice::Current&) {
  return endpoint_;
}

GetMissedTask::GetMissedTask(const MyUtil::StrSeq & keys, MemcCommandPtr cmd) 
    : memc_cmd_(cmd) {
  for(size_t i = 0; i < keys.size(); ++i) {
    if (boost::starts_with(keys[i], "FEED#")) {
      keys_.push_back(keys[i]);
      MCE_DEBUG("key to load: " << keys[i]);
    } else {
      MCE_DEBUG("key not loaded : " << keys[i]);
    }
  }
}

void GetMissedTask::handle() {
  MyUtil::Str2StrMap res;
  try {
    res = MemcFeedLoaderAdapter::instance().GetFeedSeqByKey(keys_);
  } catch(Ice::Exception& e) {
    MCE_WARN("MemcFeedLoaderAdapter error : " << e);
  } catch (...) {
    MCE_WARN("MemcFeedLoaderAdapter unknown error");
  }

  MyUtil::Str2StrMap::iterator it = res.begin();
  string & buf = memc_cmd_->missed_buf();
  buf.clear();
  for (; it != res.end(); ++it) {
    buf += "VALUE ";
    buf += it->first;

    if (*(it->second.rend()) == 'c') {
      buf += " 1 ";
    } else {
      buf += " 0 ";
    }
    buf += boost::lexical_cast<string>(it->second.size() - 1);
    buf += "\r\n";
    buf.append(it->second, 0, it->second.size() - 1);
    buf += "\r\n";
  }
  memc_cmd_->DispatchMissedKeyData();
}

