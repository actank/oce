#include "AdGateConfig.h"
#include "Singleton.h"
#include "TaskManager.h"
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include "IceLogger.h"
#include "AdGateData.h"

namespace xce {
namespace ad {

GateConfig::GateConfig():DB_("ade_db") {
  MyUtil::TaskManager::instance().schedule(new GateConfigLoadTimer());
};

bool GateConfig::LoadConfig() {
  string file_name = "../etc/ad_gate_s.config";
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
        if (2 == n) {
          if (key.compare("100000000001") == 0) {
            MCE_INFO("[LOAD_CONFIG] adzone_id = 100000000001, next_load_time = " << value);
            RefreshTime::instance().Load(value);
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
    MCE_WARN("GateConfig::LoadConfig error : " << e);
  } catch (...) {
    MCE_WARN("GateConfig::LoadConfig error : unkown error \"...\"");
  }
  return true;
};

}
}
