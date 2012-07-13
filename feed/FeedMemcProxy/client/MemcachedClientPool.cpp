#include "MemcachedClientPool.h"
#include "FeedMemcProxyAdapter.h"
#include "IceLogger.h"
#include <boost/lexical_cast.hpp>

using namespace xce::feed;
using namespace boost;

MemcachedClient* MemcachedClientPool::GetClient(){
  //IceUtil::Mutex::Lock lock(mutex_);
  MemcachedClient* cli = NULL;
  if(!free_client_.empty()){
    cli = free_client_.front();
    free_client_.pop_front();
  }
  if(!cli){
    cli = new MemcachedClient(ip_, port_, timeout_);
  }
  return cli;
}

void MemcachedClientPool::ReleaseClient(MemcachedClient* cli){
  //IceUtil::Mutex::Lock lock(mutex_);
  if(!cli->Connected()){
    delete cli;
    return;
  }
  string ip = cli->GetIp();
  int port = cli->GetPort();
  if(ip!=ip_ || port!=port_){
    MCE_WARN("MemcachedClientPool::ReleaseClient --> invalid old:" << ip << " " << port << "  now:" << ip_ << " " << port_);
    delete cli;
    return;
  }
  free_client_.push_back(cli);
}

void MemcachedClientPool::Clear(){
  list<MemcachedClient*>::iterator it = free_client_.begin();
  for(; it!=free_client_.end(); ++it){
    delete *it;
  }
  free_client_.clear();
}

void PoolManager::CheckValid(){
  int cluster = FeedMemcProxyAdapter::instance().GetClusterSize();
  if(cluster == 0){
    cluster = 1;
  }
  multimap<int, int> sorted;
  for(int i=0; i<cluster; i++){
    //string endpoint;
    ProxyInfoPtr info;
    try{
      info = FeedMemcProxyAdapter::instance().GetProxyInfo(i);
    }catch(Ice::Exception& e){
      MCE_WARN("PoolManager::CheckInvalid --> get endpoint err " << i << " " << e);
      continue;
    }
    if(!info){
      MCE_WARN("PoolManager::CheckInvalid --> get endpoint err " << i);
      continue;
    }
    sorted.insert(pair<int, int>(info->connections, i));
    IceUtil::Mutex::Lock lock(mutex_);
    string oldip;
    int oldport = 0;
    int conn = 0;
    MemcachedClientPool* oldpool = NULL;
    if(pool_.count(i)>0){
      oldip = pool_[i]->GetIp();
      oldport = pool_[i]->GetPort();
      oldpool = pool_[i];
      conn = oldpool->FreeCount();
    }
    MCE_INFO("PoolManager::CheckInvalid --> proxy, index:" << i << "  ip:" << info->ip << " port:" << info->port 
              << " connections:" << info->connections << " " << conn);
    if(oldip!=info->ip || oldport!=info->port){
      MCE_INFO("PoolManager::CheckInvalid --> proxy changed, old:" << oldip << " " << oldport << "  now:" << info->ip << " " << info->port);
      MemcachedClientPool* p = new MemcachedClientPool(i, info->ip, info->port, timeout_);
      pool_[i] = p;
      if(oldpool){
        oldpool->Clear();
        delete oldpool;
      }
    }
  }
  MCE_INFO("PoolManager::CheckInvalid --> " << sorted.size() << " " << pool_.size());
  vector<int> tmp_valid;
  map<int, int>::iterator it = sorted.begin();
  int limit = cluster*0.8 + 1;
  for(; it!=sorted.end(); ++it){
    if(tmp_valid.size()<limit){
      tmp_valid.push_back(it->second);
    }else{
      IceUtil::Mutex::Lock lock(mutex_);
      pool_[it->second]->Clear();
      MCE_INFO("PoolManager::CheckInvalid --> not use " << it->second);
    }
  }
  IceUtil::Mutex::Lock lock(mutex_);
  valid_.swap(tmp_valid);
}

bool PoolManager::Init(int timeout){
  timeout_ = timeout;
  CheckValid(); 
  TaskManager::instance().scheduleRepeated(this);
}

pair<int, MemcachedClient*> PoolManager::GetClient(Ice::Long index){
  IceUtil::Mutex::Lock lock(mutex_);
  if(valid_.size()==0){
    return pair<int, MemcachedClient*>(-1, NULL);
  }
  int s = index%valid_.size();
  //MCE_INFO("PoolManager::GetClient --> " << valid_.size() << " " << s);
  return pair<int, MemcachedClient*>(valid_[s], pool_[valid_[s]]->GetClient());
}

void PoolManager::ReleaseClient(Ice::Long index, MemcachedClient* cli){
  IceUtil::Mutex::Lock lock(mutex_);
  pool_[index]->ReleaseClient(cli);
}

void PoolManager::handle(){
  CheckValid(); 
  /*int cluster = FeedMemcProxyAdapter::instance().GetClusterSize();
  if(cluster == 0){
    cluster = 1;
  }
  for(int i=0; i<cluster; i++){
    string endpoint = FeedMemcProxyAdapter::instance().GetEndpoint(i);
    int pos = endpoint.find(":");
    string ip = endpoint.substr(0, pos);
    string strport = endpoint.substr(pos+1, endpoint.size()-pos-1);
    int port = lexical_cast<int>(strport);
    
    IceUtil::Mutex::Lock lock(mutex_);
    string oldip = pool_[i]->GetIp();
    int oldport = pool_[i]->GetPort();
    if(oldip!=ip || oldport!=port){
      MCE_INFO("PoolManager::handle --> proxy changed, old:" << oldip << " " << oldport << "  now:" << ip << " " << port);
      MemcachedClientPool* p = new MemcachedClientPool(i, ip, port, timeout_);
      MemcachedClientPool* tmp = pool_[i];
      pool_[i] = p;
      tmp->Clear();
      delete tmp;
    }
  }*/
}

void PoolManager::PrintStatInfo(){
/*  map<ip::tcp::endpoint, MemcachedClientPool*>::iterator it = pool_.begin();
  int total = 0;
  for(; it!=pool_.end(); ++it){
    total += it->second->FreeCount();
    //total += it->second->BusyCount();
    MCE_INFO("###UPCONN " << index_ << " " << it->first.address().to_string() << " free:" << it->second->FreeCount());
  }
  MCE_INFO("###UPCONN TOTAL " << index_ << " " << total);
*/
}
