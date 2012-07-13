#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <protocol/TCompactProtocol.h>
#include "PingService.h"
#include "PingSender.h"
#include "IceLogger.h"

namespace xce {
namespace ad {

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace com::renren::ad::ping;

void PingSender::SetServerInfo(const string& server_name, int threshold){
  size_t it = server_name.find('@');
  if(it != string::npos) {
    server_name_ = server_name.substr(0, it);
    node_ = server_name.substr(it+1);
  } else {
    server_name_ = server_name;
    node_ = "unknown";
  }
  threshold_ = threshold;
  MCE_DEBUG("PingSender::SetServerInfo() --> server_name = " << server_name << ",  server_name_ = " << server_name_ << ", node_ = " << node_ << ", threshold_ = " << threshold_);
}

bool PingSender::Send(const string& tag, const int64_t value) {
  MCE_DEBUG("PingSender::Send() --> tag = " << tag << ", value = " << value);
  vector<int64_t> message;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    map<string, vector<int64_t> >::iterator it = messages_.find(tag);
    if(it != messages_.end()) {
      it->second.push_back(value);
      if(it->second.size() >= threshold_) {
        it->second.swap(message);
      }
    } else {
      vector<int64_t> v;
      v.push_back(value);
      messages_[tag] = v;
    }
  }
  if(!message.empty()) {
    return BatchSend(tag, message);
  }
  return true;
}

bool PingSender::BatchSend(const string& tag, const vector<int64_t>& value) {
  MCE_DEBUG("PingSender::BatchSend() --> tag = " << tag << ", value.size() = " << value.size());
  MyUtil::TaskManager::instance().schedule(new SendTimer(server_name_, node_, tag, value));  
  return true;
}

void PingSender::SendTimer::handle() {
  try{
    boost::shared_ptr<TSocket> socket(new TSocket("ping.jebe.d.xiaonei.com", 6655));
    boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TCompactProtocol(transport));
    PingServiceClient client(protocol);
    transport->open();
    client.pingBatch(server_name_, node_, tag_, values_);
    transport->close();
  } catch (...) {
    MCE_DEBUG("PingSender::SendTimer::handle --> unknown error");
  }
}

}
}
