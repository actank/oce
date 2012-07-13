#include "MemcachedClient.h"
#include "IceLogger.h"
#include "feed_memc_client.h"


int GetLocalPort(int sock){
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  getsockname(sock, (sockaddr*)&addr, &len);
  return ntohs(addr.sin_port);
  //addr.sin_family = AF_INET;
  //addr.sin_addr.s_addr = inet_addr(ip_.c_str());
  //addr.sin_port = htons(port_);
  //MCE_INFO("MemcachedClient::CheckConnect --> connect, " << ip_ << " " << port_);
 
}

MemcachedClient::MemcachedClient(const string& ip, int port, int timeout){
  ip_ = ip;
  port_ = port;
  timeout_ = timeout;
  connected_ = false;
  recv_buf_len_ = 1024*1024;
  send_buf_len_ = 1024*1024;
  cur_recv_len_ = 0;
  total_recv_len_ = 0;
  keys_ = 0;
  sockfd_ = -1;
  //sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
  recv_buf_ = new char[recv_buf_len_];
  send_buf_ = new char[send_buf_len_];
  MCE_INFO("MemcachedClient::MemcachedClient --> " << this << " ip:" << ip_ << " port:" << port_);
}

MemcachedClient::~MemcachedClient(){
  MCE_INFO("MemcachedClient::~MemcachedClient --> " << this << " ip:" << ip_ << " port:" << port_);
  close(sockfd_);
  delete recv_buf_;
  delete send_buf_;
}

bool MemcachedClient::Get(const vector<string>& keys, map<string, string>& resdata, map<string, int>& flags){
  int pos = 0;
  memset(send_buf_, ' ', send_buf_len_);
  memcpy(send_buf_, "get", 3);
  pos += 3;
  for(size_t i=0; i<keys.size(); i++){
    if(pos >= send_buf_len_){
      MCE_WARN("MemcachedClient::Get --> not enough send buffer");
      return false;
    }
    pos++;
    memcpy(send_buf_+pos, keys[i].c_str(), keys[i].size());
    pos += keys[i].size();
  }
  memcpy(send_buf_+pos, "\r\n", 2);
  pos+=2;
  send_buf_[pos] = '\0';
  CmdType t;
  first_key_ = keys.at(0);
  keys_ = keys.size();
  //MCE_INFO("MemcachedClient::Get --> " << this << " " << first_key_ << " ip:" << ip_ << " port:" << GetLocalPort(sockfd_));
  bool res = Request(send_buf_, pos, t, resdata, flags);
  if(!res || t==CMD_NOTDEF){
    connected_ = false; 
  }
  return true;
}

int MemcachedClient::Set(const string& key, const string& value, int flags, int timeout){
  return Set(key, value.c_str(), value.size(), flags); 
}

int MemcachedClient::Set(const string& key, const char* value, int len, int flags, int timeout){
  memset(send_buf_, 0, send_buf_len_);
  sprintf(send_buf_, "set %s %d %d %d\r\n", key.c_str(), flags, timeout, len);
  int pos = strlen(send_buf_);
  memcpy(send_buf_+pos, value, len);
  pos += len;
  memcpy(send_buf_+pos, "\r\n", 2);
  pos += 2;
  map<string, string> resdata; 
  map<string, int> ff;
  CmdType t;
  bool res = Request(send_buf_, pos, t, resdata, ff);
  //printf("request %d %d %d\n", res, pos, t);
  if(!res || t==CMD_NOTDEF){
    connected_ = false;
    return -1;
  }
  if(t==CMD_STORED){
    return 1;
  }else{
    return 0;
  }
}

int MemcachedClient::Append(const string& key, const string& value, int flags){
  return Append(key, value.c_str(), value.size(), flags); 
}

int MemcachedClient::Append(const string& key, const char* value, int len, int flags){
  memset(send_buf_, 0, send_buf_len_);
  sprintf(send_buf_, "append %s %d 0 %d\r\n", key.c_str(), flags, len);
  int pos = strlen(send_buf_);
  memcpy(send_buf_+pos, value, len);
  pos += len;
  memcpy(send_buf_+pos, "\r\n", 2);
  map<string, string> resdata; 
  map<string, int> ff; 
  pos += 2;
  CmdType t;
  bool res = Request(send_buf_, pos, t, resdata, ff);
  //printf("request %d %d %s\n", res, pos, send_buf_);
  if(!res || t==CMD_NOTDEF){
    connected_ = false;
    return -1; 
  }
  if(t==CMD_STORED){
    return 1;
  }else{
    return 0;
  }
}

int MemcachedClient::Delete(const string& key){
  memset(send_buf_, 0, send_buf_len_);
  sprintf(send_buf_, "delete %s 0\r\n", key.c_str());
  int pos = strlen(send_buf_);
  map<string, string> resdata; 
  map<string, int> ff; 
  CmdType t;
  bool res = Request(send_buf_, pos, t, resdata, ff);
  //printf("request %d %d %s\n", res, pos, send_buf_);
  if(!res || t==CMD_NOTDEF){
    connected_ = false; 
  }
  if(t==CMD_DELETED){
    return 1;
  }else{
    return 0;
  }
}

