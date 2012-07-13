#ifndef _TRIPOD_COMMON_H_
#define _TRIPOD_COMMON_H_

#include <string>
#include <list>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>


namespace com {
namespace renren {
namespace tripod {

class Observer {
 public:
  virtual void Update(const std::string&, const std::string&) = 0;
  virtual ~Observer() {}
};

typedef boost::shared_ptr<Observer> ObserverPtr;

class Subject {
 public:
  Subject();
  virtual ~Subject();
  void Attach(ObserverPtr ob);
  void Detach(ObserverPtr ob);
  void NotifyObservers(const std::string&, const std::string&);
 private:
  boost::mutex mutex_; 
  std::list<ObserverPtr> observer_list_;
};

//
struct ConfigData;
typedef boost::shared_ptr<ConfigData> ConfigDataPtr;

class ConfigData {
 public:
  ConfigData(): name_(""), value_(""), version_("") {}
  std::string name() const {
    return name_;
  } 
  void set_name(std::string name) {
    name_ = name;
  }
  std::string value() const {
    return value_;
  } 
  void set_value(std::string value) {
    value_ = value;
  }
  std::string version() const {
    return version_;
  } 
  void set_version(std::string version) {
    version_ = version;
  }

  void AddChild(const std::string& name, const ConfigDataPtr& data) {
    children_data_.insert(std::make_pair(name, data));
  }

  int ChildenSize() const {
    return children_data_.size();
  }

  ConfigDataPtr GetData() const;
  std::string Str() const;

 private:
  std::string name_;
  std::string value_;
  std::string version_;
  typedef std::map<std::string, ConfigDataPtr> ConfigDataMap;
  ConfigDataMap children_data_;
};




}}} // end com::renren::tripod


#endif

