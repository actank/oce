#ifndef _AD_ENGINE_J_ADAPTER_H_
#define _AD_ENGINE_J_ADAPTER_H_
#include "Singleton.h"
#include "AdEngine.h"
#include "../../ad/enginejclient/src/EngineJClient.h"

namespace xce {
namespace ad {
using namespace com::renren::ad::engine;

class AdEngineJAdapter {
public:
  AdEngineJAdapter(string host = "localhost", int port = 9090):
                   engine_j_client_(host, port) {}
  AdResultMap GetAds(const string & user_profile, const string & referer, int rotate_index);
  AdResultMap Request(const AdRequest& request);
  void Click(int uid, const string & ip, const string & text, const string & referer, const string & click_url);
private:
  EngineJClient engine_j_client_;
};

}
}
#endif //_AD_ENGINE_J_ADAPTER_H_
