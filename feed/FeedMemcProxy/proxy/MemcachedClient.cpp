#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include "MemcachedClient.h"
#include "MemcachedClientPool.h"
#include "LogWrapper.h"

MemcachedClient::MemcachedClient(boost::asio::io_service& io_service, MemcachedClientPool* owner, const ip::tcp::endpoint& endpoint):
socket_(io_service),
endpoint_(endpoint),
owner_(owner)
{
  reseting_ = false;
  handle_ = NULL;
  socket_err_ = true;
  recv_buf_len_ = BASE_SIZE;
  recv_buf_ = new char[recv_buf_len_]; 
  MCE_INFO("MemcachedClient::MemcachedClient --> " << this);
  start_time_ = time(NULL);
  write_len_ = 0;
  recv_values_ = 0;
}

MemcachedClient::~MemcachedClient(){
  MCE_INFO("MemcachedClient::~MemcachedClient --> " << this);
}

void MemcachedClient::Reset(){
  timeval end;
  MCE_INFO("MemcachedClient::Reset --> " << this << " type:" << cmd_type_ << " cost:" << CaculateCost() << " " << business_type_ << " recv:"
            << total_recv_len_ << " " << cur_recv_len_ 
            << " " << recv_values_ << " dur:" << (time(NULL)-start_time_) << " ip:" << endpoint_.address().to_string());
  reseting_ = true;
  handle_ = NULL;
  socket_.close();
  //socket_.open(ip::tcp::v4()); 
  socket_err_ = true;
  cur_recv_len_ = 0; 
  delete recv_buf_;
  recv_buf_ = NULL;
}

void MemcachedClient::Init(CmdType cmd_type, const char* cmd_buf, int len, MemcDataHandle* handle){
  recv_values_ = 0;
  write_len_ = 0;
  cur_recv_len_ = 0; 
  total_recv_len_ = 0; 
  handle_ = handle;
  cmd_buf_ = cmd_buf;
  cmd_buf_len_ = len;
  cmd_type_ = cmd_type;
  gettimeofday(&begin_, NULL);
  //MCE_INFO("MemcachedClient::Init --> " << cmd_buf);
  business_type_ = string(cmd_buf+4, 4);
}

