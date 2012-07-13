/*
 * AdConfig.cpp
 *
 *  Created on: 2010-11-22
 *      Author: ark
 */
#include "AdReConfig.h"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include "IceLogger.h"

namespace xce {
namespace ad {

bool RecommendConfig::LoadConfig() {
  string file_name = "../etc/ad_recommends.config";
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
      }
    } else {
      MCE_INFO("no file " << file_name);
    }
    if (config) {
      config.close();
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("RecommendConfig::LoadConfig error : " << e);
  } catch (...) {
    MCE_WARN("RecommendConfig::LoadConfig error : unkown error \"...\"");
  }
  return true;
}

}
}
