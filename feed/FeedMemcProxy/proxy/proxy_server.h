
#ifndef _MEMC_PROXY_PROXY_SERVER_
#define _MEMC_PROXY_PROXY_SERVER_

#include <vector>
#include <boost/bind.hpp>

#include "key_mapping.h"
#include "client_conn.h"
#include "upstream_conn.h"
#include "Singleton.h"

namespace xce {
namespace feed {

class ProxyServer : public MyUtil::Singleton<ProxyServer>
{
public:
  ProxyServer();
  ~ProxyServer();

  void Run();
  
  void PushClient(int index, ClientConnection* cli){
    clients_[index].insert(cli);
  }
  void EraseClient(int index, ClientConnection* cli){
    clients_[index].erase(cli);
  }
  bool IsExist(int index, ClientConnection* cli){
    return clients_[index].find(cli)!=clients_[index].end();
  }
  int ClientSize(){
    int count = 0;
    for(int i=0; i<clients_.size(); i++){
      count += clients_.at(i).size();
    }
    return count;
  }
private:
  void RunConnService(int i);
  void StartAccept();
  void HandleAccept(ClientConnection* conn, const boost::system::error_code& error);
  void StatTimerHandle(deadline_timer* timer, int index, const boost::system::error_code& error);

  io_service io_service_;
  io_service::work work_;
  ip::tcp::endpoint endpoint_;
  ip::tcp::acceptor acceptor_;
  
  size_t dispatch_threads_;
  size_t dispatch_round_;
  vector<io_service *>        dispatch_services_;
  vector<io_service::work *>  dispatch_works_;
  vector<set<ClientConnection*> > clients_;

  // TODO : 结构丑陋不清晰
  vector<UpstreamConnPool *> upconn_pools_;
  vector<PoolManager*> pool_managers_;
  vector<boost::asio::deadline_timer*> stat_timers_;
};

}
}

#endif  // _MEMC_PROXY_PROXY_SERVER_
