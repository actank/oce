#include <boost/thread.hpp>

#include "proxy_server.h"
#include "LogWrapper.h"

using namespace xce::feed;
using namespace MyUtil;

ProxyServer::ProxyServer(const ip::tcp::endpoint & ep)
  : work_(io_service_)
  , acceptor_(io_service_, ep)
  , dispatch_threads_(32)
  , dispatch_round_(0)
{
}

ProxyServer::~ProxyServer() {
  // TODO : 细化 asio 的这些调用
  io_service_.stop();
  for(size_t i = 0; i < dispatch_threads_; ++i) {
    dispatch_services_[i]->stop();
    delete dispatch_services_[i];
  }
}

void ProxyServer::Run() {
  io_service_.reset();

  for(size_t i = 0; i < dispatch_threads_; ++i) {
    dispatch_services_.push_back(new io_service);
    dispatch_works_.push_back(new io_service::work(*dispatch_services_[i]));
    upconn_pools_.push_back(new UpstreamConnPool());
  }

  for(size_t i = 0; i < dispatch_threads_; ++i) {
    boost::thread t(boost::bind(&io_service::run, dispatch_services_[i]));
    t.detach();
  }

  StartAccept();

  try {
    io_service_.run();
  } catch (std::exception& e) {
    MCE_WARN("io_service.run error : " << e.what());
  }
}

void ProxyServer::StartAccept()
{
  ClientConnectionPtr conn(new ClientConnection(*dispatch_services_[dispatch_round_], upconn_pools_[dispatch_round_]));
  if(++dispatch_round_ >= dispatch_threads_) {
    dispatch_round_ = 0;
  }

  MCE_DEBUG("ClientConnection created.");

  acceptor_.async_accept(conn->socket(),
      boost::bind(&ProxyServer::HandleAccept, this, conn,
        boost::asio::placeholders::error));
}

void ProxyServer::HandleAccept(ClientConnectionPtr conn, const boost::system::error_code& error) {
  if (!error) {
    conn->Start();
    StartAccept();
  } else {
    MCE_DEBUG("io_service accept error!");
  }
}

