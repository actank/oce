#include "MemcachedClient.h"
#include "IceLogger.h"
#include <set>
#include <map>

using namespace std;

class MemcachedClientPool{
public:
  MemcachedClientPool(io_service& io_service, const ip::tcp::endpoint& endpoint);
  MemcachedClient*  GetClient();
  void              ReleaseClient(MemcachedClient* cli);
  void              ResetClient(MemcachedClient* cli);
  void              ReleaseErrClient(MemcachedClient* cli);
  void              HandleErrClient();
  int FreeCount(){
    list<MemcachedClient*>::iterator it = free_client_.begin();
    //for(; it!=free_client_.end(); ++it){
    //  MCE_INFO("@@@@ --> " << *it);
    //}
    return free_client_.size();
  }
private:
  ip::tcp::endpoint       endpoint_;
  io_service&             io_service_;
  list<MemcachedClient*>  free_client_;
  map<MemcachedClient*, int> err_client_;
};

class PoolManager{
public:
  PoolManager(io_service& io_service, int index):io_service_(io_service),index_(index){}
  MemcachedClient*  GetClient(const ip::tcp::endpoint& ep);
  void              ReleaseClient(MemcachedClient* cli);
  void              PrintStatInfo();
private:
  io_service& io_service_;
  int index_;
  map<ip::tcp::endpoint, MemcachedClientPool*> pool_;
};
