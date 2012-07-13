#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TCompactProtocol.h>
#include "IceLogger.h"
#include "AntiFraudClient.h"

namespace xce {
namespace ad {

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace com::renren::ad::engine;

bool AntiFraudClient::check(const ClickAction& clickAction) {
  MCE_DEBUG("AntiFraudClient::check() --> enter");
  bool res = true;
  try{
  //boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
  boost::shared_ptr<TSocket> socket(new TSocket(host_, port_));
  boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TCompactProtocol(transport));

  AntiFraudServiceClient client(protocol);
  transport->open();
  res = client.check(clickAction);
  transport->close();
  } catch (std::exception e) {
    res = false;
    MCE_WARN("AntiFraudClient::check() --> error: " << e.what());
  }
  return res;
  MCE_DEBUG("AntiFraudClient::check() --> out");
}

}
}
