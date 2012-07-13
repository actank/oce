#ifndef _MEMCACHED_CLIENT_H_
#define _MEMCACHED_CLIENT_H_

#include <boost/asio.hpp>
#include <string>
#include "../util/MemcProtoParser.h"

using namespace std;
using namespace boost::asio;


enum MemcError{CONNECT_ERR=0, SEND_ERR=1, RECV_ERR=2};

class MemcachedClient;
class MemcDataHandle{
public:
  virtual void OnRecv(char* buf, int len, int keys, const string& key, MemcachedClient* memcli) = 0;
  virtual void OnError(MemcError err, const string& err_desc, MemcachedClient* memcli) = 0;
  virtual void OnEnd(MemcachedClient* memcli) = 0;
  virtual void OnStored(MemcachedClient* memcli) = 0;
  virtual void OnNotStored(MemcachedClient* memcli) = 0;
  virtual void OnDeleted(MemcachedClient* memcli) = 0;
  virtual void OnNotFound(MemcachedClient* memcli) = 0;

  virtual ~MemcDataHandle(){};
};

class MemcachedClientPool;
class MemcachedClient{
public:
  MemcachedClient(io_service& io_service, MemcachedClientPool* owner, const ip::tcp::endpoint& endpoint);
  ~MemcachedClient();
  void Init(CmdType cmd_type, const char* cmd_buf, int len, MemcDataHandle* handle);
  void Init(const string& cmd){};
  void Reset();
  void Request();
  void HandleConnect(const boost::system::error_code& error);
  void AsyncRead();
  void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);
  void HandleRead(const boost::system::error_code& error, size_t bytes_transferred);
  bool ParserData(char* buf, int buf_len, int& data_len, string& key); 

  void WroteClient(int len);

  ip::tcp::endpoint GetEndpoint(){
    return endpoint_;
  }
  float CaculateCost(){
    timeval end;
    gettimeofday(&end, NULL);
    float timeuse=1000000*(end.tv_sec-begin_.tv_sec)+end.tv_usec-begin_.tv_usec;
    timeuse/=1000;
    return timeuse;
  }
private:
  static const int  BASE_SIZE = 64*1024;
  static const int  FACTOR = 2;
  ip::tcp::socket   socket_;
  ip::tcp::endpoint endpoint_;
  char*             recv_buf_;
  int               recv_buf_len_;
  int               cur_recv_len_;
  int               total_recv_len_;
  int               write_len_;
  const char*       cmd_buf_;
  int               cmd_buf_len_;
  MemcDataHandle*   handle_;  
  CmdType           cmd_type_;
  bool              socket_err_;
  timeval           begin_;
  bool              reseting_;
  int               start_time_;
  MemcachedClientPool*     owner_;

  int               recv_values_;
  string            business_type_;
};

#endif
