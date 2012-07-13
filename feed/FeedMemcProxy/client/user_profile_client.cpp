
#include "user_profile_client.h"

#include <boost/lexical_cast.hpp>

#include "FeedContent.pb.h"
#include "compress_wrap.h"

//#include "ServiceI.h"
#include "LogWrapper.h"

using namespace xce::feed;

namespace xce {
namespace ad {

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

//static char * kEmptyProfile = "EMPTY_INFO";

void GetUserKey(Ice::Int uid, char * key, size_t len) {
  snprintf(key, len, "ADUP#%d", uid);  
  key[len - 1] = 0;
}

UserProfileClient::UserProfileClient() {
  pool_.Init(50);
}

UserProfileClient::~UserProfileClient() {
}

void UserProfileClient::Serialize(const UserProfile& profile, string * serialized) {
  profile.SerializeToString(serialized);
}

void UserProfileClient::Deserialize(const string & serialized, UserProfile * profile) {
  profile->ParseFromString(serialized);
}

bool UserProfileClient::GetSerialized(Ice::Int uid, string * serialized) {
  char key[32];
  GetUserKey(uid, key, 32);
  pair<int, MemcachedClient*> cli = pool_.GetClient(uid); 
  if(!cli.second){
    return false;
  }
  vector<string> keys;
  keys.push_back(key);
  map<string, string> res;
  map<string, int> flags;
  cli.second->Get(keys, res, flags);
  pool_.ReleaseClient(cli.first, cli.second);
  if(res.size()!=1){
    return false;
  }
  serialized->assign(res[key].c_str(), res[key].size());
  return true;
}

bool UserProfileClient::Get(Ice::Int uid, UserProfile * profile) {
  string serialized;
  if (!GetSerialized(uid, &serialized)) {
    return false;
  }
  profile->ParseFromString(serialized);
  return true;
}


bool UserProfileClient::Set(const UserProfile & o) {
  char key[32];
  GetUserKey(o.id(), key, 32);
   
  string value;
  o.SerializeToString(&value);

  pair<int, MemcachedClient*> cli = pool_.GetClient(o.id()); 
  bool res = cli.second->Set(key, value, 0);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}

}
}

