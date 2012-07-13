#include <boost/thread.hpp>

#include "proxy_server.h"
#include "LogWrapper.h"

using namespace xce::feed;
using namespace MyUtil;

ProxyServer::ProxyServer()
  : work_(io_service_)
  , acceptor_(io_service_, ip::tcp::endpoint(ip::address_v4::any(), 11315))
  , dispatch_threads_(16)
  , dispatch_round_(0)
{
  
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(1));
}

ProxyServer::~ProxyServer() {
  // TODO : 细化 asio 的这些调用
  io_service_.stop();
  for(size_t i = 0; i < dispatch_threads_; ++i) {
    dispatch_services_[i]->stop();
    delete dispatch_services_[i];
  }
}

void ProxyServer::RunConnService(int i){
  MCE_INFO("ProxyServer::RunConnService --> " << i);
  for(;;){
    try {
      MCE_INFO("ProxyServer::RunConnService --> io_service run");
      dispatch_services_[i]->run();
      MCE_INFO("ProxyServer::RunConnService --> io_service run end");
    } catch (std::exception& e) {
      MCE_WARN("ProxyServer::RunConnService --> io_service.run error : " << e.what());
    }catch(Ice::Exception& e){
      MCE_WARN("ProxyServer::RunConnService --> io_service.run error : " << e);
    }catch(...){
      MCE_WARN("ProxyServer::RunConnService --> io_service.run error");
    }
  }
  MCE_INFO("ProxyServer::RunConnService --> end " << i);
}

void ProxyServer::StatTimerHandle(deadline_timer* timer, int index, const boost::system::error_code& error){
  MCE_INFO("###CLIENT " << index << " " << clients_[index].size());
  pool_managers_.at(index)->PrintStatInfo();
  timer->expires_from_now(boost::posix_time::millisec(10*1000));
  timer->async_wait(boost::bind(&ProxyServer::StatTimerHandle, this, timer, index, boost::asio::placeholders::error));
}

void ProxyServer::Run() {
  io_service_.reset();

  for(size_t i = 0; i < dispatch_threads_; ++i) {
    dispatch_services_.push_back(new io_service);
    dispatch_works_.push_back(new io_service::work(*dispatch_services_[i]));
    upconn_pools_.push_back(new UpstreamConnPool());
    pool_managers_.push_back(new PoolManager(*dispatch_services_[i], i));
    clients_.push_back(set<ClientConnection*>());
    
    deadline_timer* timer = new deadline_timer(*dispatch_services_[i]);
    timer->expires_from_now(boost::posix_time::millisec(10*1000));
    timer->async_wait(boost::bind(&ProxyServer::StatTimerHandle, this, timer, i, boost::asio::placeholders::error));
    stat_timers_.push_back(new deadline_timer(*dispatch_services_[i]));
    
  }

  for(size_t i = 0; i < dispatch_threads_; ++i) {
    //boost::thread t(boost::bind(&ProxyServer::RunConnService, dispatch_services_[i]));
    boost::thread t(boost::bind(&ProxyServer::RunConnService, this, i));
    t.detach();
  }

  StartAccept();

  for(;;){
    try {
      MCE_INFO("ProxyServer::Run --> io_service run");
      io_service_.run();
      MCE_INFO("ProxyServer::Run --> io_service run end");
    } catch (std::exception& e) {
      MCE_WARN("ProxyServer::Run --> io_service.run error : " << e.what());
    }catch(Ice::Exception& e){
      MCE_WARN("ProxyServer::Run --> io_service.run error : " << e);
    }catch(...){
      MCE_WARN("ProxyServer::Run --> io_service.run error");
    }
  }
}

void ProxyServer::StartAccept()
{
  ClientConnection* conn = new ClientConnection((int)dispatch_round_, *dispatch_services_[dispatch_round_], pool_managers_[dispatch_round_]);
  if(++dispatch_round_ >= dispatch_threads_) {
    dispatch_round_ = 0;
  }
  acceptor_.async_accept(conn->socket(),
      boost::bind(&ProxyServer::HandleAccept, this, conn,
        boost::asio::placeholders::error));
}

void ProxyServer::HandleAccept(ClientConnection* conn, const boost::system::error_code& error) {
  try{
    if (!error) {
      conn->Start();
    } else {
      MCE_WARN("io_service accept error!");
    }
  }catch(...){
    MCE_WARN("ProxyServer::HandleAccept --> err");
  }
  StartAccept();
}

