#include "memc_command.h"

#include <vector>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "util.h"
#include "LogWrapper.h"
#include "client_conn.h"
#include "upstream_conn.h"
#include "MemcProxyI.h"

using namespace MyUtil;
using namespace xce::feed;


namespace xce {
namespace feed {

static int g_count = 0;

//存储命令 : <command name> <key> <flags> <exptime> <bytes>\r\n
MemcCommand::MemcCommand(boost::asio::io_service& io_service, const ip::tcp::endpoint & ep, 
    ClientConnectionPtr owner, const char * buf, size_t cmd_len) 
  : cmd_line_(buf, cmd_len)
  , cmd_line_forwarded_(false)
  , forwarded_bytes_(0)
  , body_bytes_(0)
  , missed_ready_(false)
  , missed_popped_bytes_(0)
  , missed_timer_(0)
  , upstream_endpoint_(ep)
  , upstream_conn_(0)
  , client_conn_(owner)
  , io_service_(io_service)
  , loaded_(false)
{
  string tmp = cmd_line_.substr(0, cmd_line_.size()-2);
  MCE_INFO("MemcCommand created : cli:" << client_conn_.operator->() << " cmd:" << this << " " << tmp);
  gettimeofday(&time_created_, NULL);
  std::vector<std::string> strs;
  string cmd_line(buf, cmd_len); 
  boost::split(strs, cmd_line, boost::is_any_of(" \r\n"), boost::token_compress_on);
  method_ = strs[0];
  if (method_ == "set") {
    body_bytes_ += boost::lexical_cast<size_t>(strs[4]);
    body_bytes_ += 2; // 数据后面, 要有一个 "\r\n" 串
  } else if (method_ == "get") {
    strs.erase(strs.begin());
    strs.pop_back();
    missed_keys_.swap(strs);
    MCE_DEBUG("keys count : " << missed_keys_.size());
  }

  upstream_conn_ = owner->upconn_pool()->Pop(upstream_endpoint_);
};

MemcCommand::~MemcCommand() {
  if (missed_timer_) {
    delete missed_timer_;
  }
  string tmp = cmd_line_.substr(0, cmd_line_.size()-2);
  float timefrommem = 1000000*(time_ready_.tv_sec-time_created_.tv_sec)+time_ready_.tv_usec-time_created_.tv_usec;
  timefrommem/=1000;
  if(!loaded_){
    time_loaded_ = time_ready_;
  }
  float timefromloader = 1000000*(time_loaded_.tv_sec-time_ready_.tv_sec)+time_loaded_.tv_usec-time_ready_.tv_usec;
  timefromloader/=1000;
  //MCE_INFO("MemcCommand::~MemcCommand --> " << this << " " << tmp << " from mem:" << timefrommem 
  //          << " need load:" << loaded_ << " cost:" << timefromloader);
  //MCE_DEBUG("MemcCommand destroyed : " << cmd_line_ << '@' << -- g_count);
}

void MemcCommand::ForwardData(char * buf, size_t bytes) {
  if (upstream_conn_ == 0) {
    // 需要一个上行的 memcache connection
    upstream_conn_ = new UpstreamConn(io_service_);

    upstream_conn_->socket().async_connect(upstream_endpoint_, boost::bind(&MemcCommand::HandleConnect, shared_from_this(), 
        buf, bytes, boost::asio::placeholders::error));
    return;
  }
  
  //MCE_DEBUG(cmd_line_ << " write data to upstream. bytes : " << bytes);
  char * p = buf;
  if (!cmd_line_forwarded()) {
    p -= cmd_line_.size(); // TODO : 检查逻辑, 确保这里确实不会越界
    memcpy(p, cmd_line_.c_str(), cmd_line_.size());
    bytes += cmd_line_.size();
  }

  //MCE_INFO("MemcCommand::ForwardData --> AsyncWrite cli:" << client_conn_.operator->() << " cmd:" << this);
  async_write(upstream_conn_->socket(),
      boost::asio::buffer(p, bytes),
      boost::bind(&MemcCommand::HandleWrite, shared_from_this(), p, 0, bytes,
          boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void MemcCommand::Abort() {
  if (upstream_conn_) {
    upstream_conn_->socket().close();
    MCE_INFO("MemcCommand Abort OK.");
    delete upstream_conn_;
    upstream_conn_ = 0;
  } else {
    MCE_WARN("MemcCommand Abort NULL upstream_conn_.");
  }
}

void MemcCommand::AsyncRead() {
  UpstreamConn * conn = upstream_conn_;
  //MCE_DEBUG(cmd_line_ << " read buffer size:" << UpstreamConn::BUFFER_SIZE - conn->pushed_bytes_);

  conn->socket().async_read_some(boost::asio::buffer(conn->buf_ + conn->pushed_bytes_, UpstreamConn::BUFFER_SIZE - conn->pushed_bytes_),
                  boost::bind(&MemcCommand::HandleRead, shared_from_this(),
                      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void MemcCommand::HandleWrite(char * buf, size_t buf_offset, 
    const size_t bytes, // 命令的当前可写字节数(包含命令行和body)
    const boost::system::error_code& error, size_t bytes_transferred)
{
  if (error) {
    MCE_WARN(cmd_line_ << " upstream write error : " << upstream_endpoint_ << " - "  << error << " " << error.message());
    client_conn_->OnCommandError(shared_from_this(), error);
    return;
  }

  //MCE_DEBUG(cmd_line_ << " bytes written to upstream : " << bytes);

  // forwarded_bytes_ , 是本命令总的已发送字节数
  // bytes , 是本命令本次已发送字节数, 其中"可能"(cmd_line_forwarded_标志)包括cmd_line的长度
  forwarded_bytes_ += bytes_transferred;
  buf_offset += bytes_transferred;

  if (buf_offset < bytes) {
    //MCE_DEBUG(cmd_line_ << "向 upstream 没写完, 继续写.");

    boost::asio::async_write(upstream_conn()->socket(),
        boost::asio::buffer(buf + bytes_transferred, bytes - bytes_transferred),
        boost::bind(&MemcCommand::HandleWrite, shared_from_this(), buf, buf_offset, bytes,
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    return;
  }

  if (forwarded_bytes_ == total_bytes()) {
    // 转发了当前命令的所有数据, 需要 client_conn_ 调整相应的 buffer offset
    //MCE_DEBUG(cmd_line_ << "转发了当前命令的所有数据, 等待 upstream 的响应.");
     
    UpstreamConn * conn = upstream_conn();
    if(!conn){
      MCE_WARN("MemcCommand::HandleWrite --> up conn " << conn << " cmd:" << this);
      return;
    }
    conn->pushed_bytes_ = conn->popped_bytes_ = 0;
    //MCE_INFO("MemcCommand::HandleWrite --> AsyncRead cli:" << client_conn_.operator->() << " cmd:" << this);
    AsyncRead();
    //conn->socket().async_read_some(boost::asio::buffer(conn->buf_, UpstreamConn::BUFFER_SIZE),
    //                boost::bind(&MemcCommand::HandleRead, shared_from_this(),
    //                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    client_conn_->HandleForwardAllData(shared_from_this(), bytes);
  } else {
    // 转发了当前命令的部分数据, 需要client_conn_ 请求更多client数据
    //MCE_DEBUG( cmd_line_ << "转发了一部分, 继续处理.");
    client_conn_->HandleForwardMoreData(shared_from_this(), bytes);
  }
}

void MemcCommand::HandleConnect(char * buf, size_t bytes, const boost::system::error_code& error)
{
  if (error) {
    MCE_WARN("upstream connect error : " << upstream_endpoint_ << " - " << error << " " << error.message());
    client_conn_->OnCommandError(shared_from_this(), error);
    return;
  }

  // TODO : socket option 定制
  ip::tcp::no_delay no_delay(true);
  upstream_conn_->socket().set_option(no_delay);

  socket_base::keep_alive keep_alive(true);
  upstream_conn_->socket().set_option(keep_alive);

  boost::asio::socket_base::linger linger(true, 0);
  upstream_conn_->socket().set_option(linger);

  //boost::asio::socket_base::receive_buffer_size recv_buf_size;
  //upstream_conn_->socket().get_option(recv_buf_size);

  //boost::asio::socket_base::send_buffer_size send_buf_size;
  //upstream_conn_->socket().get_option(send_buf_size);

  ForwardData(buf, bytes);
}

bool MemcCommand::NeedLoadMissed() {
  if (missed_keys_.empty()) {
    return false;
  }

  return boost::starts_with(missed_keys_[0], "FEED#");
}

void MemcCommand::LoadMissedKeys() {
  MCE_INFO("MemcCommand::LoadMissedKeys --> cli:" << client_conn_.operator->() << " cmd:" << this);
  loaded_ = true;
  TaskManager::instance().execute(new GetMissedTask(missed_keys_, shared_from_this()));
}

void MemcCommand::RemoveMissedKey(const string & key) {
  for (size_t i = 0; i < missed_keys_.size(); ++ i) {
    if (missed_keys_[i] == key) {
      missed_keys_.erase(missed_keys_.begin() + i);
      break;
    }
  }
}

// 注意, 该函数会被其他线程调用!
void MemcCommand::DispatchMissedKeyData() {
  missed_ready_ = true; // 这里不需要同步

  //MCE_DEBUG(cmd_line_ << " 从 loader 取数据完成");

  missed_timer_ = new boost::asio::deadline_timer(io_service_, boost::posix_time::microsec(1));
  missed_timer_->async_wait(boost::bind(&MemcCommand::HandleMissedKeyReady, shared_from_this()));
}

void MemcCommand::HandleMissedKeyReady() {
  MCE_INFO("MemcCommand::HandleMissedKeyReady --> cli:" << client_conn_.operator->() << " cmd:" << this);
  gettimeofday(&time_loaded_, NULL); 
  client_conn_->OnCommandReady(shared_from_this());
}

void MemcCommand::HandleRead(const boost::system::error_code& error, size_t bytes_transferred)
{
  if (error) {
    MCE_WARN(cmd_line_ << " upstream read error : " << upstream_endpoint_ << " - "  << error << " " << error.message());
    client_conn_->OnCommandError(shared_from_this(), error);
    return;
  }

  // MCE_DEBUG(cmd_line_ << " bytes read from up server : " << bytes_transferred);

  UpstreamConn * conn = upstream_conn();
  if(!conn){
    MCE_WARN("MemcCommand::HandleRead --> up conn " << conn << " cmd:" << this);
    return;
  }
  conn->pushed_bytes_ += bytes_transferred;

  client_conn_->OnCommandReady(shared_from_this());
}

}
}

