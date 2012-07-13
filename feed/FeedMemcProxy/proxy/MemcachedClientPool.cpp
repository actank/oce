#include "MemcachedClientPool.h"
#include "IceLogger.h"

MemcachedClientPool::MemcachedClientPool(io_service& io_service, const ip::tcp::endpoint& endpoint):
io_service_(io_service){
  endpoint_ = endpoint;
}

MemcachedClient* MemcachedClientPool::GetClient(){
  MemcachedClient* cli = NULL;
  if(!free_client_.empty()){
    cli = free_client_.front();
    free_client_.pop_front();
  }
  if(!cli){
    cli = new MemcachedClient(io_service_, this, endpoint_);
  }
  return cli;
}

void MemcachedClientPool::ReleaseClient(MemcachedClient* cli){
  free_client_.push_back(cli);
}

void MemcachedClientPool::ReleaseErrClient(MemcachedClient* cli){
  err_client_[cli] = time(NULL);
}

void MemcachedClientPool::HandleErrClient(){
  int count = 0;
  vector<MemcachedClient*> seq;
  for(map<MemcachedClient*, int>::iterator it=err_client_.begin();
      it!=err_client_.end();){
    if(time(NULL) - it->second > 60){
      seq.push_back(it->first);
      err_client_.erase(it++);
    }else{
      ++it;
    }
  }
  for(int i=0; i<seq.size(); i++){
    delete seq.at(i);
  }
  MCE_INFO("MemcachedClientPool::HandleErrClient --> deling:" << err_client_.size() << " deled " << seq.size());
}

MemcachedClient* PoolManager::GetClient(const ip::tcp::endpoint& ep){
  map<ip::tcp::endpoint, MemcachedClientPool*>::iterator it = pool_.find(ep);
  if(it== pool_.end()){
    it = pool_.insert(make_pair<ip::tcp::endpoint, MemcachedClientPool*>(ep, new MemcachedClientPool(io_service_, ep))).first;
  }
  return it->second->GetClient();
}

void PoolManager::ReleaseClient(MemcachedClient* cli){
  ip::tcp::endpoint ep = cli->GetEndpoint();
  map<ip::tcp::endpoint, MemcachedClientPool*>::iterator it = pool_.find(ep);
  if( it!= pool_.end()){
    it->second->ReleaseClient(cli); 
  }
}

void PoolManager::PrintStatInfo(){
  map<ip::tcp::endpoint, MemcachedClientPool*>::iterator it = pool_.begin();
  int total = 0;
  for(; it!=pool_.end(); ++it){
    total += it->second->FreeCount();
    MCE_INFO("###UPCONN " << index_ << " " << it->first.address().to_string() << " free:" << it->second->FreeCount());
    it->second->HandleErrClient();
  }
  MCE_INFO("###UPCONN TOTAL " << index_ << " " << total);
}
