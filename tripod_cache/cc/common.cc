#include "common.h"

#include <boost/foreach.hpp>
#include <boost/thread/locks.hpp>

namespace com {
namespace renren {
namespace tripod {

//
Subject::Subject() {

}

//
Subject::~Subject() {
  boost::lock_guard<boost::mutex> lock(mutex_);
  observer_list_.clear();
}


void Subject::Attach(ObserverPtr ob) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  if (ob) {
    observer_list_.push_back(ob);
  }
}
void Subject::Detach(ObserverPtr ob) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  if (ob) {
    observer_list_.remove(ob);
  }
}
void Subject::NotifyObservers(const std::string& key, const std::string& value) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  BOOST_FOREACH(ObserverPtr& ob, observer_list_) {
    ob->Update(key, value);
  }
}

//
ConfigDataPtr ConfigData::GetData() const {
  ConfigDataPtr config_data(new ConfigData);
  config_data->set_name(name_);
  config_data->set_value(name_);
  config_data->set_version(version_);
  
  ConfigDataMap::const_iterator iter = children_data_.begin();
  for (; iter != children_data_.end(); ++iter) {
    config_data->AddChild(iter->first, iter->second);
  }
}

std::string ConfigData::Str() const {
  std::ostringstream otem;
  otem << "ConfigData [";
  otem << "name=" << name_ << ";";
  otem << "value=" << value_ << ";";
  otem << "version=" << version_ << ";";
  otem << "children size=" << ChildenSize() << ";";
  ConfigDataMap::const_iterator iter = children_data_.begin();
  for (; iter != children_data_.end(); ++iter) {
    otem << "child data [ " << iter->second->Str();   
  }
  otem << "]";
  return otem.str();
}


}}} // end com::renren::tripod

