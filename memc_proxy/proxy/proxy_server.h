
#ifndef _MEMC_PROXY_PROXY_SERVER_
#define _MEMC_PROXY_PROXY_SERVER_

#include <vector>
#include <boost/bind.hpp>

#include "key_mapping.h"
#include "client_conn.h"
#include "memc_command.h"
#include "upstream_conn.h"

namespace xce {
namespace feed {

class ProxyServer
{
public:
  ProxyServer(const ip::tcp::endpoint & ep);
  ~ProxyServer();

  void Run();

private:
  void StartAccept();
  void HandleAccept(ClientConnectionPtr conn, const boost::system::error_code& error);

  io_service io_service_;
  io_service::work work_;
  ip::tcp::acceptor acceptor_;

  size_t dispatch_threads_;
  size_t dispatch_round_;
  vector<io_service *> dispatch_services_;
  vector<io_service::work *> dispatch_works_;
  // TODO : 结构丑陋不清晰
  vector<UpstreamConnPool *> upconn_pools_;
};

}
}

#endif  // _MEMC_PROXY_PROXY_SERVER_
