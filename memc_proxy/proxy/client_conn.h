#ifndef _MEMC_PROXY_CLIENT_CONN_
#define _MEMC_PROXY_CLIENT_CONN_

#include <queue>
#include <set>
#include <string>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost::asio;
using namespace std;

namespace xce {
namespace feed {

class UpstreamConnPool;
class MemcCommand;

struct ResponseStatus {
  ResponseStatus() : unparsed_bytes(0), left_bytes(0), complete(false) {}

  void Reset() {
    unparsed_bytes = left_bytes = 0;
    complete = false;
  }
  
  // 当前 VALUE 块的未解析数据字节数(因为收到数据的长度原因, 无法解析)
  size_t unparsed_bytes;

  // 当前 VALUE 块的接收数据字节数(因为buffer的长度原因, 未能收到全部数据)
  size_t left_bytes;

  // 当前 MemcCommand 的数据已经全部收到(不只是某个VALUE块全收到)
  bool complete;
};

class ClientConnection  : public boost::enable_shared_from_this<ClientConnection> 
{
public:
  ClientConnection(boost::asio::io_service& io_service, UpstreamConnPool * pool);
  ~ClientConnection();

  ip::tcp::socket& socket() {
    return socket_;
  }

  UpstreamConnPool * upconn_pool() {
    return upconn_pool_;
  }

  void Start();

  void HandleForwardAllData(boost::shared_ptr<MemcCommand> cmd, size_t bytes);
  void HandleForwardMoreData(boost::shared_ptr<MemcCommand> cmd, size_t bytes);

  void OnCommandReady(boost::shared_ptr<MemcCommand> memc_cmd);
  void OnCommandError(boost::shared_ptr<MemcCommand> memc_cmd, const boost::system::error_code& error);

private:
  ResponseStatus  response_status_;

  boost::asio::io_service& io_service_;
  ip::tcp::socket socket_;

  enum { kBufLength = 64 * 1024};
  char up_buf_[kBufLength];
  size_t up_buf_begin_, up_buf_end_;

  UpstreamConnPool * upconn_pool_;

  boost::shared_ptr<MemcCommand> current_ready_cmd_;
  queue< boost::shared_ptr<MemcCommand> > ready_cmd_queue_;
  set< boost::shared_ptr<MemcCommand> > fetching_cmd_set_;
  // 当前命令被分拆成的子命令的个数
  size_t mapped_cmd_count_;

  size_t timeout_;
  boost::asio::deadline_timer timer_;

  void AsyncRead();
  void AsyncWrite();
  void AsyncWriteMissed();

  void HandleRead(const boost::system::error_code& error, size_t bytes_transferred);
  void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);
  void HandleWriteMissed(const boost::system::error_code& error, size_t bytes_transferred);

  void HandleMemcCommandTimeout(const boost::system::error_code& error);
  void HandleTimeoutWrite(const boost::system::error_code& error);

  int MapMemcCommand(char * buf, size_t len);
};

typedef boost::shared_ptr<ClientConnection> ClientConnectionPtr;

}
}

#endif // _MEMC_PROXY_CLIENT_CONN_
