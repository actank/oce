#ifndef _XOA_REGISTRY_CLIENT_H_
#define _XOA_REGISTRY_CLIENT_H_

#include "zookeeper.h"
#include "xoa2/client/zk_event_listener.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

class XoaRegistryClient {
 private:
  zhandle_t * zhandle_;
  std::string server_;

  static void NodeWatcher(zhandle_t *zzh, int type, int state, 
      const char *path, void *watcher_ctx);
  static void ChildWatcher(zhandle_t *zzh, int type, int state, 
      const char *path, void *watcher_ctx);

  void OnNodeEvent(const char * path, int type);
  void OnChildEvent(const char * path, int type);

  std::map<std::string, std::set<ZkEventListenerPtr> > node_listeners_;
  boost::mutex node_listener_mutex_;

  std::map<std::string, std::set<ZkEventListenerPtr> > child_listeners_;
  boost::mutex child_listener_mutex_;

 public:
  XoaRegistryClient(const char * hostname) 
      : zhandle_(NULL)
      , server_(hostname) {
  }

  void AddNodeListener(const char * path, ZkEventListenerPtr listenter);
  void AddChildListener(const char * path, ZkEventListenerPtr listenter);

  bool Init();
  bool Get(const char * path, std::string * value);
  bool GetChildren(const char * path, std::vector<std::string> * children);

  ~XoaRegistryClient() {
    zookeeper_close(zhandle_);
  }
};

#endif // _XOA_REGISTRY_CLIENT_H_

