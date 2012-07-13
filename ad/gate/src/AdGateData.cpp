#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include "IceLogger.h"
#include "AdGateData.h"


namespace xce {
namespace ad {
using namespace std;
using namespace boost;

void RefreshTime::Load(string time_str) {
  vector<int> zone_times;
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep1("+");
  tokenizer tokens1(time_str, sep1);
  for(tokenizer::iterator it1 = tokens1.begin(); it1 != tokens1.end(); ++it1) {
    boost::char_separator<char> sep2("*");
    tokenizer tokens2(*it1, sep2);
    int ms = 0;
    int number = 1;
    int i = 0;
    for(tokenizer::iterator it2 = tokens2.begin(); it2 != tokens2.end() && i < 2; ++it2, ++i) {
      if(i == 0) {
        ms = boost::lexical_cast<int>(*it2);
      } else if(i == 1) {
        number = boost::lexical_cast<int>(*it2);
      }
    }
    //插入number个ms
    if(ms > 0 && number > 0) {
      for(int j = 0; j < number; ++j) {
        zone_times.push_back(ms);
      }
    }
  }

  zone_times.push_back(600000000);
  times_[100000000001] = zone_times;
  for(map<Ice::Long, vector<int> >::iterator mit = times_.begin();
      mit != times_.end(); ++mit) {
    ostringstream oss;
    oss <<"RefreshTime::Load() --> adzone_id = " << mit->first << ", next_reload_time = [";
    for(vector<int>::iterator vit = mit->second.begin(); vit != mit->second.end(); ++vit) {
      oss << " " << *vit;
    }
    oss << " ]";
    MCE_INFO(oss.str());
  }
};

int RefreshTime::next_load_time(Ice::Long zone_id, int refresh_index) {
  vector<int> zone_times;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    zone_times = times_[zone_id];
  }
  int len = zone_times.size();
  if(len > 0 && refresh_index >= 0) {
    return zone_times[refresh_index%len];
  } else {
    return 600000000;
  }
};


}
}
