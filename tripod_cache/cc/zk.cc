#include "zk.h"

#include "LogWrapper.h"

namespace com {
namespace renren {
namespace tripod {

//const static std::string kDefaultZooKeeperAddress = "UserZooKeeper1:2181";
//const static std::string kDefaultZooKeeperAddress = "10.3.16.96:2181";
const static std::string kDefaultZooKeeperAddress = "10.22.206.32:2181";

const static int kRecvTimeout = 5000;

const std::string StateToString(int state) {
  if (state == ZOO_EXPIRED_SESSION_STATE) {
    return "ZOO_EXPIRED_SESSION_STATE";
  }
  if (state == ZOO_AUTH_FAILED_STATE) {
    return "ZOO_AUTH_FAILED_STATE";
  } 
  if (state == ZOO_CONNECTING_STATE) {
    return "ZOO_CONNECTING_STATE";
  }
  if (state == ZOO_ASSOCIATING_STATE) {
    return "ZOO_ASSOCIATING_STATE";
  }
  if (state == ZOO_CONNECTED_STATE) {
    return "ZOO_CONNECTED_STATE";
  }
  return "INVALID_STATE";
}

const std::string WatcherEventToString(int event) {
  if (event == ZOO_CREATED_EVENT) {
    return "ZOO_CREATED_EVENT"; }
  if (event == ZOO_DELETED_EVENT) {
    return "ZOO_DELETED_EVENT";
  }
  if (event == ZOO_CHANGED_EVENT) {
    return "ZOO_CHANGED_EVENT";
  }
  if (event == ZOO_CHILD_EVENT) {
    return "ZOO_CHILD_EVENT";
  }
  if (event == ZOO_SESSION_EVENT) {
    return "ZOO_SESSION_EVENT";
  }
  if (event == ZOO_NOTWATCHING_EVENT) {
    return "ZOO_NOTWATCHING_EVENT";
  }
  return "INVALID_EVENT";
}


static void Watcher(zhandle_t *zh, 
                    int type, 
                    int state, 
                    const char *path,
                    void *watcherCtx) {
  // state for conn, type for event
  MCE_INFO("ZkStat:" << StateToString(state) 
           << "\tZkEvent:" << WatcherEventToString(type)
           << "\tpath:" << path);
  if (state == ZOO_EXPIRED_SESSION_STATE) {
    ZooManager::GetInstance().ReInitialize();
  }
  if (state == ZOO_AUTH_FAILED_STATE) {
  
  }
  if (state == ZOO_CONNECTING_STATE) {
    ZooManager::GetInstance().ReInitialize();
  }
  if (state == ZOO_ASSOCIATING_STATE) {

  }
  if (state == ZOO_CONNECTED_STATE) {
    if (type == ZOO_CREATED_EVENT) {
      std::ostringstream otem;
      otem << path;
      ZooManager::GetInstance().Update(otem.str());
    }
    if (type == ZOO_DELETED_EVENT) {
      std::ostringstream otem;
      otem << path;
      ZooManager::GetInstance().Update(otem.str());
    }
    if (type == ZOO_CHANGED_EVENT) {
      std::ostringstream otem;
      otem << path;
      ZooManager::GetInstance().Update(otem.str());
    }
    if (type == ZOO_CHILD_EVENT) {
  
    }
    if (type == ZOO_SESSION_EVENT) {
  
    }
    if (type == ZOO_NOTWATCHING_EVENT) {
  
    }
  }
}

ZooManager::ZooManager() : zk_(0) {
  Initialize();
}

ZooManager::~ZooManager() {
  Destroy();
}

void ZooManager::Initialize() {
  zk_ = zookeeper_init(kDefaultZooKeeperAddress.c_str(), Watcher, kRecvTimeout, 0, 0, 0);
  if (zk_) {
    MCE_INFO("ZooManager succeeded in initializing zookeeper connection");
  }
}

void ZooManager::ResetBuffer() {
  memset(buffer_, 0, kBuflen);
}

void ZooManager::Destroy() {
  if (!zk_)
    return;
  int rc = zookeeper_close(zk_);
  zk_ = 0;
  switch (rc) {
    case ZOK:
      MCE_INFO("ZooManager succeeded in finalizing zookeeper connection");
      break;
    case ZBADARGUMENTS:break;
    case ZMARSHALLINGERROR: break;
    case ZOPERATIONTIMEOUT:break;
    case ZCONNECTIONLOSS:break;
    case ZSYSTEMERROR: break;
  }
  config_map_.clear();
}


bool ZooManager::Get(const std::string& key, std::string* value) {
  boost::lock_guard<boost::mutex> lock(config_mutex_);
  std::map<std::string, std::string>::const_iterator it = config_map_.find(key);
  if (it != config_map_.end()) {
    *value = it->second;
    return true;
  }
  if (!zk_) {
    return false;
  }
  ResetBuffer();
  int buflen = kBuflen;
  int rc = zoo_get(zk_, key.c_str(), 1, buffer_, &buflen, 0);
  if (rc == ZOK) {
    std::string v(buffer_, kBuflen);
    *value = v;
    config_map_.insert(std::make_pair(key, v));
    return true;
  }
  return false;
}

void ZooManager::Update(const std::string& key) {
  boost::lock_guard<boost::mutex> lock(config_mutex_);
  ResetBuffer();
  std::map<std::string, std::string>::iterator it = config_map_.find(key);
  if (it != config_map_.end()) {
    config_map_.erase(it);
  }
  int buflen = kBuflen;
  int rc = zoo_get(zk_, key.c_str(), 1, buffer_, &buflen, 0);
  if (rc == ZOK) {
    std::string value(buffer_, kBuflen);
    config_map_.insert(std::make_pair(key, value));
    NotifyObservers(key, value);
  }
}
bool ZooManager::GetChildrenNames(const std::string& key, std::vector<std::string>* name_vec) {
  boost::lock_guard<boost::mutex> lock(config_mutex_);
  struct String_vector node_names;
  int no_watch = 0;
  int rc = zoo_get_children(zk_, key.c_str(), no_watch, &node_names);
  if (rc != ZOK) {
    return false;
  }
  int count = node_names.count;
  if (count == 0) {
    return false;
  }
  for (int i = 0; i < count; ++i) {
    std::string name = node_names.data[i];
    name_vec->push_back(name);
  }
  return true;
}

bool ZooManager::GetChildrenNameAndValue(const std::string& key, std::map<std::string, std::string>* name_value_map) {
  boost::lock_guard<boost::mutex> lock(config_mutex_);
  struct String_vector node_names;
  int no_watch = 0;
  int rc = zoo_get_children(zk_, key.c_str(), no_watch, &node_names);
  if (rc != ZOK) {
    return false;
  }
  int count = node_names.count;
  if (count == 0) {
    return false;
  }
  for (int i = 0; i < count; ++i) {
    std::string child_name = node_names.data[i];
    std::string child_path = key + "/" + child_name;
    ResetBuffer();
    int buflen = kBuflen;
    int watcher = 1;
    int rc2 = zoo_get(zk_, key.c_str(), watcher, buffer_, &buflen, 0);
    if (rc2 == ZOK) {
      std::string child_value(buffer_, kBuflen);
      name_value_map->insert(std::make_pair(child_name, child_value));
      // cause watcher = 1 so insert config_map_
      config_map_.insert(std::make_pair(child_path, child_value));
    }
  }
  return true;
}

void ZooManager::Delete(const std::string& key) {
  boost::lock_guard<boost::mutex> lock(config_mutex_);
  ResetBuffer();
  if (config_map_.find(key) != config_map_.end()) {
    config_map_.erase(key);
  }
}

void ZooManager::ReInitialize() {
  boost::lock_guard<boost::mutex> lock(config_mutex_);
  MCE_INFO("ZooManager::ReInitialize()");
  Destroy();
  Initialize();
}

}}} // end com::renren::tripod

