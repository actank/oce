#include "upstream_conn.h"

#include "LogWrapper.h"

const static size_t kMaxConnPerEndpoint = 64;

namespace xce {
namespace feed {

UpstreamConn::UpstreamConn(boost::asio::io_service& io_service) 
  : popped_bytes_(0)
  , pushed_bytes_(0)
  , socket_(io_service) 
{
}

UpstreamConn * UpstreamConnPool::Pop(const ip::tcp::endpoint & ep){
  UpstreamConn * conn = 0;

  //boost::mutex::scoped_lock(mutex_);
  ConnMap::iterator i = conn_map_.find(ep);
  if ((i != conn_map_.end()) && (!i->second.empty())) {
    conn = i->second.back();
    i->second.pop_back();
    MCE_DEBUG("conn_pool " << ep << " pop. size=" << conn_map_[ep].size());
  }
  return conn;
}

void UpstreamConnPool::Push(const ip::tcp::endpoint & ep, UpstreamConn * conn) {
  if (conn) {
    //boost::mutex::scoped_lock(mutex_);

    ConnMap::iterator i = conn_map_.find(ep);
    if (i != conn_map_.end() && conn_map_.size() >= kMaxConnPerEndpoint){
      conn->socket().close();
      delete conn;
    } else {
      conn->Reset();
      conn_map_[ep].push_back(conn);
      MCE_DEBUG("conn_pool " << ep << " push. size=" << conn_map_[ep].size());
    }
  }
}

}
}

