#ifndef XCE_ARCH_CONFIG_H__
#define XCE_ARCH_CONFIG_H__

#include <string>
#include <vector>
#include <list>

#include <boost/algorithm/string/classification.hpp>  // for is_any_of
#include <boost/algorithm/string/split.hpp>           // for split
#include <boost/lexical_cast.hpp>

#if 0
#include "storm/xcestorm.h"
#include "storm/svcdescriptor.h"
#include "storm/storm_adapter.h"
#endif
#include "arch/config_adapter.h"

namespace xce {

#if 0
// TODO: remove file property.inl
#include "arch/property.inl"

template<typename ObjectT>
class StormObserver : public xce::storm::Observer {
public:
  StormObserver(ObjectT& t) : t_(t) {}

  bool update(const MyUtil::Str2StrMap& context, const Ice::Current&) {
    // if changed
    Ice::PropertiesPtr p = detail::MakeProperties(context);
    t_.ApplyConfig(p); // TODO: add key
    return true;
  }

private:
  ObjectT& t_;
};

template<typename ObjectT>
class ServiceObserver : public MyUtil::SvcObserver {
public:
  ServiceObserver(ObjectT& t) : t_(t) {}

  bool update(const MyUtil::SvcInstancePtr& ptr, const Ice::Current&) {
    // if changed
    Ice::PropertiesPtr p = detail::MakeProperties(ptr->properties);
    t_.ApplyConfig(p);
    return true;
  }

private:
  ObjectT& t_;
};
#endif

template<typename ObjectT>
class ListenerOfArch : public ConfigListener {
public:
  virtual bool Update(const std::string& key, const std::string& val
    , const Ice::Current& = Ice::Current()) {
      t_.ApplyConfig(key, val);
      return true;
  }

  ListenerOfArch(ObjectT& t) : t_(t) {}
private:
  ObjectT& t_;
};

inline std::string Config(const std::string& key, const std::string& default_value) {
  try {
    ConfigPrx config = CachedConfigPrx();
    return config->Query(key);
  } catch(...) {}

  return default_value;
}

inline int Config(const std::string& key, int default_value) {
  std::string ret = Config(key, "");
  if (ret.empty())
    return default_value;

  int int_ret = default_value;
  try {
    int_ret = boost::lexical_cast<int>(ret);
  } catch(...) {}

  return int_ret;
}

inline void TheConfigSplit(const std::string& text, std::list<std::string>* result
            , const char* delimit = "|") {
  using namespace std;
  typedef vector<string> split_vector_type;

  split_vector_type SplitVec;
  boost::split(SplitVec, text, boost::is_any_of(delimit));

  if (result)
    std::copy(SplitVec.begin(), SplitVec.end(), std::back_inserter(*result));
}

inline bool Config(const std::string& key, std::list<std::string>* result
                   , const char* delimit = "|") {
  std::string ret = Config(key, "");
  if (ret.empty())
    return false;

  TheConfigSplit(ret, result, delimit);
  return true;
}

}
#endif // XCE_ARCH_CONFIG_H__
