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
  EngineConfig(): DB_("ade_db"), sys_min_price_(100), X_(2), Y_(1),
                  server_name_("AdEngineU0@127.0.0.1"), server_object_("self"),
                  cost_discount_(0.85), rand_seed_(1500),

                  sort_type_(1), ctr_power_(1), price_power_(1),user_id_(-1),ad_blind_threshold_(50)
                  , url_encode_(0) , alpha_threshold_(2.5),test_impr_ratio_(0.05),state1_test_impr_ratio_(0.08),
                    state2_test_impr_ratio_(0.05),state3_test_impr_ratio_(0.05),
                    state1_reserved_test_impr_ratio_(0.2),state2_reserved_test_impr_ratio_(0.2),state3_reserved_test_impr_ratio_(0.2),
                  photo_wb1_count_(2),big_pic_id_(31), style_db_host_("10.3.23.69") {

    priority_pos_.push_back(1);
    priority_pos_.push_back(2);
    priority_pos_.push_back(3);
    priority_pos_.push_back(4);

    inner_priority_pos_.push_back(3);
    inner_priority_pos_.push_back(1);
    inner_priority_pos_.push_back(2);
    inner_priority_pos_.push_back(4);
    
  //  LoadConfig();
  //  MyUtil::TaskManager::instance().schedule(new xce::ad::EngineConfig::ConfigLoadTimer(60*1000));
  }
  bool url_encode() {
    return url_encode_;
  }
  float X() {
    return X_;
  }
  float Y() {
    return Y_;
  }
  string DB() {
    return DB_;
  }
  void set_DB(string DB) {
    DB_ = DB;
  }
  int sys_min_price() {
    return sys_min_price_;
  }
  vector<int> priority_pos() {
    return priority_pos_;
  }
  vector<int> inner_priority_pos() {
    return inner_priority_pos_;
  }
  void set_X(float X) {
    X_ = X;
  }
  void set_Y(float Y) {
    Y_ = Y;
  }
  int server_index() {
    return server_index_;
  }
  string server_name() {
    return server_name_;
  }
  void set_server_name(int server_index) {
    server_index_ = server_index;
    ostringstream oss;
    oss << "AdEngineU" << server_index << "@" << GetIp();
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
  float GetCostDiscount() {
    return cost_discount_;
  }
  double GetAlphaThreshold(){
    return alpha_threshold_;
  }
  int GetRandSeed() {
    return rand_seed_;
  }
  int sort_type() {
    return sort_type_;
  }
  int price_power() {
    return price_power_;
  }
  int ctr_power() {
    return ctr_power_;
  }
  int GetUserId() {
    return user_id_;
  }
  int AdBlindThreshold() {
    return ad_blind_threshold_;
  }
  double GetTestImprRatio() {
    return test_impr_ratio_;
  }
  double GetStateTestImprRatio(Ice::Long zone, int pos) {
    Ice::Long key = zone * 10 + pos;
    if(key == 1000000000723 || key == 1000000000702 || key == 1000000000703) {
      return state1_test_impr_ratio_;
    } else if(key == 1000000000701 || key == 1000000000722 || key == 1000000000633) {
      return state2_test_impr_ratio_;
    } else if(key == 1000000000721 || key == 1000000000632 || key == 1000000000631) {
      return state3_test_impr_ratio_;
    }
    return 0.05;
  }
  double GetReservedStateTestImprRatio(Ice::Long zone, int pos) {
    Ice::Long key = zone * 10 + pos;
    if(key == 1000000000723 || key == 1000000000702 || key == 1000000000703) {
      return state1_reserved_test_impr_ratio_;
    } else if(key == 1000000000701 || key == 1000000000722 || key == 1000000000633) {
      return state2_reserved_test_impr_ratio_;
    } else if(key == 1000000000721 || key == 1000000000632 || key == 1000000000631) {
      return state3_reserved_test_impr_ratio_;
    }
    return 0.05;
  }
  int photo_wb1_count() {
    return photo_wb1_count_;
  }
  int big_pic_id() {
    return big_pic_id_;
  }
  string style_db_host() {
    return style_db_host_;
  }

private:
  string DB_;
  int sys_min_price_;
  float X_;
  float Y_;
  float cost_discount_;
  double alpha_threshold_;
  double test_impr_ratio_;
  int rand_seed_;
  int price_power_;
  int ctr_power_;
  int sort_type_;
  int server_index_;
  int user_id_;
  int ad_blind_threshold_;

  double state1_test_impr_ratio_;
  double state2_test_impr_ratio_;
  double state3_test_impr_ratio_;
  
  double state1_reserved_test_impr_ratio_;
  double state2_reserved_test_impr_ratio_;
  double state3_reserved_test_impr_ratio_;
 
  string server_name_;
  string server_object_; // 服务对象, 分为品牌/ 自助
  vector<int> priority_pos_;
  vector<int> inner_priority_pos_;
  map<string, string> map_;
  bool url_encode_;
  int photo_wb1_count_;
  int big_pic_id_;
  string style_db_host_;

  string GetIp();
public:
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
