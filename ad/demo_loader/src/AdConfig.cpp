/*
 * AdConfig.cpp
 *
 *  Created on: 2010-11-22
 *      Author: ark
 */
#include "AdConfig.h"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include "IceLogger.h"
#include <netdb.h>
#include <arpa/inet.h>

namespace xce {
namespace ad {

bool AdDemoConfig::LoadConfig() {
  string file_name = "../etc/ad_quality_d.config";
//  string file_name = "../etc/ad_engine" + server_index + ".config";
  try {
    ifstream config(file_name.c_str());
    string line;

    if (config) {
      while (config >> line) {
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep("=");
        tokenizer tokens(line, sep);
        string key;
        string value;
        int n = 0;
        for(tokenizer::iterator it = tokens.begin(); it != tokens.end(); ++it, ++n) {
          if (0==n) {
            key = *it;
          } else if(1==n) {
            value = *it;
          }
        }
        if (!key.empty()) {
          map_[key] = value;
          MCE_INFO("EngineConfig::LoadConfig Get config key: " << key
              << ", value: " << value);
        }
        if (2 == n) {
          if (key.compare("all_pv_th") == 0) {
            all_pv_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] all_pv_th = " << all_pv_th_);
          } else if (key.compare("all_click_th") == 0) {
            all_click_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] all_click_th = " << all_click_th_);
          } else if (key.compare("no_gender_pv_th") == 0) {
            no_gender_pv_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] no_gender_pv_th = " << no_gender_pv_th_);
          } else if (key.compare("no_gender_click_th") == 0) {
            no_gender_click_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] no_gender_click_th = " << no_gender_click_th_);
          } else if (key.compare("no_stage_pv_th") == 0) {
            no_stage_pv_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] no_stage_pv_th = " << no_stage_pv_th_);
          } else if (key.compare("no_stage_click_th") == 0) {
            no_stage_click_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] no_stage_click_th = " << no_stage_click_th_);
          } else if (key.compare("spid_pv_th") == 0) {
            spid_pv_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] spid_pv_th = " << spid_pv_th_);
          } else if (key.compare("spid_click_th") == 0) {
            spid_click_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] spid_click_th = " << spid_click_th_);
          } else if (key.compare("two_feature_pv_th") == 0) {
            two_feature_pv_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] two_feature_pv_th = " << two_feature_pv_th_);
          } else if (key.compare("two_feature_click_th") == 0) {
            two_feature_click_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] two_feature_click_th = " << two_feature_click_th_);
          } else if (key.compare("fresh_ad_722_th") == 0) {
            fresh_ad_722_th_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] fresh_ad_722_th = " << fresh_ad_722_th_);
          } else if (key.compare("fresh_ad_721_th") == 0) {
            fresh_ad_721_th_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] fresh_ad_721_th = " << fresh_ad_721_th_);
          } else if (key.compare("fresh_ad_632_th") == 0) {
            fresh_ad_632_th_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] fresh_ad_632_th = " << fresh_ad_632_th_);
          } else if (key.compare("fresh_ad_631_th") == 0) {
            fresh_ad_631_th_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] fresh_ad_631_th = " << fresh_ad_631_th_);
          } else if (key.compare("fresh_ad_702_th") == 0) {
            fresh_ad_702_th_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] fresh_ad_702_th = " << fresh_ad_702_th_);
          } else if (key.compare("fresh_ad_701_th") == 0) {
            fresh_ad_701_th_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] fresh_ad_701_th = " << fresh_ad_701_th_);
          }
           else if (key.compare("test_base_impr") == 0) {
            test_base_impr_ = boost::lexical_cast<Ice::Long>(value);
            MCE_INFO("[LOAD_CONFIG] two_feature_click_th = " << test_base_impr_);
          }
           else if (key.compare("test_real_impr") == 0) {
            test_real_impr_ = boost::lexical_cast<Ice::Long>(value);
            MCE_INFO("[LOAD_CONFIG] test_real_impr = " << test_real_impr_);
          }
          else if (key.compare("test_impr_ratio") == 0) {
            test_impr_ratio_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] test_impr_ratio = " << test_impr_ratio_);
          }
          else if (key.compare("fresh_db_name") == 0) {
            fresh_db_name_ = boost::lexical_cast<string>(value);
            MCE_INFO("[LOAD_CONFIG] fresh_db_name = " << fresh_db_name_);
          }else if (key.compare("state_pv_th") == 0) {
            state_pv_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] state_pv_th_ = " << state_pv_th_);
          }else if (key.compare("state_click_th") == 0) {
            state_click_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] state_click_th_ = " << state_click_th_);
          }else if (key.compare("state1_test_impr_ratio") == 0) {
            state1_test_impr_ratio_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] state1_test_impr_ratio_ = " << state1_test_impr_ratio_);
          }else if (key.compare("state2_test_impr_ratio") == 0) {
            state2_test_impr_ratio_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] state2_test_impr_ratio_ = " << state2_test_impr_ratio_);
          }else if (key.compare("state3_test_impr_ratio") == 0) {
            state3_test_impr_ratio_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] state3_test_impr_ratio_ = " << state3_test_impr_ratio_);
          }
          else if (key.compare("fresh_pv_thr") == 0) {
            fresh_pv_thr_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] fresh_pv_impr = " << fresh_pv_thr_);
          }
          else if (key.compare("fresh_click_thr") == 0) {
            fresh_click_thr_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] fresh_click_ratio = " << fresh_click_thr_);
          }
          else if (key.compare("fresh_ad_test_day") == 0) {
            fresh_ad_test_day_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] fresh_ad_test_day = " << fresh_ad_test_day_);
          }
          else if (key.compare("ecpm_ratio") == 0) {
            ecpm_ratio_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] ecpm_ratio = " << ecpm_ratio_);
          } else if (key.compare("app_all_pv_th") == 0) {
            app_all_pv_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] app_all_pv_th = " << app_all_pv_th_);
          } else if (key.compare("app_all_click_th") == 0) {
            app_all_click_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] app_all_click_th = " << app_all_click_th_);
          } else if (key.compare("app_no_gender_pv_th") == 0) {
            app_no_gender_pv_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] app_no_gender_pv_th = " << app_no_gender_pv_th_);
          } else if (key.compare("app_no_gender_click_th") == 0) {
            app_no_gender_click_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] app_no_gender_click_th = " << app_no_gender_click_th_);
          } else if (key.compare("app_no_stage_pv_th") == 0) {
            app_no_stage_pv_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] app_no_stage_pv_th = " << app_no_stage_pv_th_);
          } else if (key.compare("app_no_stage_click_th") == 0) {
            app_no_stage_click_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] app_no_stage_click_th = " << app_no_stage_click_th_);
          } else if (key.compare("app_spid_pv_th") == 0) {
            app_spid_pv_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] app_spid_pv_th = " << app_spid_pv_th_);
          } else if (key.compare("app_spid_click_th") == 0) {
            app_spid_click_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] app_spid_click_th = " << app_spid_click_th_);
          } else if (key.compare("app_two_feature_pv_th") == 0) {
            app_two_feature_pv_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] app_two_feature_pv_th = " << app_two_feature_pv_th_);
          } else if (key.compare("app_two_feature_click_th") == 0) {
            app_two_feature_click_th_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] app_two_feature_click_th = " << app_two_feature_click_th_);
          } else if (key.compare("db_name") == 0) {
            db_name_ = boost::lexical_cast<string>(value);
            MCE_INFO("[LOAD_CONFIG] db_name = " << db_name_);
          } else if (key.compare("db_host") == 0) {
            db_host_ = boost::lexical_cast<string>(value);
            MCE_INFO("[LOAD_CONFIG] db_host = " << db_host_);
          }else if (key.compare("ctr_discount_type") == 0) {
            ctr_discount_type_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] ctr_discount_type = " << ctr_discount_type_);
          }

        }
      }
    } else {
      MCE_INFO("no file " << file_name);
    }
    if (config) {
      config.close();
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("EngineConfig::LoadConfig error : " << e);
  } catch (...) {
    MCE_WARN("EngineConfig::LoadConfig error : unkown error \"...\"");
  }
  return true;
}
}
}
