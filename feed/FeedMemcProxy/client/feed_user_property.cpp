#include "feed_user_property.h"
#include <boost/lexical_cast.hpp>

#include "LogWrapper.h"

using namespace xce::feed;

namespace xce {
namespace feed{

class StatFunc {
  string name_;
  timeval start_, end_;
public:
  StatFunc(string name) : name_(name) {
    gettimeofday(&start_, NULL);
  }

  ~StatFunc() {
    gettimeofday(&end_, NULL);
    float timeuse = 1000000 * (end_.tv_sec - start_.tv_sec) + end_.tv_usec - start_.tv_usec;
    timeuse /= 1000;
    if (timeuse >= 5) {
      MCE_INFO(name_ << " time usage : "<< timeuse << " ms");
    } else {
      MCE_DEBUG(name_ << " time usage : "<< timeuse << " ms");
    }
  }
};

void GetUserKey(Ice::Int uid, char * key, size_t len) {
  snprintf(key, len, "FUP#%d", uid);  
  key[len - 1] = 0;
}

FeedUserPropertyClient::FeedUserPropertyClient() {
  pool_.Init(50);
}

FeedUserPropertyClient::~FeedUserPropertyClient() {
}


FeedUserPropertyData FeedUserPropertyClient::Get(Ice::Int uid){
  char key[32];
  GetUserKey(uid, key, 32);
  pair<int, MemcachedClient*> cli = pool_.GetClient(uid); 
  if(!cli.second){
    return FeedUserPropertyData();
  }
  vector<string> keys;
  keys.push_back(key);
  map<string, string> res;
  map<string, int> flags;
  cli.second->Get(keys, res, flags);
  pool_.ReleaseClient(cli.first, cli.second);
  if(res.size()!=1){
    return FeedUserPropertyData();
  }
  return FeedUserPropertyData(res[key]);
}

bool FeedUserPropertyClient::Set(int uid, FeedUserPropertyData& data){
  char key[32];
  GetUserKey(uid, key, 32);
  pair<int, MemcachedClient*> cli = pool_.GetClient(uid); 
  bool res = cli.second->Set(key, data.GetOriginalData(), 0);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}

}
}
/*int main(){
  char data[128] = {0};
  *(Ice::Long*)data = 1;
  *(Ice::Long*)(data+8) = 2;
  //*(Ice::Long*)(data+16) = 3;

  string s;
  s.insert(s.begin(), data, data+16);
  FeedUserPropertyData t(s);
  printf("%d %d %d\n", t.GetMaxFavFeedId(), t.GetMaxFeedId(), t.GetActiveFriendCount());
  t.SetActiveFriendCount(1321);
  printf("%d %d %d\n", t.GetMaxFavFeedId(), t.GetMaxFeedId(), t.GetActiveFriendCount());
}*/


