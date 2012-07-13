#ifndef _ENGINE_J_CLIENT_H_
#define _ENGINE_J_CLIENT_H_
#include "ImprService.h"

namespace xce {
namespace ad {
using namespace com::renren::ad::engine;

class EngineJClient {
public:
  EngineJClient(string host = "localhost", int port = 9090): host_(host), port_(port) {}
  void handleRequest(AdResponse& _return, const AdRequest& request);
  void handleClick(const ClickAction& clickAction);
private:
  std::string host_;
  int port_;
};

}
}
#endif //_ENGINE_J_CLIENT_H_