void MemcachedClient::SetSocketOpt(){
  int buf_len = 1024*1024;
  int res = setsockopt(sockfd_, SOL_SOCKET, SO_RCVBUF, &buf_len, sizeof(buf_len));
  if(res!=0){
    MCE_WARN("MemcachedClient::Request --> set recvbuf err, " << strerror(errno));
    return;
  }
  int flags = fcntl(sockfd_, F_GETFL, 0);
  res = fcntl(sockfd_, F_SETFL, flags | O_NONBLOCK);
  if(res < 0){
  }
  
  //MCE_INFO("MemcachedClient::SetSocketOpt --> set linger");
  struct linger l;
  l.l_onoff = 1;
  l.l_linger = 0;
  res = setsockopt(sockfd_, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
  if(res!=0){
    MCE_WARN("MemcachedClient::SetSocketOpt --> set linger err, " << strerror(errno) << " " << ip_ << " " << port_);
  }

  int nodelay = 1;
  //MCE_INFO("MemcachedClient::SetSocketOpt --> set nodelay");
  res = setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
  if(res!=0){
    MCE_WARN("MemcachedClient::SetSocketOpt --> set nodelay err, " << strerror(errno));
  }
}

bool MemcachedClient::HandleRead(CmdType& t, map<string, string>& resdata, map<string, int>& flags){
  t = CMD_NOTDEF;
  while(true){
    int tmp = recv(sockfd_, recv_buf_+cur_recv_len_, recv_buf_len_-cur_recv_len_, 0);
    if(tmp==-1){
      if(errno!=EAGAIN){
        return false;
      }else{
        return true;
      }
    }else if(tmp==0){
      MCE_WARN("MemcachedClient::HandleRead --> closed by peer");
      return false;
    }
    cur_recv_len_ += tmp;
    total_recv_len_ += tmp;
    bool succ = FetchData(resdata, flags, t);
    if(!succ){
      return false;
    }
    if(t!=CMD_VALUE){
      return true;
    }
  }
  return true;
}

bool MemcachedClient::CheckConnect(){
  if(connected_){
    return true;
  }
  if(sockfd_ != -1){
    close(sockfd_);
  }
  sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
  SetSocketOpt();
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip_.c_str());
  addr.sin_port = htons(port_);
  MCE_INFO("MemcachedClient::CheckConnect --> connect, " << ip_ << " " << port_);
  int res = connect(sockfd_, (sockaddr*)&addr, sizeof(addr));
  if(res == 0){
    connected_ = true;
    return true;
  }else if(res<0 && errno != EINPROGRESS){
    MCE_INFO("MemcachedClient::CheckConnect --> err, " << res << " " << strerror(errno));
    return false;
  }
  struct pollfd fds;
  fds.fd=sockfd_; 
  fds.events=POLLOUT;
  res = poll(&fds, 1, timeout_);
  if(res==1){
    connected_ = true;
    return true;
  }
  MCE_INFO("MemcachedClient::CheckConnect --> err, " << res << " " << strerror(errno));
  return false;
}

bool MemcachedClient::Send(const char* buf, int len){
  int tmp = 0;
  int count = 0;
  while(count < 10){
    int res = send(sockfd_, buf+tmp, len-tmp, 0);
    if(res < 0){
      MCE_WARN("MemcachedClient::Send --> err, " << res << " " << strerror(errno));
      return false;
    }
    tmp += res;
    if(tmp == len){
      //printf("MemcachedClient::Send --> %d %d\n", count, tmp);  
      return true;
    }
    struct pollfd fds;
    fds.fd=sockfd_;
    fds.events=POLLOUT;
    res = poll(&fds, 1, timeout_);
    if(res!=1){
      //printf("MemcachedClient::Send --> %d, %s\n", res, strerror(errno));
      return false;
    }
    count++;
  }
  return false;
}

bool MemcachedClient::Request(const char* buf, int len, CmdType& t, map<string, string>& resdata, map<string, int>& flags){
  t = CMD_NOTDEF;
  cur_recv_len_ = 0;
  total_recv_len_ = 0;
  res_.clear();
  int retry = 2;
  
  //服务端重启的情况下，会导致send失败,需要重新链接再send
  while(retry-- > 0){
    if(!CheckConnect()){
      return false;
    }
    if(Send(buf, len)){
      break;
    }else{
      connected_ = false;
      continue;
    }
  }
  int pos = 0;
  bool suc = false;
  int count = 0;
  while(true){
    struct pollfd fds;
    fds.fd=sockfd_;
    fds.events=POLLIN;
    int res = poll(&fds, 1, timeout_);
    if(res <= 0){
      //printf("@@@@@@@@3 %d\n", count);
      if(res < 0){
        MCE_WARN("MemcachedClient::Request --> poll err, " << res << " " << strerror(errno));
      }else{
        MCE_INFO("MemcachedClient::Request --> poll timeout, " << res << " " << strerror(errno));
      }
      return false;
    }else if(res==1){
      bool s = HandleRead(t, resdata, flags);
      //printf("HandleRead %d %d\n", s, t);
      if(!s){
        //printf("@@@@@@@@4\n");
        return false;
      }
      if(t==CMD_VALUE || t==CMD_NOTDEF){
        count++;
        continue;
      }else if(t==CMD_END || t==CMD_STORED || t==CMD_NOT_STORED || t==CMD_DELETED || t==CMD_NOT_FOUND){
        return true; 
      }else{
        MCE_WARN("MemcachedClient::Request --> parser err " << t);
        return false;
      }
    }
    count++;
  }
  //printf("@@@@@@@@ 2222\n");
  MCE_WARN("MemcachedClient::Request --> parser err3 " << t);
  return false;
}

