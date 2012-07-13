/*
 * AdConfig.h
 *
 *  Created on: 2010-11-2
 *      Author: zhaohui.tang
 */

#ifndef AD_CONFIG_H_
#define AD_CONFIG_H_

#include "Util.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "Evictor.h"
#include <map>

namespace xce {
namespace ad {

class EngineConfig : public MyUtil::Singleton<EngineConfig> {
public:                                       
  EngineConfig(): DB_("ade_db"), NielsonDB_("report.db.jebe.d.xiaonei.com"), sys_min_price_(100),
                  SA_(53), SAA_(43),
                  server_name_("AdEngineB0@127.0.0.1"), server_object_("brand"), url_encode_(true), cpm_esc_pro_(30), cpm_esc_pro75_(30) {
    
    LoadConfig();
    MyUtil::TaskManager::instance().schedule(new xce::ad::EngineConfig::ConfigLoadTimer(60*1000));
  }
  bool url_encode() {
    return url_encode_;
  }
  int SA() {
    return SA_;
  }
  int SAA() {
    return SAA_;
  }
  string DB() {
    return DB_;
  }
  string NielsonDB() {
    return NielsonDB_;
  }
  void set_DB(string DB) {
    DB_ = DB;
  }
  int sys_min_price() {
    return sys_min_price_;
  }
  string server_name() {
    return server_name_;
  }
  void set_server_name(int server_index) {
    ostringstream oss;
    oss << "AdEngineB" << server_index << "@" << GetIp();
    server_name_ = oss.str();
  }
  void set_server_name(string server_name) {
    server_name_ = server_name;
  }
  bool LoadConfig();
  string GetValue(string key) {
    map<string, string>::iterator it = map_.find(key);
    if (it != map_.end()) {
      return it->second;
    } else {
      return string();
    }
  }
  void set_server_object(string server_object) {
    server_object_ = server_object;
  }
  bool IsForBrand() {
    return server_object_ == "brand";
  }
  int cpm_esc_pro() {
    return cpm_esc_pro_;
  }
  int cpm_esc_pro75() {
    return cpm_esc_pro75_;
  }
private:
  string DB_;
  string NielsonDB_;
  int sys_min_price_;
  int SA_;
  int SAA_;
  string server_name_;
  string server_object_; // 服务对象, 分为品牌/ 自助
  map<string, string> map_;
  bool url_encode_;
  int cpm_esc_pro_;
  int cpm_esc_pro75_;

  string GetIp();
  class ConfigLoadTimer : public MyUtil::Timer{
  public:
    ConfigLoadTimer(int time = 0):Timer(time){}
    virtual void handle() {
      MyUtil::TaskManager::instance().schedule(new ConfigLoadTimer(60 * 1000));//1分钟更改一次
      EngineConfig::instance().LoadConfig();
    }
  };
};

} // end namespace ad
} // end namespace xce
#endif /* AD_CONFIG_H_ */
