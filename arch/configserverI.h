#ifndef XCE_ARCH_CONFIGSERVERI_H__
#define XCE_ARCH_CONFIGSERVERI_H__

#include <set>
#include <boost/thread/mutex.hpp>
#include "base/hashmap.h"
#include "arch/configserver.h"

namespace xce {

class ThreadPool;

class ConfigServerI : public xce::ConfigServer {
public:
  ConfigServerI();
  virtual ~ConfigServerI();
  
  void ApplyConfig(const std::string& key, const std::string& value) {}
  
  virtual void Subscribe(const std::string& s, const ConfigListenerPrx& p, const Ice::Current& c = Ice::Current());
  virtual void Unsubscribe(const std::string&, const ConfigListenerPrx&, const Ice::Current& c = Ice::Current());

  virtual std::string Query(const std::string&, const Ice::Current& = Ice::Current()) const;
  virtual ConfigContent BatchQuery(const std::string&, const Ice::Current& = Ice::Current()) const;

  virtual void Update(const std::string&, const std::string&, const Ice::Current& = Ice::Current());
  virtual void Updates(const StringList&, const StringList&, const Ice::Current& = Ice::Current());

  std::string content_file_path() const;
  std::string listener_file_path() const;

  // -- load/store
  void LoadListener();
  void StoreListener();

  void LoadContent();
  void StoreContent();

private:
  void Init();

  //
  typedef std::map<std::string, std::string> ContentMap;

  ContentMap content_map_;
  mutable boost::mutex content_mutex_;
  bool content_changed_;  // 是否有更改没有序列化

  template<typename T>
  struct less_by_id : public std::binary_function<T, T, bool> {
    bool operator()(const T& left, const T & right) const {
      return left->ice_getIdentity() < right->ice_getIdentity();
    }
  };

  //
  typedef std::set<ConfigListenerPrx, less_by_id<ConfigListenerPrx> > ListenerSet;
  typedef std::hash_map<std::string, ListenerSet> ListenerMap;

  ListenerMap listener_map_;
  boost::mutex listener_mutex_;
  bool listener_changed_; // 是否有更改没有序列化

  // 
  ThreadPool* pool_;
  
  // --- for validate
  // 协助validate进程优雅退出
  volatile bool quit_;

  // 防止ValidateProc 重入
  volatile bool validating_;

  void ValidateProc(); // 定时检查 listener proxy 的有效性
  
  ListenerSet suspected_listeners_; // 可能有问题的 listeners
};

}
#endif // XCE_ARCH_CONFIGSERVERI_H__
