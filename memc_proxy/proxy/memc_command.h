#ifndef _MEMC_PROXY_MEMC_COMMAND_
#define _MEMC_PROXY_MEMC_COMMAND_

#include <set>
#include <vector>
#include <string>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <IceLogger.h>

using namespace std;
using namespace boost::asio;

namespace xce {
namespace feed {

class UpstreamConn;
class ClientConnection;

class MemcCommand : public boost::enable_shared_from_this<MemcCommand> {
public:
  MemcCommand(boost::asio::io_service& io_service, const ip::tcp::endpoint & ep, 
      boost::shared_ptr<ClientConnection> owner, const char * buf, size_t cmd_len);
  ~MemcCommand();

  void ForwardData(char * buf, size_t bytes);
  void AsyncRead();
  void Abort();

  // response_cmd_line
  const string & cmd_line() const {
    return cmd_line_;
  }
  size_t cmd_line_bytes() const {
    return cmd_line_.size();
  }
  bool cmd_line_forwarded() {
    return cmd_line_forwarded_;
  }
  void set_cmd_line_forwarded(bool b) {
    cmd_line_forwarded_ = b;
  }

  size_t body_bytes() const {
    return body_bytes_;
  }
  size_t total_bytes() const {
    return cmd_line_bytes() + body_bytes();
  }
  size_t forwarded_bytes() const {
    return forwarded_bytes_;
  }

  const string & method() const {
    return method_;
  }

  string & missed_buf() {
    return missed_buf_;
  }

  bool missed_ready() const {
    return missed_ready_;
  }

  const ip::tcp::endpoint & upstream_endpoint() const {
    return upstream_endpoint_;
  }

  UpstreamConn * upstream_conn() {
    return upstream_conn_;
  }

  bool NeedLoadMissed();

  void set_upstream_conn(UpstreamConn * conn) {
    upstream_conn_ = conn;
  }

  void RemoveMissedKey(const string & key);
  void LoadMissedKeys();
  void DispatchMissedKeyData();
  friend class ClientConnection;
private:
  void HandleConnect(char * buf, size_t bytes, const boost::system::error_code& error);
  void HandleWrite(char * buf, size_t buf_offset, const size_t bytes, const boost::system::error_code& error, size_t bytes_transferred);
  void HandleRead(const boost::system::error_code& error, size_t bytes_transferred);
  void HandleMissedKeyReady();

  string cmd_line_;
  string method_;

  bool cmd_line_forwarded_;
  size_t forwarded_bytes_;
  size_t body_bytes_;

  vector<string> missed_keys_;
  bool missed_ready_;
  string missed_buf_;
  size_t missed_popped_bytes_;
  boost::asio::deadline_timer * missed_timer_;

  ip::tcp::endpoint upstream_endpoint_;
  UpstreamConn * upstream_conn_;
  boost::shared_ptr<ClientConnection> client_conn_;

  boost::asio::io_service& io_service_;

  timeval time_created_, time_ready_, time_loaded_;
  bool loaded_;
};

typedef boost::shared_ptr<MemcCommand> MemcCommandPtr;

}
}

#endif //_MEMC_PROXY_MEMC_COMMAND_
