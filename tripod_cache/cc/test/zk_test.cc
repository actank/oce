#include "gtest/gtest.h"

#include <boost/foreach.hpp>

#include "LogWrapper.h"
#include "zk.h"

namespace com {
namespace renren {
namespace tripod {

class ObI : public Observer {
 public:
  ObI(int index):index_(index)  {}
  void Update(const std::string& key, const std::string& value) {
    MCE_INFO("ObI::Update() index:" << index_ << "\tkey:" << key << "\tvalue:" << value);
  }
  virtual ~ObI() {
    MCE_INFO("ObI::~ObI() index:" << index_);
  }
 private:
  int index_;
};

TEST(ZooManager, test) {
  MCE_DEFAULT_INIT("WARN");

  ObserverPtr ob1(new ObI(1));
  ObserverPtr ob2(new ObI(2));
  ObserverPtr ob3(new ObI(3));

  ZooManager::GetInstance().Attach(ob1);
  ZooManager::GetInstance().Attach(ob2);
  ZooManager::GetInstance().Attach(ob3);

  for (int i = 0; i < 5; ++i) {
    std::string v;
    bool flag = ZooManager::GetInstance().Get("/test", &v);
    if (flag) {
      MCE_INFO("key:/test value:" << v);
    }
    else {
      MCE_INFO("key:/test get error");
    }
    sleep(1);
    //bool flag2 = ZooManager::GetInstance().("/test2", &v);
  }

}


TEST(ZooManager, GetChildrenNames) {
  std::vector<std::string> names;
  std::string path = "/Tripod";
  bool flag = ZooManager::GetInstance().GetChildrenNames(path, &names);
  if (flag) {
    MCE_INFO("GetChildNames return false");
  }
  MCE_INFO("path:" << path);
  BOOST_FOREACH(std::string& s, names) {
    MCE_INFO("\tchild name:" << s);
  } 
}

TEST(ZooManager, GetChildrenNameAndValue) {
  std::map<std::string, std::string> name_value_map;
  std::string path = "/Tripod/Cache";
  bool flag = ZooManager::GetInstance().GetChildrenNameAndValue(path, &name_value_map);
  if (flag) {
    MCE_INFO("GetChildNames return false");
  }
  MCE_INFO("path:" << path);
  std::map<std::string, std::string>::const_iterator iter = name_value_map.begin();
  for (; iter != name_value_map.end(); ++iter) {
    MCE_INFO("\tchild_name:" << iter->first << "\tchild_value:" << iter->second);
  }
}

}}} 

