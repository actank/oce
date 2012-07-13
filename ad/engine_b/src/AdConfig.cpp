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
  string file_name = "../etc/ad_engine_b.config";
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
          } else if (key.compare("SA") == 0) {
            SA_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] SA = " << SA_);
          } else if (key.compare("SAA") == 0) {
            SAA_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] SAA = " << SAA_);
          } else if (key.compare("NielsonDB") == 0) {
            NielsonDB_ = value;
            MCE_INFO("[LOAD_CONFIG] NielsonDB = " << NielsonDB_);
          } else if(key.compare("url_encode") == 0) {
            url_encode_ = boost::lexical_cast<bool>(value);
            MCE_INFO("[LOAD_CONFIG] url_encode = " << url_encode_);
          } else if(key.compare("cpm_esc_pro") == 0) {
            cpm_esc_pro_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] cpm_esc_pro = " << cpm_esc_pro_);
          } else if(key.compare("cpm_esc_pro75") == 0) {
            cpm_esc_pro75_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] cpm_esc_pro75 = " << cpm_esc_pro75_);
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
