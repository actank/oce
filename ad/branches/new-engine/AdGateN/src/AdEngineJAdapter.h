#ifndef _AD_ENGINE_J_ADAPTER_H_
#define _AD_ENGINE_J_ADAPTER_H_

#include "Singleton.h"
#include "AdGate.h"
#include "AdEngine.h"
#include "ServiceI.h"
#include "ad/enginejclient/src/EngineJClient.h"

namespace xce {
namespace ad {
using namespace com::renren::ad::engine;
using namespace MyUtil;
using namespace com::xiaonei::xce;

class AdEngineJAdapter : public MyUtil::Singleton<AdEngineJAdapter>  {
public:
  AdEngineJAdapter(){};
  AdResultMap GetAds(const string & user_profile, const string & referer, int rotate_index);
  AdResultMap Request(const AdRequest& request);
  void Click(int uid, const string & ip, const string & text, const string & referer, const string & click_url);
  bool Reload();
private:
  vector<pair<string,int> > clients_;
  int GetServerIndex( int uid) ;
  
};

}
}
#endif //_AD_ENGINE_J_ADAPTER_H_
