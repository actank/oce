#include "xoa_registry_client.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>
#include "LogWrapper.h"

static void DnsQuery(const char * hostname, std::string * resolved) {
  struct hostent *hp = gethostbyname(hostname);
  if (hp) {
    unsigned int i = 0;
    while( hp -> h_addr_list[i] != NULL) {
      if (i > 0) {
        *resolved += ',';
      }

      *resolved += inet_ntoa( *( struct in_addr*)( hp -> h_addr_list[i]));
      *resolved += ":2181";
      i++;
    }
  }
}


void XoaRegistryClient::AddNodeListener(const char * path, ZkEventListenerPtr listenter) {
  MCE_DEBUG(path << " add node listener.");
  boost::mutex::scoped_lock(node_listener_mutex_);
  node_listeners_[path].insert(listenter);
}

void XoaRegistryClient::AddChildListener(const char * path, ZkEventListenerPtr listenter) {
  MCE_DEBUG(path << " add child listener.");
  boost::mutex::scoped_lock(child_listener_mutex_);
  child_listeners_[path].insert(listenter);
}

// TODO : 回调中使用的对象，可能被销毁！。。。。所以必须使用shared_ptr
void XoaRegistryClient::ChildWatcher(zhandle_t *zzh, int type, int state, 
    const char *path, void * watcher_ctx) {
  XoaRegistryClient * client = reinterpret_cast<XoaRegistryClient *>(watcher_ctx); 
  // 是一个独立的线程在监控事件
  client->OnChildEvent(path, type);
}

void XoaRegistryClient::OnChildEvent(const char * path, int type) {
  MCE_INFO("zookeeper child changed " << path);
  std::vector<std::string> children;
  GetChildren(path, &children);

  {
    boost::mutex::scoped_lock(child_listener_mutex_);

    std::map<std::string, std::set<ZkEventListenerPtr> >::iterator it = child_listeners_.find(path);
    if (it != child_listeners_.end()) {
      std::set<ZkEventListenerPtr>::iterator i = it->second.begin();
      // TODO : 同步调用，性能上可能不够好
      while(i != it->second.end()) {
        boost::shared_ptr<ZkEventListener> sp = (*i).lock();
        if (sp) {
          MCE_DEBUG("handle child event : " << path);
          sp->HandleEvent(type);
          ++i;
        } else {
          MCE_DEBUG("remove null child listener : " << path);
          it->second.erase(i++);
        }
      }
    } else {
      MCE_DEBUG("Path no child listeners : " << path);
    }
  }
}

// TODO : 回调中使用的对象，可能被销毁！。。。。所以必须使用shared_ptr
void XoaRegistryClient::NodeWatcher(zhandle_t *zzh, int type, int state, 
    const char *path, void * watcher_ctx) {
  XoaRegistryClient * client = reinterpret_cast<XoaRegistryClient *>(watcher_ctx); 
  // 是一个独立的线程在监控事件
  client->OnNodeEvent(path, type);
}

void XoaRegistryClient::OnNodeEvent(const char * path, int type) {
  MCE_INFO("zookeeper node changed " << path);
  // 通过发起下一次zk监听。有没有更优雅点的方法？
  std::string value;
  Get(path, &value);

  {
    boost::mutex::scoped_lock(node_listener_mutex_);

    std::map<std::string, std::set<ZkEventListenerPtr> >::iterator it = node_listeners_.find(path);
    if (it != node_listeners_.end()) {
      std::set<ZkEventListenerPtr>::iterator i = it->second.begin();
      // TODO : 同步调用，性能上可能不够好
      while(i != it->second.end()) {
        boost::shared_ptr<ZkEventListener> sp = (*i).lock();
        if (sp) {
          MCE_DEBUG("handle node event : " << path);
          sp->HandleEvent(type);
          ++i;
        } else {
          MCE_DEBUG("remove null node listener : " << path);
          it->second.erase(i++);
        }
      }
    } else {
      MCE_DEBUG("Path no node listeners : " << path);
    }
  }
}

bool XoaRegistryClient::Init() {
  if (zhandle_) {
    zookeeper_close(zhandle_);
  }
  if (server_.empty()) {
    return false;
  }

  std::string addrs;
  if (isdigit(server_[0])) {
    addrs = server_;
  } else {
    DnsQuery(server_.c_str(), &addrs);
  }
  if (addrs.empty()) {
    return false;
  }
  zhandle_ = zookeeper_init(addrs.c_str(), NULL, 10000, 0, NULL, 0);

  MCE_INFO("connect zk server : " << addrs << " result : " << (zhandle_ != NULL));
  return zhandle_ != NULL;
}

bool XoaRegistryClient::GetChildren(const char * path, std::vector<std::string> * children) {
  if (!zhandle_) {
    if (!Init()) {
      return false;
    }
  }

  struct String_vector str_vec;
  int rc = zoo_wget_children(zhandle_, path, &XoaRegistryClient::ChildWatcher, reinterpret_cast<void*>(this), &str_vec);
  // int rc = zoo_get_children(zhandle_, path, 0, &str_vec);
  if (rc != 0) {
    MCE_WARN("zoo_wget_children() error. path=" << path << " rc=" << rc);
    return false;
  }
  for(int i = 0; i < str_vec.count; ++i) {
    children->push_back(str_vec.data[i]);
  }
  deallocate_String_vector(&str_vec);
  return true;
}

bool XoaRegistryClient::Get(const char * path, std::string * value) {
  if (!zhandle_) {
    if (!Init()) {
      return false;
    }
  }

  static const int ZK_BUF_LEN = 2048;
  // TODO : 将get的结果缓存在本地
  struct Stat stat;
  char * zk_buf = new char[ZK_BUF_LEN];
  zk_buf[ZK_BUF_LEN - 1] = '\0';
  int buflen = ZK_BUF_LEN - 1;

  int rc = zoo_wget(zhandle_, path, &XoaRegistryClient::NodeWatcher, reinterpret_cast<void*>(this), zk_buf, &buflen, &stat);
  if (rc) {
    MCE_WARN("zoo_wget() error. path=" << path << " rc=" << rc);
  } else {
    zk_buf[buflen] = '\0';
    *value = zk_buf;
  }

  delete [] zk_buf;

  return rc == 0;
}

