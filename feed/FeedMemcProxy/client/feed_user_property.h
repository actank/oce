
#ifndef _XCE_USER_PROFILE_CLIENT_H_
#define _XCE_USER_PROFILE_CLIENT_H_

#include <Ice/Ice.h>

#include "MemcachedClientPool.h"
#include "Singleton.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedUserPropertyData{
public:
  FeedUserPropertyData(const string& d) : data_(d){

  }
  FeedUserPropertyData(){
  }
  string& GetOriginalData(){
    return data_;
  }

  Ice::Long  GetMaxFeedId(){
    return GetData(0); 
  }
  Ice::Long  GetMaxFavFeedId(){
    return GetData(1); 
  }
  Ice::Long  GetActiveFriendCount(){
    return GetData(2); 
  }


  Ice::Long  SetMaxFeedId(Ice::Long v){
    return SetData(0, v); 
  }
  Ice::Long  SetMaxFavFeedId(Ice::Long v){
    return SetData(1, v); 
  }
  Ice::Long  SetActiveFriendCount(Ice::Long v){
    return SetData(2, v); 
  }

  
  Ice::Long  GetData(int i){
    const char* p = data_.c_str();
    int len = data_.size();
    printf("%d\n", len);
    if((size_t)i < (len/(sizeof(Ice::Long)))){
      return *(((Ice::Long*)p) + i);
    }
    return 0;
  }
  bool SetData(int i, Ice::Long v){
    const char* p = data_.c_str();
    int len = data_.size();
    printf("%d\n", len);
    if((size_t)i < (len/(sizeof(Ice::Long)))){
      *(((Ice::Long*)p) + i) = v;
      return true;
    }else{
      printf("resize %d\n", len);
      int d = ((i+1)*sizeof(Ice::Long)) - len;
      char buf[1024] = {0};
      if(d>1024){
        return false;
      }
      data_.insert(data_.begin(), buf, buf+d);
      return SetData(i, v); 
    }
  }
private:
  string data_; 
};


class FeedUserPropertyClient : public Singleton<FeedUserPropertyClient> {
public:
  FeedUserPropertyClient();
  ~FeedUserPropertyClient();

  FeedUserPropertyData Get(Ice::Int uid);
  bool Set(int uid, FeedUserPropertyData& data);
private:
  PoolManager pool_;
};

}
}
#endif // _XCE_USER_PROFILE_CLIENT_H_
