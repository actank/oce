#include "base/once.h"
#include "base/logging.h"
#include "socialgraph/socialgraphutil/zkclient.h"

namespace xce {

static const char* kHostName = "10.3.17.99:2181,10.3.17.98:2181,10.3.17.102:2181,10.3.19.60:2181,10.3.19.61:2181";

static ZooConfig* zc_ = 0;
static void InitZooKeeper() {
  zc_ = new ZooConfig();
}

ZooConfig& ZooConfig::GetInstance() {
  static XCE_DECLARE_ONCE(once_guard_);
  XceOnceInit(&once_guard_, &InitZooKeeper);
  return *zc_;
}

static void Watcher(zhandle_t *zh, int type, int state, const char *path,void *watcherCtx) {
	int rc = zoo_exists(zh, path, 1, 0);
	const std::string key(path);
  if (state == ZOO_CONNECTED_STATE) {
		if (type == ZOO_CREATED_EVENT || type == ZOO_CHANGED_EVENT) {
			switch (rc) {
			case ZOK: {
				xce::ZooConfig::GetInstance().Update(key);
			}
				break;
			case ZNONODE: {
					xce::ZooConfig::GetInstance().Delete(key);
			}
				break;
			}
		} else if (type == ZOO_DELETED_EVENT) {
			xce::ZooConfig::GetInstance().Delete(key);
		} else if (type == ZOO_SESSION_EVENT) {
		} else if (type == ZOO_CHILD_EVENT) {
		} else if (type == ZOO_NOTWATCHING_EVENT) {
		}
	}
}

void ZooConfig::Init() {
  zh_ = zookeeper_init(kHostName, &Watcher, 5000, 0, 0, 0);
	if (zh_)
		LOG(INFO) << "zh_ init ok!";
}

void ZooConfig::Destroy() {
	if (zh_) {
		zookeeper_close(zh_);
		LOG(INFO) << "zh_ is closed!";
	}
}

void ZooConfig::Reset() {
	buflen_ = 10240;
	memset(buffer_, 0, buflen_);
}


bool ZooConfig::Get(const std::string& znode, std::string* value) {
  boost::lock_guard<boost::mutex> guard(mutex);
	std::hash_map<std::string, std::string>::const_iterator iter = map_.find(znode);
  if (iter != map_.end()) { 
		*value = iter->second;
		return true;
	}
	else {  // 去zookeeper server 中取
		if (!zh_)
			return false;
		Reset();
		int rc = zoo_get(zh_, znode.c_str(), 1, buffer_, &buflen_, 0);
		if (rc == ZOK) {
			std::string v(buffer_, buflen_);
			*value = v;
			map_.insert(std::make_pair(znode, v));
			return true;
		}
	}
	return false;
}

bool ZooConfig::Set(const std::string& znode,const std::string* value) {
	if (zh_) {
		Reset();
		if (value->empty())
			return false;
		buflen_ = value->length();
		strcpy(buffer_, value->c_str());
		int rc = zoo_set(zh_, znode.c_str(), buffer_, buflen_, -1);
		if (rc == ZOK)
			return true;
	}
	return false;
}

bool ZooConfig::Update(const std::string& key) {
	boost::lock_guard<boost::mutex> guard(mutex);
	Reset();
	int rc = zoo_get(zh_, key.c_str(), 1, buffer_, &buflen_, 0);
	if (rc == ZOK) {
		std::string v(buffer_, buflen_);
		map_[key] = v;
		NotifyObservers(key, v);
		return true;
	}
	
	return false;
}

bool ZooConfig::Delete(const std::string& key) {
	boost::lock_guard<boost::mutex> guard(mutex);
	if (map_.find(key) != map_.end()) {
		map_.erase(key);
		return true;
	}
	return false;
}

} // end namespace xce


