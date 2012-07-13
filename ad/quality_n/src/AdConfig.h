/*
 * AdConfig.h
 *
 *  Created on: 2010-11-2
 *      Author: zhaohui.tang
 */

#ifndef AD_RECONFIG_H_
#define AD_RECONFIG_H_

#include "Util.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "Evictor.h"
#include <map>
#include "LogWrapper.h"
namespace xce {
namespace ad {

/***********************************************************************************************************************/
class AdConfig : public MyUtil::Singleton<AdConfig> {
public:
  AdConfig():server_index("0") {
  }
  bool LoadConfig();
  void set_server_index(string index) {
    server_index = index;
  }
  string GetValue(string key) {
    map<string, string>::iterator it = map_.find(key);
    if (it != map_.end()) {
      return it->second;
    } else {
      return string();
    }
  }
  class ConfigLoadTimer : public MyUtil::Timer {
  public:
    ConfigLoadTimer(int time = 0) :
      Timer(time) {
    }
    virtual void handle() {
      MyUtil::TaskManager::instance().schedule(new ConfigLoadTimer(60 * 1000));//1分钟更改一次
      AdConfig::instance().LoadConfig();
    }
  };
private:
  map<string, string> map_;
  string server_index;
};

} // end namespace ad
} // end namespace xce
#endif /* AD_CONFIG_H_ */
