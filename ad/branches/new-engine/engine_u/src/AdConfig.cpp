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

bool EngineConfig::LoadConfig() {
//  string file_name = "../etc/ad_engine.config";
  string server_index; 
  if(server_index_ >= 0) {
    server_index = boost::lexical_cast<string>(server_index_);
  }
  
  string file_name = "../etc/ad_engine" + server_index + ".config";
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
          if (key.compare("sys_min_price") == 0) {
            sys_min_price_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] sys_min_price = " << sys_min_price_);
          } else if (key.compare("priority_pos") == 0) {
            priority_pos_.clear();
            for(string::iterator sit = value.begin(); sit != value.end(); ++sit) {
              int pos = boost::lexical_cast<int>(*sit);
              priority_pos_.push_back(pos);
              MCE_INFO("[LOAD_CONFIG] pos = " << pos);
            }
          } else if (key.compare("X") == 0) {
            X_ = boost::lexical_cast<float>(value);
            MCE_INFO("[LOAD_CONFIG] X = " << X_);
          } else if (key.compare("Y") == 0) {
            Y_ = boost::lexical_cast<float>(value);
            MCE_INFO("[LOAD_CONFIG] Y = " << Y_);
          } else if (key.compare("inner_priority_pos") == 0) {
            inner_priority_pos_.clear();
            for(string::iterator sit = value.begin(); sit != value.end(); ++sit) {
              int pos = boost::lexical_cast<int>(*sit);
              inner_priority_pos_.push_back(pos);
              MCE_INFO("[LOAD_CONFIG] pos = " << pos);
            }

          } else if(key.compare("cost_discount") == 0) {
            cost_discount_ = boost::lexical_cast<float>(value);
            MCE_INFO("[LOAD_CONFIG] cost_discount = " << cost_discount_);
          } else if(key.compare("rand_seed") == 0) {
            rand_seed_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] rand_seed = " << rand_seed_);
          } else if(key.compare("sort_type") == 0) {
            sort_type_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] sort_type = " << sort_type_);

          } else if(key.compare("price_power") == 0) {
            price_power_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] price_power = " << price_power_);
          } else if(key.compare("ctr_power") == 0) {
            ctr_power_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] ctr_power = " << ctr_power_);
          }else if(key.compare("user_id") == 0) {
            user_id_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] user_id = " << user_id_);
          }else if(key.compare("ad_blind_threshold") == 0) {
            ad_blind_threshold_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] ad_blind_threshold = " << ad_blind_threshold_);

          } else if(key.compare("url_encode") == 0) {
            url_encode_ = boost::lexical_cast<bool>(value);
            MCE_INFO("[LOAD_CONFIG] url_encode = " << url_encode_);
          } else if(key.compare("alpha_threshold") == 0) {
            alpha_threshold_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] alpha_threshold = " << alpha_threshold_);
          } else if(key.compare("test_impr_ratio") == 0) {
            test_impr_ratio_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] test_impr_ratio_ = " << test_impr_ratio_);
          } else if (key.compare("state1_test_impr_ratio") == 0) {
            state1_test_impr_ratio_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] state1_test_impr_ratio_ = " << state1_test_impr_ratio_);
          }else if (key.compare("state2_test_impr_ratio") == 0) {
            state2_test_impr_ratio_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] state2_test_impr_ratio_ = " << state2_test_impr_ratio_);
          }else if (key.compare("state3_test_impr_ratio") == 0) {
            state3_test_impr_ratio_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] state3_test_impr_ratio_ = " << state3_test_impr_ratio_);
          } else if (key.compare("state1_reserved_test_impr_ratio") == 0) {
            state1_reserved_test_impr_ratio_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] state1_reserved_test_impr_ratio_ = " << state1_reserved_test_impr_ratio_);
          }else if (key.compare("state2_reserved_test_impr_ratio") == 0) {
            state2_reserved_test_impr_ratio_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] state2_reserved_test_impr_ratio_ = " << state2_reserved_test_impr_ratio_);
          }else if (key.compare("state3_reserved_test_impr_ratio") == 0) {
            state3_reserved_test_impr_ratio_ = boost::lexical_cast<double>(value);
            MCE_INFO("[LOAD_CONFIG] state3_reserved_test_impr_ratio_ = " << state3_reserved_test_impr_ratio_);
          } else if (key.compare("PHOTO_WB1_COUNT") == 0) {
            photo_wb1_count_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] photo_wb1_count_ = " << photo_wb1_count_);
          
          } else if (key.compare("big_pic_id") == 0) {
            big_pic_id_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] big_pic_id_ = " << big_pic_id_);
          } else if (key.compare("style_db_host") == 0) {
            style_db_host_ = value;
            MCE_INFO("[LOAD_CONFIG] style_db_host_ = " << style_db_host_);
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

string EngineConfig::GetIp()
{
    int result = 0;
    char hostName[100];
    struct hostent* host = NULL;
    result = gethostname(hostName, 100);
    host = gethostbyname(hostName);
    return string(inet_ntoa(*((struct in_addr *)host->h_addr_list[0])));
}

}
}
