#ifndef _ANTI_FRAUD_CLIENT_H_
#define _ANTI_FRAUD_CLIENT_H_
#include "AntiFraudService.h"

namespace xce {
namespace ad {
using namespace com::renren::ad::engine;

class AntiFraudClient {
public:
  AntiFraudClient(string host = "localhost", int port = 9096): host_(host), port_(port) {}
  bool check(const ClickAction& clickAction);
private:
  std::string host_;
  int port_;
};

}
}
#endif //_ANTI_FRAUD_CLIENT_H_
