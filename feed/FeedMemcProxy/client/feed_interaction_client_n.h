
#ifndef _XCE_FEED_INTERACTION_CLIENT_N_H_
#define _XCE_FEED_INTERACTION_CLIENT_N_H_

#include "MemcachedClientPool.h"
#include "RFeed.h"
#include "Singleton.h"
#include <Ice/Object.h>
#include <IceUtil/Handle.h>
#include <set>
#include <boost/smart_ptr.hpp>
//#include "./MemcachedClient.h"
namespace xce {
namespace feed {

using namespace std;
using namespace MyUtil;

class FeedInter{
public:
	FeedInter(const string& d) : data_(d){ }
	FeedInter(){ }
  const string& GetOriginalData() const { return data_; }

  int GetReply() const { return GetData(0); }
  void SetReply(int r) { SetData(0,r); }
  int GetRange() const { return GetData(1); }
  void SetRange(int r) { SetData(1,r); }
  int  GetData(int i) const {
    const char* p = data_.c_str();
    int len = data_.size();
//    printf("%d\n", len);
    if((size_t)i < (len/(sizeof(int)))){
      return *(((int*)p) + i);
    }
    return 0;
  }
  bool SetData(int i, int v){
    const char* p = data_.c_str();
    int len = data_.size();
//    printf("%d\n", len);
    if((size_t)i < (len/(sizeof(int)))){
      *(((int*)p) + i) = v;
      return true;
    }else{
//      printf("resize %d\n", len);
      int d = ((i+1)*sizeof(int)) - len;
      char buf[1024] = {0};
      if(d>1024){
        return false;
      }
      data_.insert(data_.end(), buf, buf+d);
      return SetData(i, v);
    }
  }
private:
  string data_;
};

//typedef IceUtil::Handle<FeedInter> FeedInterPtr;
typedef boost::shared_ptr<FeedInter> FeedInterPtr;
typedef map<long,FeedInterPtr> FeedInterMap;

class FeedInteractionMemcClientN :  public Singleton<FeedInteractionMemcClientN> {
public:
  FeedInteractionMemcClientN();
  ~FeedInteractionMemcClientN(){}

  bool GetFeedInterMap(const set<long> & fids,
  		FeedInterMap & res	);
  bool SetFeedInter(long fid,const FeedInterPtr & inter);
protected:
  bool GetValues(long idx,const vector<string> & keys,
  		map<string,string> & kvs,
  		map<string,int> & flags) ;

  long ParseFid(const string & key);
  string MakeKey(long fid);
  void Parse(const map<string,string> & kvs, FeedInterMap & res);

	const int kHeadSize_;
  PoolManager pool_;
};

}
}
#endif //_XCE_FEED_INTERACTION_CLIENT_H_
