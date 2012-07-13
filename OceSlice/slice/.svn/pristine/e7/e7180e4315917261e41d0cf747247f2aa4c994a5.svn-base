#ifndef __FEED_FEEDCONFIG_ICE__
#define __FEED_FEEDCONFIG_ICE__
#include <Util.ice>

module xce{
module feed {

struct UserConfig {
  bool sendConfig;
  bool recvConfig;
};  


dictionary<int,UserConfig> UserConfigDict;

class ConfigManager {

  void SetSendConfig(int uid, int bigtype, int appid, bool newsconfig);  
  bool GetSendConfig(int uid, int bigtype, int appid);   

  void SetRecvConfig(int uid, int bigtype, int appid, bool newsconfig);
  MyUtil::IntArray FilterRecvConfig(MyUtil::IntArray target, int bigtype, int appid);

  void SetUserConfig(int uid, int bigtype, int appid, bool sendConfig, bool recvConfig);
  UserConfig GetUserConfig(int uid, int bigtype, int appid);
  
  void Preload();

  UserConfigDict GetUserBlockedAppDict(int uid);
};






};
};
#endif
