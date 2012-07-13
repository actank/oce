#ifndef _MEMC_PROXY_UPSTREAM_CONN_
#define _MEMC_PROXY_UPSTREAM_CONN_

#include <map>
#include <string>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace std;

namespace xce {
namespace feed {

class UpstreamConn {
public:
  UpstreamConn(boost::asio::io_service& io_service);

  void Reset() {
    popped_bytes_ = pushed_bytes_ = 0;
  }

  ip::tcp::socket & socket() {
    return socket_;
  }

  enum { BUFFER_SIZE = 5*1024 * 1024};
  char buf_[BUFFER_SIZE];

  size_t popped_bytes_;
  size_t pushed_bytes_;
private:
  ip::tcp::socket socket_;
};

class UpstreamConnPool {
private:
  typedef map<ip::tcp::endpoint, vector<UpstreamConn*> > ConnMap;
  ConnMap conn_map_;
public:
  //static UpstreamConnPool & instance() {
  //  static UpstreamConnPool p; 
  //  return p;
  //}

  //boost::mutex mutex_;

  UpstreamConn * Pop(const ip::tcp::endpoint & ep);
  void Push(const ip::tcp::endpoint & ep, UpstreamConn * conn);
};

}
}


#endif //_MEMC_PROXY_UPSTREAM_CONN_
