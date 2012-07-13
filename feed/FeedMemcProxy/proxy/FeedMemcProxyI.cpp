#include "FeedMemcProxyI.h"

#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>

#include "key_mapping.h"
#include "proxy_server.h"

#include "FeedMemcLoaderAdapter.h"
#include "ServiceI.h"
#include "LogWrapper.h"
//#include "PerformanceStat.h"

//#include "client/linux/handler/exception_handler.h"

using namespace xce::feed;
using namespace MyUtil;

/*bool MyCallback(const char *dump_path,
    const char *minidump_id,
    void *context,
    bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}*/

void MyUtil::initialize() {
	MCE_DEBUG("begin initialize...");
  
	//TaskManager::instance().scheduleRepeated(&PerformanceStat::instance());
  //static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
  
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedMemcProxyI::instance(), service.createIdentity("M", ""));

  if (!InitCacheClusters()) {
	  MCE_WARN("InitCacheClusters() error.");
    return;
  }

  /*Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string listen_socket = props->getPropertyWithDefault("Service." + service.getName() + ".Listen", "127.0.0.1:11311");

  FeedMemcProxyI::instance().SetEndpoint(listen_socket);

  size_t pos = listen_socket.find(':');
  unsigned short port = boost::lexical_cast<unsigned short>(listen_socket.c_str() + pos + 1);
  listen_socket.resize(pos);
  ip::tcp::endpoint endpoint(ip::address::from_string(listen_socket), port);

	MCE_INFO("MemcProxy listen on : " << endpoint);

  ProxyServer * server = new ProxyServer(endpoint);
  */
  boost::thread t(boost::bind(&ProxyServer::Run, &ProxyServer::instance()));
  t.detach();
}

bool FeedMemcProxyI::SetUpstreamList(const ::std::string & cfg, const Ice::Current&) {
  return false;
}

string GetIp(){
  char host_name[255]; 
  if(gethostname(host_name, 255) == -1) { 
    MCE_INFO("FeedMemcProxyI::GetIp --> get host name err"); 
    return ""; 
  } 
  struct hostent *phe = gethostbyname(host_name); 
  if (phe == 0) { 
    return ""; 
  }
  string ip; 
  for (int i = 0; phe->h_addr_list[i] != 0; ++i) { 
    struct in_addr addr; 
    memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr)); 
    ip = inet_ntoa(addr);
    MCE_INFO("FeedMemcProxyI::GetIp --> " << ip);
  } 
  return ip;
}

ProxyInfoPtr FeedMemcProxyI::GetProxyInfo(const Ice::Current&){
  if(ip_.empty() || ip_=="127.0.0.1"){
    ip_ = GetIp();
    if(ip_.empty() || ip_=="127.0.0.1"){
      return NULL;
    }
  }
  ProxyInfoPtr info = new ProxyInfo;
  info->ip = ip_;
  info->port = 11315;
  info->connections = ProxyServer::instance().ClientSize();
  return info;
}

  // 取得McProxy 的 memcache 监听端口
string FeedMemcProxyI::GetEndpoint(const Ice::Current&) {
  char host_name[255]; 
  if(gethostname(host_name, 255) == -1) { 
    MCE_INFO("FeedMemcProxyI::GetEndpoint --> get host name err"); 
    return ""; 
  } 
  struct hostent *phe = gethostbyname(host_name); 
  if (phe == 0) { 
    return ""; 
  }
  string ip; 
  for (int i = 0; phe->h_addr_list[i] != 0; ++i) { 
    struct in_addr addr; 
    memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr)); 
    ip = inet_ntoa(addr);
    MCE_INFO("FeedMemcProxyI::GetEndpoint --> " << ip);
  } 
  return ip+":11315";
}
/*
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
}*/
/*
void GetMissedTask::handle() {
  MyUtil::Str2StrMap res;
  try {
    res = FeedMemcLoaderAdapter::instance().GetFeedSeqByKey(keys_);
  } catch(Ice::Exception& e) {
    MCE_WARN("FeedMemcLoaderAdapter error : " << e);
  } catch (...) {
    MCE_WARN("FeedMemcLoaderAdapter unknown error");
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
}*/

