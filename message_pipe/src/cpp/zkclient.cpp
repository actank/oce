#include "zkclient.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "LogWrapper.h"

namespace com {
namespace renren {
namespace messageconfig {

static const char* State2String(int state){
    switch(state){
        case 0:
            return "ZOO_CLOSED_STATE";
        case 1:
            return "ZOO_CONNECTING_STATE";
        case 2:
            return "ZOO_ASSOCIATING_STATE";
        case 3:
            return "ZOO_CONNECTED_STATE";
        case -112:
            return "ZOO_EXPIRED_SESSION_STATE";
        case -113:
            return "ZOO_AUTH_FAILED_STATE";
    }
    return "INVALID_STATE";
}

static const char* WatcherEvent2String(const int ev){
    switch(ev){
        case 0:
            return "ZOO_ERROR_EVENT";
        case 1:
            return "ZOO_CREATED_EVENT";
        case 2:
            return "ZOO_DELETED_EVENT";
        case 3:
            return "ZOO_CHANGED_EVENT";
        case 4:
            return "ZOO_CHILD_EVENT";
        case -1:
            return "ZOO_SESSION_EVENT";
        case -2:
            return "ZOO_NOTWATCHING_EVENT";
    }
    return "INVALID_EVENT";
}

NamespaceListener::NamespaceListener(const std::string& ns) : ns_(ns) {

}

NamespaceListener::~NamespaceListener() {

}


std::map<std::string, ZkClientPtr> ZkClient::address_zk_client_map_;
std::map<zhandle_t*, ZkClientPtr> ZkClient::zhandle_zk_client_map_;
boost::mutex ZkClient::zk_client_map_mutex_;

const std::string ZkClient::kUpdateAllNamespace = "HOLY_SHIT_UPDATE_ALL_NAMESPACE";
const std::string ZkClient::kDefaultPrefix = "/MessagePipe";

ZkClient::ZkClient(const std::string& zk_address) : reinit_flag_(false), zk_address_(zk_address), zk_(NULL),
                       message_deque_(new std::deque<std::string>),
                       notify_thread_(boost::bind(&ZkClient::NotifyThreadFunction, this)) {

}

ZkClient::~ZkClient() {  
  MCE_INFO("ZkClient::~ZkClient");
  message_deque_.reset();
  message_deque_cond_.notify_all();
  notify_thread_.join();
  
  if (zk_) {
    zookeeper_close(zk_);
  }
}

void ZkClient::NotifyThreadFunction() {
  while (true) {
    if (!message_deque_) {
      MCE_DEBUG("deque_ is null, notify thread(" << boost::this_thread::get_id() << ") shuts down right now");
      break;
    }
    boost::unique_lock<boost::mutex> lock(message_deque_mutex_);
    if (message_deque_->size() == 0) {
      MCE_DEBUG("notify thread(" << boost::this_thread::get_id() << ") waits");
      message_deque_cond_.wait(lock);
    }
    else {
      std::string msg = message_deque_->front();
      message_deque_->pop_front();
      MCE_DEBUG("notify thread(" << boost::this_thread::get_id() << ") takes a message(" << msg << ")");
      bool ret = true;
      for (int i = 0; i < 3; ++i) {
        ret = Update(msg); 
        if (!ret) {
          boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        }
        else {
          break;
        }
      }
      if (!ret) {
        //加入队列
        std::deque<std::string>::const_iterator iter = std::find(message_deque_->begin(), message_deque_->end(), msg);
        if (iter == message_deque_->end()) {
          message_deque_->push_back(msg);
        }
        message_deque_cond_.notify_all();
      }
    }
  }

}

void ZkClient::AddMessage(const std::string& msg) {
  boost::unique_lock<boost::mutex> lock(message_deque_mutex_);
  std::deque<std::string>::const_iterator iter = std::find(message_deque_->begin(), message_deque_->end(), msg);
  if (iter == message_deque_->end()) {
    message_deque_->push_back(msg);
  }
  message_deque_cond_.notify_all();
  MCE_DEBUG("ZkClient::AddMessage() msg:" << msg << "\tthread_id:" << boost::this_thread::get_id());
}

void ZkClient::AddNamespaceListener(NamespaceListenerPtr listener) {
  if (!listener) {
    return;
  }
  boost::unique_lock<boost::shared_mutex> write_lock(namespace_listener_mutex_);
  std::string ns = listener->ns();
  std::map<std::string, std::list<NamespaceListenerPtr> >::iterator iter = listeners_.find(ns);
  if (iter != listeners_.end()) {
    (iter->second).push_back(listener);
  }
  std::list<NamespaceListenerPtr> list;
  list.push_back(listener);
  listeners_.insert(std::make_pair(ns, list));
}

bool ZkClient::Update(const std::string& znode_path) {
  MCE_INFO("ZkClient::Update znode_path:" << znode_path);
  if (znode_path == kUpdateAllNamespace) {
    return UpdateAllNamespaces();
  }
  return UpdateNamespace(znode_path);
}

bool ZkClient::GetNodes(const std::string& ns, std::list<std::string>* list) {
  if (!list) {
    return false;
  }
  std::string znode_path = GetNamespaceZnodePath(ns);
  return GetChildrenNamesFromZk(znode_path, list);
}

bool ZkClient::UpdateAllNamespaces() {
  bool flag = true;
  boost::shared_lock<boost::shared_mutex> read_lock(namespace_listener_mutex_);
  std::map<std::string, std::list<NamespaceListenerPtr> >::const_iterator iter = listeners_.begin();
  for (; iter != listeners_.end(); ++iter) {
    std::string znode_path = GetNamespaceZnodePath(iter->first);
    bool ret = Update(znode_path);
    if (!ret) {
      flag = false;
    }
  }
  return flag;
}

bool ZkClient::UpdateNamespace(const std::string& znode_path) {
  {
    std::string new_version;
    GetNamespaceVersion(znode_path, &new_version);
    boost::mutex::scoped_lock version_lock(znode_version_mutex_);
    std::map<std::string, std::string>::const_iterator iter = znode_version_map_.find(znode_path);
    if (iter == znode_version_map_.end()) {
      znode_version_map_.insert(std::make_pair(znode_path, new_version));
    }
    else {
      const std::string& old_version = iter->second;
      if (new_version == old_version) { // version没变，直接返回成功
        return true;
      }
      znode_version_map_.insert(std::make_pair(znode_path, new_version));
    }
  }
  
  boost::shared_lock<boost::shared_mutex> read_lock(namespace_listener_mutex_);
  bool ret = true;
  size_t start_pos = znode_path.find_last_of("/");
  if (start_pos == std::string::npos) {
    MCE_ERROR("ZkClient::UpdateNamespace error znode_path:" << znode_path);
    return false;
  } 
  std::string ns = znode_path.substr(start_pos + 1);

  std::map<std::string, std::list<NamespaceListenerPtr> >::const_iterator c_iter = listeners_.find(ns);
  if (c_iter == listeners_.end()) {
    return ret;
  }
  //取ns下面的结点数据
  std::list<std::string> children_names;
  if (!GetChildrenNamesFromZk(znode_path, &children_names)) {
    MCE_ERROR("ZkClient::UpdateNamespace() call GetChildrenNamesFromZk return false, znode_path:" << znode_path);
    return false;
  }
  if (children_names.empty()) {
    MCE_DEBUG("ZkClient::UpdateNamespace() call GetChildrenNamesFromZk return empty list, znode_path:" << znode_path);
    return true;
  }
  //更新
  BOOST_FOREACH(const NamespaceListenerPtr& listener, c_iter->second) {
    bool f = listener->Update(children_names);
    if (!f) {
      ret = false;
    }
  }
  return ret;
}

bool ZkClient::GetNamespaceVersion(const std::string& znode_path, std::string* new_version) {
  return GetDataFromZk(znode_path, new_version);
}

bool ZkClient::GetDataFromZk(const std::string& znode_path, std::string* data) {
  if (!data) {
    return false;
  }
  boost::unique_lock<boost::mutex> lock(zk_mutex_); 
  static const int kBufferLen = 4096;
  char buffer[kBufferLen];
  memset(buffer, 0, kBufferLen);
  int watch = 1;
  int buflen = kBufferLen;
  int rc = zoo_get(zk_, znode_path.c_str(), watch, buffer, &buflen, NULL);
  if (rc != ZOK) {
    return false;
  }
  std::string version(buffer, buflen);
  *data = version;
  return true;
}
  
bool ZkClient::GetChildrenNamesFromZk(const std::string& znode_path, std::list<std::string>* list) {
  if (!list) {
    return false;
  }
  boost::unique_lock<boost::mutex> lock(zk_mutex_); 
  int watch = 1;
  int rc = zoo_exists(zk_, znode_path.c_str(), watch, NULL);
  if (rc != ZOK) {
    MCE_ERROR("znode_path  doesn't exist! znode_path:" << znode_path);
    return false;
  }

  struct String_vector str_vec;
  rc = zoo_get_children(zk_, znode_path.c_str(), 0, &str_vec);
  if (rc != ZOK) {
    return false;
  } 
  for (int i = 0; i < str_vec.count; ++i) {
    std::string child_name = str_vec.data[i];
    list->push_back(child_name);
  }
  return true;
}

std::string ZkClient::GetNamespaceZnodePath(const std::string& ns) {
  std::ostringstream otem;
  otem << ZkClient::kDefaultPrefix << "/" << ns;
  return otem.str();
}

ZkClientPtr ZkClient::GetInstance(const std::string& zk_address) {
  {
    boost::mutex::scoped_lock lock(zk_client_map_mutex_);
    std::map<std::string, ZkClientPtr>::const_iterator iter = ZkClient::address_zk_client_map_.find(zk_address);
    if (iter != address_zk_client_map_.end()) {
      return iter->second;
    }
  }
  ZkClient* zk_client = new ZkClient(zk_address);
  if (zk_client->Initialize()) {
    boost::mutex::scoped_lock lock(zk_client_map_mutex_);
    std::map<std::string, ZkClientPtr>::const_iterator iter = ZkClient::address_zk_client_map_.find(zk_address);
    if (iter != address_zk_client_map_.end()) {
      return iter->second;
    }
  }
  return ZkClientPtr();
}

void ZkClient::NotifyInitialize() {
  zk_cond_.notify_all();
}

bool ZkClient::Initialize() {
  boost::unique_lock<boost::mutex> lock(zk_mutex_);
  zk_ = zookeeper_init(zk_address_.c_str(), &ZkClient::ZkWatcher, 10000, 0, 0, 0);
  if (zk_ == NULL) {
    MCE_INFO("ZkClient::Initialize failed! zk_address:" << zk_address_);
    return false;
  }
  {
    boost::mutex::scoped_lock lock(zk_client_map_mutex_);
    ZkClientPtr client(this);
    address_zk_client_map_.insert(std::make_pair(zk_address_, client));
    zhandle_zk_client_map_.insert(std::make_pair(zk_, client));
  }
  zk_cond_.wait(lock);
  MCE_DEBUG("ZkClient::Initialize wakes up");
  return true;
}

bool ZkClient::Reinitialize() {
  boost::unique_lock<boost::mutex> lock(zk_mutex_); 
  //delete zhandle_zk_client_map_ pair, update address_zk_client_map_
  zhandle_t* old_zk = zk_;
  zhandle_zk_client_map_.erase(old_zk);
  int rc = zookeeper_close(zk_);
  zk_ = NULL;
  zk_ = zookeeper_init(zk_address_.c_str(), &ZkClient::ZkWatcher, 10000, 0, 0, 0);
  if (zk_ == NULL) {
    MCE_ERROR("ZkClient::Reinitialize failed! zk_address:" << zk_address_);
    return false;
  }
  {
    boost::mutex::scoped_lock lock(zk_client_map_mutex_);
    std::map<std::string, ZkClientPtr>::const_iterator iter = address_zk_client_map_.find(zk_address_);
    if (iter != address_zk_client_map_.end()) {
      zhandle_zk_client_map_.insert(std::make_pair(zk_, iter->second));
    }
  }
  zk_cond_.wait(lock);
  set_reinit_flag(true);
  MCE_DEBUG("ZkClient::Reinitialize wakes up");
  return true;
}

void ZkClient::ZkWatcher(zhandle_t *zzh, int type, int state, const char *path, void *watcher_ctx) {
  MCE_INFO("ZkClient::ZkWatcher type:" << WatcherEvent2String(type) << "\tstate:" << State2String(state)
            << "\tpath:" << path
            << "\tthread_id:" << boost::this_thread::get_id() << "\tzzh:" << zzh);
  if (type != ZOO_SESSION_EVENT) {
    std::string znode_path(path);
    boost::unique_lock<boost::mutex> lock(ZkClient::zk_client_map_mutex_);
    std::map<zhandle_t*, ZkClientPtr>::const_iterator iter = ZkClient::zhandle_zk_client_map_.find(zzh);
    if (iter != ZkClient::zhandle_zk_client_map_.end()) {
      iter->second->AddMessage(znode_path);
    }
  }
  else {
    if (state == ZOO_CONNECTING_STATE) {
      boost::unique_lock<boost::mutex> lock(ZkClient::zk_client_map_mutex_);
      std::map<zhandle_t*, ZkClientPtr>::const_iterator iter = ZkClient::zhandle_zk_client_map_.find(zzh);
      if (iter != ZkClient::zhandle_zk_client_map_.end()) {
        iter->second->set_reinit_flag(true);
      }
    }
    if (state == ZOO_CONNECTED_STATE) {
      boost::unique_lock<boost::mutex> lock(ZkClient::zk_client_map_mutex_);
      std::map<zhandle_t*, ZkClientPtr>::const_iterator iter = ZkClient::zhandle_zk_client_map_.find(zzh);
      if (iter != ZkClient::zhandle_zk_client_map_.end()) {
        iter->second->NotifyInitialize();
        if (iter->second->reinit_flag()) {
          iter->second->AddMessage(kUpdateAllNamespace);
        }
      }
    }
    if (state == ZOO_EXPIRED_SESSION_STATE) {
      ZkClientPtr client;
      {
        boost::mutex::scoped_lock lock(ZkClient::zk_client_map_mutex_);
        std::map<zhandle_t*, ZkClientPtr>::const_iterator iter = ZkClient::zhandle_zk_client_map_.find(zzh);
        if (iter != ZkClient::zhandle_zk_client_map_.end()) {
          client = iter->second;
        }
      }
      if (client) {
        client->Reinitialize();
        client->AddMessage(kUpdateAllNamespace);
      }
    }
  }
}

}}} // namespace com::renren::messageconfig


