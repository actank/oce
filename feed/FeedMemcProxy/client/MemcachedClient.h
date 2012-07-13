#ifndef MEMCACHED_CLIENT
#define MEMCACHED_CLIENT

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>
#include "../util/MemcProtoParser.h"

using namespace std;

enum MemcError{CONNECT_ERR=0, SEND_ERR=1, RECV_ERR=2};
enum EState{ERR=0, END=1, MORE=2};

class MemcachedClient{
public:
  //vector<string> Get(vector<keys>);
  MemcachedClient(const string& ip, int port, int timeout);
  ~MemcachedClient();
  bool Get(const vector<string>& keys, map<string, string>& resdata, map<string, int>& flags);
  int  Set(const string& key, const string& value, int flags, int tiemout=0);
  int  Set(const string& key, const char* value, int len, int flags, int timeout=0);
  int  Append(const string& key, const string& value, int flags);
  int  Append(const string& key, const char* value, int len, int flags);
  int  Delete(const string& key);

  bool Connected(){
    return connected_;
  }
  string GetIp(){
    return ip_;
  }
  int GetPort(){
    return port_;
  }
protected:
  bool    Request(const char* buf, int len, CmdType& t, map<string, string>& resdata, map<string, int>& flags);
  bool    HandleRead(CmdType& t, map<string, string>& resdata, map<string, int>& flags);
  bool    CheckConnect();
  bool    Send(const char* buf, int len);
  void    SetSocketOpt();
  bool    FetchData(map<string, string>& resdata, map<string, int>& flags, CmdType& type);
private:
  string  ip_;
  int     port_;
  int     timeout_;
  int     sockfd_;
  bool    connected_;
  char*   recv_buf_;
  int     recv_buf_len_;
  int     cur_recv_len_;
  int     total_recv_len_;
  int     keys_;
  char*   send_buf_;
  int     send_buf_len_;
  map<string, string> res_;
  string  first_key_;
  int     local_port_;
};
#endif
