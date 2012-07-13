#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TCompactProtocol.h>
#include "IceLogger.h"
#include "EngineJClient.h"

namespace xce {
namespace ad {

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace com::renren::ad::engine;

void EngineJClient::handleRequest(AdResponse& result, const AdRequest& request) {
  MCE_DEBUG("EngineJClient::handleRequest() --> enter");
  //boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
  try{
    boost::shared_ptr<TSocket> socket(new TSocket(host_, port_));
    boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TCompactProtocol(transport));
    MCE_DEBUG("EngineJClient::handleRequest() --> Line: " << __LINE__);
    ImprServiceClient client(protocol);
    MCE_DEBUG("EngineJClient::handleRequest() --> Line: " << __LINE__);
    transport->open();
    MCE_DEBUG("EngineJClient::handleRequest() --> Line: " << __LINE__);
    client.handleRequest(result, request);
    MCE_DEBUG("EngineJClient::handleRequest() --> Line: " << __LINE__);
    transport->close();
  } catch (std::exception e) {
    MCE_WARN("EngineJClient::handleRequest() --> error: " << e.what());
  }
  MCE_DEBUG("EngineJClient::handleRequest() --> out");
}

void EngineJClient::handleClick(const ClickAction& clickAction) {
  MCE_DEBUG("EngineJClient::handleClick() --> enter");
  try{
  //boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
  boost::shared_ptr<TSocket> socket(new TSocket(host_, port_));
  boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TCompactProtocol(transport));

  ImprServiceClient client(protocol);
  transport->open();
  client.handleClick(clickAction);
  transport->close();
  } catch (std::exception e) {
    MCE_WARN("EngineJClient::handleClick() --> error: " << e.what());
  }
  MCE_DEBUG("EngineJClient::handleClick() --> out");
}

}
}
