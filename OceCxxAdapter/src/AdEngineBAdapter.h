#ifndef AD_ENGINEB_ADAPTER_H_
#define AD_ENGINEB_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "AdEngine.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdEngineBAdapter: public MyUtil::ReplicatedClusterAdapterI<AdMatchManagerPrx>, 
  //public MyUtil::AdapterISingleton<MyUtil::AdChannel, AdEngineBAdapter> { 
  public MyUtil::Singleton<AdEngineBAdapter> { 
public:
  /*AdEngineBAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
    srand(time(NULL));
    }*/
  AdEngineBAdapter() : MyUtil::ReplicatedClusterAdapterI<AdMatchManagerPrx> ("ControllerAdEngineB",120,300,new AdChannel) {
  }

  AdResultMap GetAds(const string & user_profile, const string & referer, int rotate_index, bool hide = false);
  void Click(int uid, const string & ip, const string & text, const string & referer, const string & click_url);

  void BindUser(int userid, Ice::Long creativeid, Ice::Long zoneid);
  void UnbindUser(int userid, Ice::Long zoneid);
  void NotifyBindUser(int userid, Ice::Long creativeid, Ice::Long zoneid);
  void NotifyUnbindUser(int userid, Ice::Long zoneid);

private:
  AdMatchManagerPrx getManager(int id);
  AdMatchManagerPrx getManagerOneway(int id);
  int GetServerIndex(int uid);

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@AdEngineB";
  }
  virtual size_t cluster() {
    return 10;
  }

  vector<AdMatchManagerPrx> _managersOneway;
  vector<AdMatchManagerPrx> _managers;
};

}
}


#endif // AD_ENGINEB_ADAPTER_H_
