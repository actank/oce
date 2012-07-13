#include "client_conn.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

//#include "memc_command.h"
#include "util.h"
#include "LogWrapper.h"

#include "key_mapping.h"
#include "proxy_server.h"
//#include "upstream_conn.h"
//#include "PerformanceStat.h"

using namespace boost::asio;
using namespace std;
using namespace MyUtil;

namespace xce {
namespace feed {

int g_cc_count = 0;

float DiffTime(const timeval& begin, const timeval& end){
    float timeuse=1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
    timeuse/=1000;
    return timeuse;
}

ClientConnection::ClientConnection(int index, boost::asio::io_service& io_service, PoolManager* poolmgr)
  : index_(index)
  , io_service_(io_service)
  , socket_(io_service)
  , first_timeout_(50)
  , timer_(io_service)
  , pool_manager_(poolmgr)
{
  closed_ = false;
  deleted_ = false;
  wrote_end_ = false;
  recv_buf_len_ = 0;
  cur_recv_len_ = 0;
  recv_buf_ = 0 ;
  MCE_INFO("ClientConnection::ClientConnection --> " << this);
}

ClientConnection::~ClientConnection() {
  MCE_INFO("ClientConnection::~ClientConnection --> " << this);
}

void ClientConnection::Close(const string& log) {
  MCE_INFO("ClientConnection::Close --> " << index_ << " " << this << " sock:" << socket_.native() 
          << " dur:" << (time(NULL) - started_time_) << " " << log << " closed:" << closed_ << " deleted:" << deleted_);
  if(socket_.native() == -1){
    MCE_WARN("ClientConnection::Close --> has been closed " << this);
    return;
  }
  closed_ = true;
  timer_.cancel();
  socket_.close();
  if(recv_buf_){
    delete recv_buf_;
    recv_buf_ = 0;
  }
  map<MemcachedClient*, string>::iterator it = up_clis_.begin();
  for(; it!=up_clis_.end(); ++it){
    it->first->Reset();
    MCE_INFO("ClientConnection::Close --> reseting " << it->first);
  }
  ProxyServer::instance().EraseClient(index_, this);
  delete this;
}
void ClientConnection::Start() {
  try{
    ip::tcp::no_delay nodelay(true);
    socket_.set_option(nodelay);

    boost::asio::socket_base::linger linger(true, 0);
    socket_.set_option(linger);

    boost::asio::socket_base::send_buffer_size send_buf_size(1024*1024);
    socket_.set_option(send_buf_size);
    
    boost::asio::socket_base::send_buffer_size opt;
    socket_.get_option(opt);
    int size = opt.value();


    char tmp[64] = {0};
    sprintf(tmp, "Connected%d", index_);
    cli_ip_ = socket_.remote_endpoint().address().to_v4().to_string();
    cli_port_ = socket_.remote_endpoint().port();
    MCE_INFO("ClientConnection::Start --> started " << index_ << " " << socket_.remote_endpoint().address().to_v4().to_string() << " " << this << " sndbuf:" << size);
    started_time_ = time(NULL);
    ProxyServer::instance().PushClient(index_, this);
    CheckRead();
  }catch(std::exception& e){
    MCE_WARN("ClientConnection::Start --> err, " << e.what());
  }
}

void ClientConnection::AsyncRead()
{
  timer_.cancel();
  //MCE_INFO("ClientConnection::AsyncRead --> " << this << " sock:" << socket_.native() << " "
  //    << (Ice::Long)recv_buf_ << " " << (Ice::Long)tmp_ << " "<< recv_buf_len_ << " " << cur_recv_len_);
  tmp_ = recv_buf_ + cur_recv_len_;
  socket_.async_read_some(boost::asio::buffer(recv_buf_ + cur_recv_len_, recv_buf_len_ - cur_recv_len_),
          boost::bind(&ClientConnection::HandleRead, this, index_,
          boost::asio::placeholders::error, // 占位符
          boost::asio::placeholders::bytes_transferred));
}

vector<string> ClientConnection::GetKeys(const string& cmd){
  string sub = cmd.substr(4, cmd.size()-6);
  std::vector<std::string> keys;
  boost::split(keys, sub, boost::is_any_of(" "), boost::token_compress_on);
  return keys;
}

void ClientConnection::ParserSetCmd(MemCmdHead* head){
  string key(head->key_, head->key_len_);
  key_type_ = key; 
  ip::tcp::endpoint ep = GetEndpointByKey(key.c_str(), key.size());
  MemcachedClient* cli = pool_manager_->GetClient(ep);
  up_clis_.insert(make_pair<MemcachedClient*, string>(cli, key));
  cli->Init(head->type_, recv_buf_, cur_recv_len_, this);
  cli->Request();
  MCE_INFO("ClientConnection::ParserSetCmd --> " << key << " type:" << head->type_ << " datalen:" << head->body_len_);
}

void ClientConnection::ParserGetCmd(MemCmdHead* head) {
  char* buf = head->key_;
  int len = head->key_len_;
  std::vector<std::string> keys;
  string key_list(buf, len);
  boost::split(keys, key_list, boost::is_any_of(" "), boost::token_compress_on);

  map<ip::tcp::endpoint, string> cmd_lines;
  for (size_t i = 0; i < keys.size(); ++i) {
    if (keys[i].empty()) {
      continue;
    }
    if(key_type_.empty()){
      key_type_ = keys[i];
    }
    ip::tcp::endpoint ep = GetEndpointByKey(keys[i].c_str(), keys[i].size());
    //missed_keys_.insert(keys[i]);
    total_key_++;
    req_keys_++;
    map<ip::tcp::endpoint, string>::iterator it = cmd_lines.find(ep);
    if (it == cmd_lines.end()) {
      it = cmd_lines.insert(make_pair(ep, string("get"))).first;
    }
    it->second += ' ';
    it->second += keys[i];
  }
  map<ip::tcp::endpoint, string>::iterator it = cmd_lines.begin();
  for(; it!=cmd_lines.end(); ++it){
    it->second += "\r\n"; 
    MemcachedClient* cli = pool_manager_->GetClient(it->first);
    up_clis_.insert(make_pair<MemcachedClient*, string>(cli, it->second));
    cli->Init(head->type_, it->second.c_str(), it->second.size(), this);
    cli->Request();
  }
}

void ClientConnection::CheckRead(bool resetbuf){
  if(resetbuf){
    total_key_ = 0;
    cur_recv_len_ = 0;
    if(recv_buf_len_ != BASE_BUF_LEN){
      delete recv_buf_;
      recv_buf_len_ = BASE_BUF_LEN;
      recv_buf_ = new char[recv_buf_len_];
      MCE_INFO("ClientConnection::CheckRead --> reset buf");
    }
  }else if(recv_buf_len_ == 0){ 
    total_key_ = 0;
    cur_recv_len_ = 0;
    recv_buf_len_ = BASE_BUF_LEN;
    recv_buf_ = new char[recv_buf_len_];
  }else if(cur_recv_len_ == recv_buf_len_){
    char* p = recv_buf_;
    recv_buf_len_ *= FACTOR;
    recv_buf_ = new char[recv_buf_len_];
    memset(recv_buf_, 0, recv_buf_len_);
    memcpy(recv_buf_, p, cur_recv_len_);
    MCE_INFO("ClientConnection::CheckRead --> new a more large buf " << (Ice::Long)recv_buf_ << " " << (Ice::Long)p << " "
    << recv_buf_len_ << " " << cur_recv_len_);
    delete p;
  } 
  AsyncRead();
}

/*bool CheckValid(char* buf, int len){
  for(int i=0; i<len; i++){
    if(buf[i]!=0){
      return true;
    }
  }
  return false;
}*/

void ClientConnection::HandleWrite(int index, MemcachedClient* memcli, const boost::system::error_code& error, size_t bytes_transferred){
  if(!CheckValid(index)){
    return;
  }
  if(error){
    MCE_WARN("ClientConnection::HandleWrite --> " << this << " " << error.message());
    Close(error.message());
    return;
  }
  if(memcli && up_clis_.find(memcli)!=up_clis_.end()){
    memcli->WroteClient(bytes_transferred);
  }
}

void ClientConnection::HandleRead(int index, const boost::system::error_code& error, size_t bytes_transferred)
{
  if(!CheckValid(index)){
    MCE_WARN("ClientConnection::HandleRead --> invalid client " << this);
    return;
  }
  if (error) {
    if (error == boost::asio::error::eof) {
      Close("closed by client"); 
    } else {
      Close(error.message());
      /*if(!closed_){
        Close(error.message());
      }else if(!deleted_){
        MCE_INFO("ClientConnection::HandleRead --> delete connection " << this << " " << error.message()
                  << " closed:" << closed_ << " deleted:" << deleted_);
        deleted_ = true;
        delete this;
      }else{
        MCE_WARN("ClientConnection::HandleRead --> connection err, " << this << " closed:" << closed_ << " deleted:" << deleted_);
      }*/
    }
    return;
  }
  //if(!CheckValid(tmp_, bytes_transferred)){
  //  MCE_INFO("@@@@@@@@@@@@@");
  //}
  cur_recv_len_ += bytes_transferred;
  MemCmdHead head = MemcProtoParser::ParserHead(recv_buf_, cur_recv_len_);
  //MCE_INFO("ClientConnection::HandleRead --> " << this << " buf:" << (Ice::Long)recv_buf_ << " " << (Ice::Long)tmp_ << " len:" << bytes_transferred << " " << head.succ_  << " cur_recv:" << cur_recv_len_ << " " << head.type_ << " " << head.data_len_);
  if(head.succ_==PARSER_ERR){
    MCE_WARN("ClientConnection::HandleRead --> parser err, " << cur_recv_len_ << " " << head.type_); 
  }else if(head.succ_==PARSER_SUC){
    req_keys_ = 0;
    res_keys_ = 0;
    write_bytes_ = 0;
    req_type_ = head.type_;
    if(head.type_ == CMD_GET){
      ParserGetCmd(&head);
    }else if(head.type_==CMD_SET || head.type_==CMD_DELETE || head.type_==CMD_APPEND){
      ParserSetCmd(&head);
    }
    wrote_end_ = false;
  }else{
    CheckRead(); 
    return;
  }
  gettimeofday(&begin_time_, NULL); 
  if (first_timeout_ > 0) {
    timer_.expires_from_now(boost::posix_time::millisec(first_timeout_));
    timer_.async_wait(boost::bind(&ClientConnection::HandleMemcCommandTimeout, this, index_, boost::asio::placeholders::error));
  }
}


void ClientConnection::HandleMemcCommandTimeout(int index, const boost::system::error_code& error) {
  if (error) {
    if (error != boost::asio::error::operation_aborted) {
      MCE_WARN("ClientConnection::HandleMemcCommandTimeout --> " << error << " " << error.message());
    }
    return;
  }
  if(!CheckValid(index)){
    return;
  }
  timer_.cancel();
  timeval end;
  gettimeofday(&end, NULL);
  if(!wrote_end_){
    if(req_type_ == CMD_GET){
      AsyncWrite(MemcProtoParser::END_BUF, MemcProtoParser::END_BUF_LEN, NULL);
    }else if(req_type_ == CMD_SET||req_type_ == CMD_APPEND){
      AsyncWrite(MemcProtoParser::NOT_STORED_BUF, MemcProtoParser::NOT_STORED_BUF_LEN, NULL);
    }else if(req_type_ == CMD_DELETE){
      AsyncWrite(MemcProtoParser::NOT_FOUND_BUF, MemcProtoParser::NOT_FOUND_BUF_LEN, NULL);
    }
    wrote_end_ = true;
  }else{
    MCE_WARN("ClientConnection::HandleMemcCommandTimeout --> had wrote end " << this);
  }
  map<MemcachedClient*, string>::iterator it = up_clis_.begin();
  for(; it!=up_clis_.end(); ++it){
    it->first->Reset();
    MCE_INFO("ClientConnection::HandleMemcCommandTimeout --> timeout, reseting " << it->first);
  }
  MCE_INFO("ClientConnection::HandleMemcCommandTimeout --> write end to client, " << index_ << " reqtype:" << req_type_ << " " 
            << key_type_ << " req:" << req_keys_ <<  " res:" << res_keys_ 
            << " write:" << write_bytes_ << " cost:" << DiffTime(begin_time_, end) << " up:" << up_clis_.size() << " ip:" << cli_ip_ << " port:" << cli_port_);
  up_clis_.clear();
  //missed_keys_.clear(); 
  CheckRead(true);
}

void ClientConnection::AsyncWrite(const char* buf, int len, MemcachedClient* memcli){
  boost::asio::async_write(socket_,
    boost::asio::buffer(buf, len),
    boost::bind(&ClientConnection::HandleWrite, this, index_, memcli,
      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

bool ClientConnection::CheckValid(int index){
  if(index != index_){
    MCE_WARN("ClientConnection::CheckValid --> UNKONW ERROR " << this);
    return false;
  }
  if(!ProxyServer::instance().IsExist(index, this)){
    MCE_WARN("ClientConnection::CheckValid --> invalid " << this);
    return false;
  }
  return true;
}


void ClientConnection::Write(const char* buf, int len) {
  timeval beg, end;
  gettimeofday(&beg, NULL);
  boost::asio::write(socket_, boost::asio::buffer(buf, len));
  gettimeofday(&end, NULL);
  float cost = DiffTime(beg, end);
  if(cost > 1){
    MCE_INFO("write cost " << cost << " len:" << len);
  }
}

void ClientConnection::OnRecv(char* buf, int len, int keys, const string& key, MemcachedClient* memcli){
  //missed_keys_.erase(key);
  AsyncWrite(buf, len, memcli);
  if(buf[len-1]=='\n' && buf[len-2]=='\r' && buf[len-3]=='D' && buf[len-4]=='N' && buf[len-5]=='E'){
    MCE_WARN("ClientConnection::OnRecv --> include END " << this);
  }
  res_keys_ += keys;
  write_bytes_ += len;
}

void ClientConnection::OnError(MemcError err, const string& err_desc, MemcachedClient* memcli){
  up_clis_.erase(memcli);
  if(up_clis_.empty()){
    if(!wrote_end_){
      if(req_type_ == CMD_GET){
        AsyncWrite(MemcProtoParser::END_BUF, MemcProtoParser::END_BUF_LEN, NULL);
      }else if(req_type_ == CMD_SET||req_type_ == CMD_APPEND){
        AsyncWrite(MemcProtoParser::NOT_STORED_BUF, MemcProtoParser::NOT_STORED_BUF_LEN, NULL);
      }else if(req_type_ == CMD_DELETE){
        AsyncWrite(MemcProtoParser::NOT_FOUND_BUF, MemcProtoParser::NOT_FOUND_BUF_LEN, NULL);
      }
      wrote_end_ = true;
    }else{
      MCE_WARN("ClientConnection::OnEnd --> had wrote end " << this);
    }
    cur_recv_len_ = 0;
    timeval end;
    gettimeofday(&end, NULL);
    MCE_INFO("ClientConnection::OnError --> write end to client, " << index_ << " " << " reqtype:" << req_type_ << " "
            << key_type_ << " req:" << req_keys_ << " res:" << res_keys_
            << " write:" << write_bytes_ << " cost:" << DiffTime(begin_time_, end) << " ip:" << cli_ip_ << " port:" << cli_port_);
    CheckRead(true);
  }else{
    MCE_INFO("ClientConnection::OnError --> up conn err " << err_desc << " " << memcli->GetEndpoint().address().to_string());
  }
}

void ClientConnection::OnEnd(MemcachedClient* memcli){
  up_clis_.erase(memcli);
  //pool_manager_->ReleaseClient(memcli);
  
  if(up_clis_.empty()){
    
    if(!wrote_end_){
      AsyncWrite(MemcProtoParser::END_BUF, MemcProtoParser::END_BUF_LEN, NULL);
      wrote_end_ = true;
    }else{
      MCE_WARN("ClientConnection::OnEnd --> had wrote end " << this);
    }
    cur_recv_len_ = 0;
    /*if(missed_keys_.empty()){
      MCE_INFO("ClientConnection::OnEnd --> need not load");
    }else{
      MCE_INFO("ClientConnection::OnEnd --> need load, size:" << missed_keys_.size());
    }*/
    //missed_keys_.clear();
    //MCE_INFO("ClientConnection::OnEnd --> req read " << this);
    //AsyncRead();
    timeval end;
    gettimeofday(&end, NULL);
    MCE_INFO("ClientConnection::OnEnd --> write end to client, " << index_ << " "
            << key_type_ << " req:" << req_keys_ << " res:" << res_keys_
            << " write:" << write_bytes_ << " cost:" << DiffTime(begin_time_, end) << " ip:" << cli_ip_ << " port:" << cli_port_);
    CheckRead(true);
  } 
}

void ClientConnection::OnStored(MemcachedClient* memcli){
  MCE_INFO("ClientConnection::OnStored --> " << index_ << " " << key_type_ << " ip:" << cli_ip_);
  up_clis_.erase(memcli);
  //pool_manager_->ReleaseClient(memcli);
  AsyncWrite(MemcProtoParser::STORED_BUF, MemcProtoParser::STORED_BUF_LEN, NULL);
  CheckRead(true);
}

void ClientConnection::OnNotStored(MemcachedClient* memcli){
  MCE_INFO("ClientConnection::OnNotStored --> " << key_type_ << " ip:" << cli_ip_);
  up_clis_.erase(memcli);
  //pool_manager_->ReleaseClient(memcli);
  AsyncWrite(MemcProtoParser::NOT_STORED_BUF, MemcProtoParser::NOT_STORED_BUF_LEN, NULL);
  CheckRead(true);
}

void ClientConnection::OnDeleted(MemcachedClient* memcli){
  MCE_INFO("ClientConnection::OnDeleted --> " << index_ << " " << key_type_ << " ip:" << cli_ip_);
  up_clis_.erase(memcli);
  AsyncWrite(MemcProtoParser::DELETED_BUF, MemcProtoParser::DELETED_BUF_LEN, NULL);
  CheckRead(true);
}

void ClientConnection::OnNotFound(MemcachedClient* memcli){
  MCE_INFO("ClientConnection::OnNotFound --> " << index_ << " " << key_type_ << " ip:" << cli_ip_);
  up_clis_.erase(memcli);
  AsyncWrite(MemcProtoParser::NOT_FOUND_BUF, MemcProtoParser::NOT_FOUND_BUF_LEN, NULL);
  CheckRead(true);
}

}
}

