#ifndef _TRIPOD_ZK_H_
#define _TRIPOD_ZK_H_

#include <vector>
#include <map>
#include "singleton.h"
#include "common.h"

#include <zookeeper/zookeeper.h>

namespace com {
namespace renren {
namespace tripod {

class ZooManager : public Subject, public Singleton<ZooManager> {
 public:
  virtual ~ZooManager();
  bool Get(const std::string& key, std::string*);
  bool GetChildrenNames(const std::string& key, std::vector<std::string>*);
  bool GetChildrenNameAndValue(const std::string& key, std::map<std::string, std::string>*);
  void Delete(const std::string& key);
  void Update(const std::string&);

  void ReInitialize();

 private:
  ZooManager();
  void Initialize();
  void ResetBuffer();
  void Destroy();

 private:
  const static int kBuflen = 10240;
  friend class Singleton<ZooManager>;
 private:
  std::map<std::string, std::string> config_map_;
  boost::mutex config_mutex_;
  zhandle_t* zk_;
  char buffer_[kBuflen];
};

}}} // end com::renren::tripod
#endif

