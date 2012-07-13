// #ifndef _CLIENT_POOL_H_
// #define _CLIENT_POOL_H_

#include <list>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "xoa2/client/xoa_client_factory.h"
#include "LogWrapper.h"

class ShardingStrategy {
 public:
  virtual int Shard(int factor) const = 0;
  virtual ~ShardingStrategy() {} 
};

class ModSharding : public ShardingStrategy {
 public:
  virtual int Shard(int factor) const {
    return factor % 10;
  }
  virtual ~ModSharding() {} 
};
 
// 可以是任意client类型, 经ClientWrap封装后放在pool中
template <typename ClientType>
class ClientPool : public ZkEventListener {
  ClientFactory<ClientType> * factory_;

  // 每个shard, 对应一组地址, 以及各组地址当前的load balance index
  std::vector<std::pair<size_t, std::vector<std::string> > > server_addrs_;
  boost::shared_mutex addr_mutex_;

  // 各个地址，对应的client组
  typedef boost::shared_ptr<ClientWrap<ClientType> > ClientWrapPtr;

  typedef std::map<std::string, std::list<ClientWrapPtr> > PoolType;
  PoolType pool_;

  // boost::mutex pool_mutex_;
  boost::shared_mutex pool_mutex_;

  typedef std::map<ClientType *, ClientWrapPtr> AllocatedMap;
  AllocatedMap allocated_pool_;

  // boost::mutex allocated_mutex_;
  boost::shared_mutex allocated_mutex_;

  ShardingStrategy * sharding_strategy_;

  XoaRegistryClient registry_client_;

  std::string service_name_;

 public:
  ClientPool(const char * registry_addr, const char * service_name, ShardingStrategy * sharding = NULL)
    : factory_(NULL)
    , sharding_strategy_(sharding)
    , registry_client_(registry_addr)
    , service_name_(service_name) {
    factory_ = new XoaClientFactory<ClientType>(service_name);
  }

  virtual ~ClientPool() {
  }

  virtual void HandleEvent(int type) {
    MCE_INFO("ClientPool reload config on zk event ");
    InitConfig();
  }

  void ParseProperty(const std::string & prop_str, std::map<std::string, std::string> * res) {
    std::vector<std::string> lines;
    boost::split(lines, prop_str, boost::is_any_of("\r\n"), boost::token_compress_on);
    for(size_t i = 0; i < lines.size(); ++i) {
      size_t pos = lines[i].find_first_of('=');
      std::string key = lines[i].substr(0, pos);
      boost::trim(key);
      std::string value = lines[i].substr(pos + 1);
      boost::trim(value);
      if (pos != std::string::npos) {
        res->insert(std::make_pair(key, value));
      }
    }
  }

  void FilterServerAddrs(const std::vector<std::string> & addrs, std::vector<std::string> * res) {
    std::string value;
    std::string shard_path = GetShardPath(service_name_, 0, 0);
    for(size_t i = 0; i < addrs.size(); ++i) {
      if (registry_client_.Get((shard_path + '/' + addrs[i]).c_str(), &value)) {
        std::map<std::string, std::string> props;
        ParseProperty(value, &props);
        // TODO : read-only 访问
        if (props["status"] == "enabled") {
          res->push_back(addrs[i]);
          MCE_INFO(service_name_ << " server enabled : " << addrs[i]);
        } else {
          MCE_INFO(service_name_ << " server disabled : " << addrs[i]);
        }
      } else {
        MCE_WARN(service_name_ << " server status error : " << addrs[i]);
      }
    }
  }

  // 加载服务地址列表
  void SetServerAddrs(const std::vector<std::string> & addrs) {
    std::vector<std::string> active_addrs;
    // 只启用enabled状态的服务
    FilterServerAddrs(addrs, &active_addrs);

    if (active_addrs.empty()) {
      MCE_WARN(service_name_ << " no active server!");
    }
    
    {
      boost::unique_lock<boost::shared_mutex> wlock(addr_mutex_);
      if (server_addrs_.empty()) {
        server_addrs_.push_back(std::make_pair(0, active_addrs));
      } else {
        server_addrs_[0].second.swap(active_addrs);
      }
    }
    
    // 清掉无效的端口
    {
      std::set<std::string> addr_set;
      for(size_t i = 0; i < active_addrs.size(); ++i) {
        addr_set.insert(active_addrs[i]);
      }

      // boost::mutex::scoped_lock(pool_mutex_);
      boost::unique_lock<boost::shared_mutex> wlock(pool_mutex_);
      typename PoolType::iterator it = pool_.begin();
      while(it != pool_.end()) {
        if (addr_set.find(it->first) == addr_set.end()) {
          MCE_WARN("server removed : " << it->first);
          pool_.erase(it++);
        } else {
          ++it;
        }
      }
    }
  }

