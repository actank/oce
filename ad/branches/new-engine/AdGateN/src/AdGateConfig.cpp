#include "AdGateConfig.h"
#include "Singleton.h"
#include "TaskManager.h"
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include "IceLogger.h"
//#include "AdGateData.h"

namespace xce {
namespace ad {

GateConfig::GateConfig():DB_("ade_db"), ping_seq_size_(30), engine_host_("localhost"), engine_port_(9090), env_test_(false) {
  MyUtil::TaskManager::instance().schedule(new GateConfigLoadTimer());
};

string GateConfig::GetIp()
{
    int result = 0;
    char hostName[100];
    struct hostent* host = NULL;
    result = gethostname(hostName, 100);
    host = gethostbyname(hostName);
    return string(inet_ntoa(*((struct in_addr *)host->h_addr_list[0])));
}

bool GateConfig::LoadConfig() {
  string file_name = "../etc/ad_gate_n.config";
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
          if (key.compare("PING_SEQ_SIZE") == 0) {
            ping_seq_size_ = boost::lexical_cast<int>(value);
            MCE_INFO("[LOAD_CONFIG] ping_seq_size_ = " << ping_seq_size_);
          } else if (key.compare("ENGINE_HOST") == 0) {
            MCE_INFO("[LOAD_CONFIG] ENGINE_HOST = " << value);
            engine_host_ = value;
          } else if (key.compare("ENGINE_PORT") == 0) {
            MCE_INFO("[LOAD_CONFIG] ENGINE_PORT = " << value);
            engine_port_ = boost::lexical_cast<int>(value);
          } else if (key.compare("env_test") == 0) {
            env_test_ = boost::lexical_cast<bool>(value);
            MCE_INFO("[LOAD_CONFIG] _ = " << env_test_);
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
