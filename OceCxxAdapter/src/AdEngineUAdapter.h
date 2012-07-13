#ifndef AD_ENGINEU_ADAPTER_H_
#define AD_ENGINEU_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "AdEngine.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdEngineUAdapter: public MyUtil::ReplicatedClusterAdapterI<AdMatchManagerPrx>, 
    public MyUtil::Singleton<AdEngineUAdapter> { 
    //public MyUtil::AdapterI,
    //public MyUtil::AdapterISingleton<MyUtil::AdChannel, AdEngineUAdapter> {
public:
 // AdEngineUAdapter() {
 //   _managers.resize(cluster() > 0 ? cluster() : 1);
 //   _managersOneway.resize(cluster() > 0 ? cluster() : 1);
 //   srand(time(NULL));
 // }
  AdEngineUAdapter() : MyUtil::ReplicatedClusterAdapterI<AdMatchManagerPrx> ("ControllerAdEngineU",120,300,new AdChannel) {
      srand(time(NULL));
  }

  AdResultMap GetAds(const string & user_profile, const string & referer, int rotate_index, bool env_test = false);
  AdResultMap GetAds(const string & user_profile, const string & referer, int rotate_index, int login_uid, const string& tag = "", const string& photo = "", bool env_test = false);
  AdZone2Creatives GetAdsByZone(const string & user_profile, const string & referer, int rotate_index, int login_uid, const string& tag = "", const string& photo = "", const string& count = "", bool env_test = false);
  AdZone2Creatives GetAdsByZoneInfo(const ::std::string& user_profile, const ::std::string& referer, int rotate_index, const ZoneInfoMap& , int login_uid, const string& photo = "", const string& count = "", bool env_test = false);
  void Click(int uid, const string & ip, const string & text, const string & referer, const string & click_url, bool env_test = false);
  void Close(int uid, Ice::Long creative, bool env_test = false);

  void BindUser(int userid, Ice::Long creativeid, Ice::Long zoneid, bool env_test = false);
  void UnbindUser(int userid, Ice::Long zoneid, bool env_test = false);
  void NotifyBindUser(int userid, Ice::Long creativeid, Ice::Long zoneid);
  void NotifyUnbindUser(int userid, Ice::Long zoneid);

private:

  AdMatchManagerPrx getManager(int id);
  AdMatchManagerPrx getManagerOneway(int id);
  int GetServerIndex(int uid, bool env_test = false);

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@AdEngineU";
  }
  virtual size_t cluster() {
    return 50;
  }

  vector<AdMatchManagerPrx> _managersOneway;
  vector<AdMatchManagerPrx> _managers;
};

}
}


#endif // AD_ENGINEU_ADAPTER_H_