  std::string GetShardPath(const std::string & service_name, int group, int shard) {
    if (service_name.empty()) {
      return "";
    }
    std::stringstream shard_0;
    shard_0 << service_name;
    if (*service_name.rbegin() != '/') {
      shard_0 << '/';
    }
    shard_0 << ".service_group_" << group << "/shard_" << shard;

    return shard_0.str();
  }

  bool InitConfig() {
    if (service_name_.empty()) {
      return false;
    }

    MCE_INFO("Reload server config.");
    std::string config;
    if (!registry_client_.Get(service_name_.c_str(), &config)) {
      return false;
    }
    // .service_group兄弟节点有增删时，通知
    registry_client_.AddChildListener(service_name_.c_str(), shared_from_this());

    std::string shard_path = GetShardPath(service_name_, 0, 0);
    std::vector<std::string> children;
    if (!registry_client_.GetChildren(shard_path.c_str(), &children)) {
      return false;
    }
    
    // ip:port 节点有增删时，通知
    registry_client_.AddChildListener(shard_path.c_str(), shared_from_this());
    
    for(size_t i = 0; i < children.size(); ++i) {
      // ip:port 节点的值有变化时，通知
      registry_client_.AddNodeListener((shard_path + '/' + children[i]).c_str(), shared_from_this());
    }
    SetServerAddrs(children);
    return true;
  }

  std::string ShardServerAddr(size_t shard) {
    boost::shared_lock<boost::shared_mutex> rlock(addr_mutex_);
    // TODO: add read lock
    if (server_addrs_.empty()) {
      // return "127.0.0.1:10093";
      return "";
    }
    size_t shard_idx = shard % server_addrs_.size();
    size_t lb_idx = ++ server_addrs_[shard_idx].first;

    if (server_addrs_[shard_idx].second.empty()) {
      // 没有 enabled server 的情况
      return "";
    }
    return server_addrs_[shard_idx].second[lb_idx % server_addrs_[shard_idx].second.size()];
  }

  ClientType * Alloc(int sharding) {
    int shard = 0;
    if (sharding_strategy_) {
      // shard = sharding_strategy_->Shard(sharding);
    }

    std::string server_addr = ShardServerAddr(shard);
    if (server_addr.empty()) {
      return NULL;
    }

    ClientWrapPtr wrap;
    {
      // boost::mutex::scoped_lock(pool_mutex_);
      boost::unique_lock<boost::shared_mutex> wlock(pool_mutex_);
      std::list<ClientWrapPtr> & resources = pool_[server_addr];

      while (!resources.empty()) {
        wrap = resources.front();
        resources.pop_front();
        if (!wrap->disabled()) {
          // MCE_DEBUG("Cached client reused : " << wrap->client());
          break;
        } else {
          wrap.reset();
        }
      }
    }

    if (!wrap) {
      wrap.reset(factory_->Create(server_addr.c_str()));
      if (wrap) {
        registry_client_.AddNodeListener((GetShardPath(service_name_, 0, 0) + server_addr).c_str(), ZkEventListenerPtr(wrap));
      }
    }

    if (!wrap || !wrap->client()) {
      return NULL;
    }

    {
      // boost::mutex::scoped_lock(allocated_mutex_);
      boost::unique_lock<boost::shared_mutex> wlock(allocated_mutex_);

      typename AllocatedMap::iterator it = allocated_pool_.find(wrap->client());
      if (it != allocated_pool_.end()) {
        // TODO: 致命错误！
        // MCE_DEBUG("Client collision : " << wrap->client());
      }

      pair<typename AllocatedMap::iterator,bool> i_res = allocated_pool_.insert(std::make_pair(wrap->client(), wrap));
      if(!i_res.second) {
        // MCE_DEBUG("Client insert error : " << wrap->client());
        return NULL;
      }
    }

    return wrap->client();
  }

  void Release(ClientType * client) {
    if (client == NULL) {
      return;
    }
    ClientWrapPtr wrap;

    // MCE_DEBUG("Client released 1 : " << client);
    {
      // boost::mutex::scoped_lock(allocated_mutex_);
      boost::unique_lock<boost::shared_mutex> wlock(allocated_mutex_);

      typename AllocatedMap::iterator it = allocated_pool_.find(client);
      if (it == allocated_pool_.end()) {
        // TODO: 致命错误！
        // MCE_DEBUG("Client not found : " << client);
        return;
      }
      
      wrap = it->second;
      allocated_pool_.erase(it);
    }

    if (!wrap->disabled()) {
      // boost::mutex::scoped_lock(pool_mutex_);
      boost::unique_lock<boost::shared_mutex> wlock(pool_mutex_);

      typename PoolType::iterator it = pool_.find(wrap->load_balance_key());
      if (it != pool_.end()) {
        std::list<ClientWrapPtr> & resources = it->second;
        
        if (resources.size() < 100) {
          // MCE_DEBUG("Client cached : " << wrap);
          resources.push_back(wrap);
        }
      }
    }
  }
};

// #endif // _CLIENT_POOL_H_