void MemcachedClient::Request(){
  if(socket_err_){
    MCE_INFO("MemcachedClient::Request --> connecting " << this);
    socket_.async_connect(endpoint_, boost::bind(&MemcachedClient::HandleConnect, this, boost::asio::placeholders::error));
  }else{
    async_write(socket_, boost::asio::buffer(cmd_buf_, cmd_buf_len_),
      boost::bind(&MemcachedClient::HandleWrite, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  }
}

void MemcachedClient::HandleConnect(const boost::system::error_code& error){
  if (error) {
    socket_err_ = true; 
    MCE_WARN("MemcachedClient::HandleConnect --> err, " << this << " " << error.message() << " handler:" << handle_);
    if(handle_){
      handle_->OnError(CONNECT_ERR, "connect server err", this);  
    }
    if(!reseting_){
      Reset();
    }else{
      MCE_INFO("MemcachedClient::HandleConnect --> delete " << this << " " << error.message());
      owner_->ReleaseErrClient(this);
      //delete this;
    }
    return;
  }
  MCE_INFO("MemcachedClient::HandleConnect --> connected " << this << " " << endpoint_);
  socket_err_ = false; 
  ip::tcp::no_delay no_delay(true);
  socket_.set_option(no_delay);
  socket_base::keep_alive keep_alive(true);
  socket_.set_option(keep_alive);
  boost::asio::socket_base::linger linger(true, 0);
  socket_.set_option(linger); 

  async_write(socket_, boost::asio::buffer(cmd_buf_, cmd_buf_len_),
  boost::bind(&MemcachedClient::HandleWrite, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void MemcachedClient::AsyncRead(){
   socket_.async_read_some(boost::asio::buffer(recv_buf_ + cur_recv_len_, recv_buf_len_ - cur_recv_len_),
                      boost::bind(&MemcachedClient::HandleRead, this,
                      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void MemcachedClient::WroteClient(int len){
  if(len != write_len_){
    MCE_WARN("MemcachedClient::WroteClient --> err, " << write_len_ << " " << len);
    return;
  }
  if(reseting_){
    MCE_WARN("MemcachedClient::WroteClient --> resting, " << this);
    return;
  }
  write_len_ = 0;
  MemCmdHead head = MemcProtoParser::ParserHead(recv_buf_ + len, cur_recv_len_-len);
  if(head.succ_==PARSER_ERR){
    MCE_WARN("MemcachedClient::WroteClient --> parser err, " << this);
    return;
  }else if(head.succ_==PARSER_SUC && head.type_==CMD_END){
    if(handle_){
      handle_->OnEnd(this);
    }else{
      MCE_WARN("MemcachedClient::WroteClient --> err, " << this);
    }
    float cost = CaculateCost();
    if(cost > 10){
      MCE_INFO("MemcachedClient::WroteClient --> recv end, cost:" << CaculateCost() << " recv:" << cur_recv_len_ << " " << endpoint_.address().to_string());
    }
    owner_->ReleaseClient(this); 
    return;
  }
  memmove(recv_buf_, recv_buf_ + len, cur_recv_len_ - len);
  cur_recv_len_ -= len;
  AsyncRead();
  //MCE_INFO("MemcachedClient::HandleRead --> memmove " << total_len << " " << cur_recv_len_ << " " << recv_buf_); 
}

void MemcachedClient::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred){
  if(error){
    MCE_WARN("MemcachedClient::HandleWrite --> err, " << this << " " << error.message() << " handler:" << handle_);
    if(handle_){
      handle_->OnError(SEND_ERR, error.message(), this);
    }
    Reset();
    return;
  }
  //MCE_WARN("MemcachedClient::HandleWrite --> wrote to memcached, " << this << " len:" << cmd_buf_len_ << " "<< bytes_transferred);
  //if(bytes_transferred == cmd_buf_len_){
  AsyncRead();
  //}
}

void MemcachedClient::HandleRead(const boost::system::error_code& error, size_t bytes_transferred){
  if(error){
    MCE_WARN("MemcachedClient::HandleRead --> err, " << this << " " << error.message() << " handler:" << handle_);
    if(handle_){
      handle_->OnError(RECV_ERR, error.message(), this);
    }
    if(reseting_){
      MCE_INFO("MemcachedClient::HandleRead --> delete " << this << " " << error.message());
      owner_->ReleaseErrClient(this);
      //delete this;
      //owner_->ReleaseClient(this); 
    }else{
      MCE_INFO("MemcachedClient::HandleRead --> err, reseting " << this << " " << error.message());
      Reset();
    }
    return;
  }
  if(reseting_){
    MCE_WARN("MemcachedClient::HandleRead --> reseting " << this);
    return;
  }
  cur_recv_len_ += bytes_transferred;
  total_recv_len_ += bytes_transferred;
  if(cmd_type_ == CMD_GET){
    int total_len = 0;
    int count = 0;
    bool get_end = false;
    int loop_count = 0;
    while(cur_recv_len_-total_len > 0){
      MemCmdHead head = MemcProtoParser::ParserHead(recv_buf_ + total_len, cur_recv_len_-total_len);
      if(head.succ_==PARSER_ERR){
        MCE_WARN("MemcachedClient::HandleRead --> parser err " << this);
        return;
      }else if(head.succ_==PARSER_MORE){
        break;
      }
      if(head.type_==CMD_END){
        get_end = true;
        break;
      }
      if(head.type_ == CMD_VALUE){
        total_len += head.data_len_;
        recv_values_++;
        count++;
      }else{
        MCE_WARN("MemcachedClient::HandleRead --> recv err " << head.type_ << " " << cmd_buf_ << " " << recv_buf_+total_len);
        break;
      }
      loop_count++;
      if(loop_count == 10000){
        MCE_WARN("MemcachedClient::HandleRead --> @#@#@ " << cur_recv_len_ << " " << total_len << " " << head.data_len_ << " " << recv_buf_+total_len);
      }
    }
    if(total_len==0 && get_end){
      MCE_INFO("MemcachedClient::HandleRead --> recv end, " << this);
      if(handle_){
        handle_->OnEnd(this);
      }else{
        MCE_WARN("MemcachedClient::HandleRead --> err, handle is NUL, " << this);
      }
      owner_->ReleaseClient(this); 
      return;
    }
    if(total_len==0){
      AsyncRead();
    } 
    if(total_len > 0){
      if(handle_){
        handle_->OnRecv(recv_buf_, total_len, count, "", this);
        write_len_ = total_len;
      }else{
        MCE_WARN("MemcachedClient::HandleRead --> OnRecv handle is NULL, " << this);
      }
    }else if(cur_recv_len_ == recv_buf_len_){
      char* p = recv_buf_;
      recv_buf_len_ *= FACTOR;
      recv_buf_ = new char[recv_buf_len_];
      memset(recv_buf_, 0, recv_buf_len_);
      memcpy(recv_buf_, p, cur_recv_len_);
      delete p;
      AsyncRead();
      MCE_INFO("MemcachedClient::HandleRead --> new a more large buf " << recv_buf_len_);
    }
  }else if(cmd_type_==CMD_SET || cmd_type_==CMD_APPEND){
    if(cur_recv_len_ == MemcProtoParser::STORED_BUF_LEN){
      handle_->OnStored(this);
      owner_->ReleaseClient(this); 
    }else if(cur_recv_len_ == MemcProtoParser::NOT_STORED_BUF_LEN){
      handle_->OnNotStored(this);
      owner_->ReleaseClient(this); 
    }
  }else if(cmd_type_==CMD_DELETE){
    if(cur_recv_len_ == MemcProtoParser::DELETED_BUF_LEN){
      handle_->OnDeleted(this);
      owner_->ReleaseClient(this); 
    }else if(cur_recv_len_ == MemcProtoParser::NOT_FOUND_BUF_LEN){
      handle_->OnNotFound(this);
      owner_->ReleaseClient(this); 
    }
  }
}

