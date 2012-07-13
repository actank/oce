#ifndef ZKCLIENT_H_
#define ZKCLIENT_H_

#include <map>
#include <string>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <zookeeper/zookeeper.h>

namespace com {
namespace renren {
namespace messageconfig {

//class ZkClientFactory;

class NamespaceListener {
 public:
  explicit NamespaceListener(const std::string&);
  virtual bool Update(const std::list<std::string>& children_names) = 0;
  virtual ~NamespaceListener();

  std::string ns() const {
    return ns_;
  }
 private:
  std::string ns_; //
};

typedef boost::shared_ptr<NamespaceListener> NamespaceListenerPtr;

class ZkClient;
typedef boost::shared_ptr<ZkClient> ZkClientPtr;

class ZkClient {
 public:
  explicit ZkClient(const std::string& zk_address);
  virtual ~ZkClient();
  void AddMessage(const std::string&);
  void NotifyInitialize();
  bool Update(const std::string&);
  bool GetNodes(const std::string& ns, std::list<std::string>* );

  void AddNamespaceListener(NamespaceListenerPtr listener);

  bool reinit_flag() const {
   return reinit_flag_;
  }
  void set_reinit_flag(bool f) {
    reinit_flag_ = f;
  }
 public:
  static const std::string kUpdateAllNamespace;
  static const std::string kDefaultPrefix;
  static std::map<std::string, ZkClientPtr> address_zk_client_map_;
  static std::map<zhandle_t*, ZkClientPtr> zhandle_zk_client_map_;
  static boost::mutex zk_client_map_mutex_;

  static ZkClientPtr GetInstance(const std::string& zk_address);
  static std::string GetNamespaceZnodePath(const std::string&);
 private:
  bool Initialize();
  bool Reinitialize();

  void NotifyThreadFunction();
  bool UpdateAllNamespaces();
  bool UpdateNamespace(const std::string&);

  bool GetNamespaceVersion(const std::string&, std::string*);

  bool GetDataFromZk(const std::string&, std::string*);
  
  bool GetChildrenNamesFromZk(const std::string&, std::list<std::string>*);

  static void ZkWatcher(zhandle_t *zzh, int type, int state, const char *path, void *watcher_ctx);
 private:
  boost::shared_mutex namespace_listener_mutex_;
  std::map<std::string, std::list<NamespaceListenerPtr> > listeners_;

  boost::mutex znode_version_mutex_;
  std::map<std::string, std::string> znode_version_map_;

  bool reinit_flag_;

  zhandle_t* zk_;
  boost::mutex zk_mutex_;
  boost::condition_variable zk_cond_;
  std::string zk_address_;

  boost::mutex message_deque_mutex_;
  boost::shared_ptr<std::deque<std::string> > message_deque_;
  boost::condition_variable message_deque_cond_;
  boost::thread notify_thread_;
};



}}} // namespace com::renren::messageconfig

#endif

