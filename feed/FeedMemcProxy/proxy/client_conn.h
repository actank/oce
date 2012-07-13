#ifndef _MEMC_PROXY_CLIENT_CONN_
#define _MEMC_PROXY_CLIENT_CONN_

#include <queue>
#include <set>
#include <string>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include "MemcachedClientPool.h"

using namespace boost::asio;
using namespace std;

namespace xce {
namespace feed {

class UpstreamConnPool;
//class MemcCommand;

class ClientConnection : public MemcDataHandle
{
public:
  ClientConnection(int index, boost::asio::io_service& io_service, PoolManager* poolmgr);
  ~ClientConnection();

  ip::tcp::socket& socket() {
    return socket_;
  }
  
  virtual void OnRecv(char* buf, int len, int keys, const string& key, MemcachedClient* memcli);
  virtual void OnError(MemcError err, const string& err_desc, MemcachedClient* memcli);
  virtual void OnEnd(MemcachedClient* memcli);
  virtual void OnStored(MemcachedClient* memcli);
  virtual void OnNotStored(MemcachedClient* memcli);
  virtual void OnDeleted(MemcachedClient* memcli);
  virtual void OnNotFound(MemcachedClient* memcli);


  bool CheckValid(int index);
  void CheckRead(bool resetbuf = false); 
  vector<string> GetKeys(const string& cmd); 
  void ParserGetCmd(MemCmdHead* head);
  void ParserSetCmd(MemCmdHead* head);
  void Start();
  void Close(const string& log);

  void Write(const char* buf, int len);
  void AsyncWrite(const char* buf, int len, MemcachedClient* memcli);

  int GetIndex(){
    return index_;
  }
  PoolManager* GetPoolManager(){
    return pool_manager_;
  }
private:
  static const int BASE_BUF_LEN = 64*1024;
  static const int FACTOR = 2;
  bool        deleted_;
  bool        closed_;
  bool        wrote_end_;
  char*       tmp_;
  char*       recv_buf_;
  int         recv_buf_len_;
  int         cur_recv_len_;
  int         total_key_;
  CmdType     req_type_;
  int         req_keys_;
  int         res_keys_;
  int         write_bytes_;
  //set<string> missed_keys_;
  timeval     begin_time_;
  //int id_count_;
  //int req_write_len_;
  //int wrote_len_;
  string      key_type_;
  string      cli_ip_;
  int         cli_port_;
  int index_;
//  ResponseStatus  response_status_;

  boost::asio::io_service& io_service_;
  ip::tcp::socket socket_;
  int started_time_;
  enum { kBufLength = 256 * 1024,MAX_WAIT_TIME=80};

  PoolManager* pool_manager_;

  map<MemcachedClient*, string>   up_clis_;
  // 当前命令被分拆成的子命令的个数
  //size_t mapped_cmd_count_;

  size_t first_timeout_;
  //size_t second_timeout_;
  //size_t rest_wait_time_;

  boost::asio::deadline_timer timer_;

  //size_t write_cmd_bytes;
  //size_t write_cmd_poped_bytes;

  void AsyncRead();
  void HandleRead(int index, const boost::system::error_code& error, size_t bytes_transferred);
  void HandleWrite(int index, MemcachedClient* memcli, const boost::system::error_code& error, size_t bytes_transferred);
  void HandleMemcCommandTimeout(int index, const boost::system::error_code& error);
};

typedef boost::shared_ptr<ClientConnection> ClientConnectionPtr;

}
}

#endif // _MEMC_PROXY_CLIENT_CONN_