bool MemcachedClient::FetchData(map<string, string>& resdata, map<string, int>& flags, CmdType& type){
  int total_len = 0;
  type = CMD_NOTDEF;
  while(cur_recv_len_ - total_len > 0){
    type = CMD_NOTDEF;
    MemCmdHead head = MemcProtoParser::ParserHead(recv_buf_ + total_len, cur_recv_len_-total_len);
    type = head.type_;
    if(head.succ_ == PARSER_ERR){
      MCE_WARN("MemcachedClient::FetchData --> parser err, " << first_key_ << " keys:" << keys_ << " total_recv:" << total_recv_len_ << " res:" << resdata.size());
      return false;
    }else if(head.succ_ == PARSER_MORE){
      //MCE_INFO("MemcachedClient::FetchData --> need more");
      break;
    }
    if(head.type_==CMD_END || head.type_==CMD_STORED || head.type_==CMD_NOT_STORED || head.type_==CMD_DELETED || head.type_==CMD_NOT_FOUND){
      //MCE_INFO("MemcachedClient::FetchData --> get end " << this << " " << first_key_  << " ip:" << ip_ << " port:" << GetLocalPort(sockfd_));
      break;
    }else if(head.type_ == CMD_VALUE){
      total_len += head.data_len_;
      string key(head.key_, head.key_len_);
      resdata[key].insert(0, head.body_, head.body_len_);
      flags[key] = head.flag_; 
    }else{
      MCE_WARN("MemcachedClient::FetchData --> parser cmdtype err");
      return false;
    }
  }
  if(total_len > 0){
    memmove(recv_buf_, recv_buf_ + total_len, cur_recv_len_ - total_len);
    cur_recv_len_ -= total_len;
  }
  return true;
}

/*
#include "RFeed.h"
#include <stdio.h>
using namespace xce::feed;


void get(MemcachedClient& cli, const char* key){
  //printf("---------------\n");
  vector<string> ids;
  ids.push_back(key);
  map<string, string> res;
  map<string, int> fs;
  cli.Get(ids, res, fs);
  printf("recv %d\n", res.size());
  map<string, string>::iterator it = res.begin();
  for(; it!=res.end(); ++it){
    FeedItem* p = (FeedItem*)it->second.c_str();
    int count = it->second.size()/32;
    for(int i=0; i<count; i++){
      FeedItem item = *p;
      printf("%lld %lld %d %d %d %d\n", item.feed, item.merge, item.type, item.time, item.weight, item.actor);
      p++;
    }
  } 
}
int main(int argc, char* argv[]){
  MemcachedClient cli("10.3.17.90", 11315, 100);
  FeedItem item;
  item.feed = -9218857826;
  //int res = cli.Append(string(argv[1]), (char*)&item, sizeof(item), 0);
  //printf("delete %d\n", res);
  get(cli, argv[1]);
  return 0;
  //MemcachedClient cli("10.3.20.45", 11214, 100);
  FeedItem item;
  item.feed = 1;
  item.merge = 2;
  item.type = 3;
  item.time = 4;
  item.weight = 5;
  item.actor = 6;
  int res = cli.Set("FEEDLIST#12345", (char*)&item, sizeof(item), 0); 
  printf("set %d\n", res);
  //exit(0);
  get(cli);  
  item.feed = 11;
  item.merge = 22;
  item.type = 33;
  item.time = 44;
  item.weight = 55;
  item.actor = 66;
  cli.Append("FEEDLIST#12345", (char*)&item, sizeof(item), 0); 
  get(cli); 

  item.feed = 111;
  item.merge = 222;
  item.type = 333;
  item.time = 444;
  item.weight = 555;
  item.actor = 666;
  cli.Append("FEEDLIST#12345", (char*)&item, sizeof(item), 0); 
  item.feed = 1111;
  item.merge = 2222;
  item.type = 3333;
  item.time = 4444;
  item.weight = 5555;
  item.actor = 6666;
  cli.Append("FEEDLIST#12345", (char*)&item, sizeof(item), 0); 
  get(cli); 

  item.feed = -111;
  item.merge = 222;
  item.type = 333;
  item.time = 444;
  item.weight = 555;
  item.actor = 666;
  cli.Append("FEEDLIST#12345", (char*)&item, sizeof(item), 0);
  get(cli);

  res = cli.Delete("FEEDLIST#12345");
  printf("del %d\n", res);
  res = cli.Delete("FEEDLIST#12345");
  printf("del %d\n", res);
  get(cli);
}*/
