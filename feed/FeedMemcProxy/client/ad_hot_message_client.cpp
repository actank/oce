#include "ad_hot_message_client.h"

#include <boost/lexical_cast.hpp>

//#include "ServiceI.h"
#include "LogWrapper.h"

//using namespace xce::feed;

namespace xce {
namespace ad {

static void GetMessageKey(Ice::Int uid, Ice::Int ad_id, char * key, size_t len) {
  snprintf(key, len, "ADUP#%d-%d", ad_id, uid);  
  key[len - 1] = 0;
}


AdHotMessageClient::AdHotMessageClient() {
  pool_.Init(50);
}

AdHotMessageClient::~AdHotMessageClient() {
}


bool AdHotMessageClient::Get(Ice::Int uid, Ice::Int ad_id, string * message) {
  char key[32];
  GetMessageKey(uid, ad_id, key, 32);
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
  message->assign(res[key].c_str(), res[key].size());
  return true;
}

bool AdHotMessageClient::Set(Ice::Int uid, Ice::Int ad_id, const string & message) {
  char key[32];
  GetMessageKey(uid, ad_id, key, 32);
  pair<int, MemcachedClient*> cli = pool_.GetClient(uid); 
  bool res = cli.second->Set(key, message, 0, time(NULL) + 60 * 60 * 24 * 14);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}

}
}

