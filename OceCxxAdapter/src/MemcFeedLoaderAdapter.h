#ifndef _MC_PROXY_LOADER_ADAPTER_H_
#define _MC_PROXY_LOADER_ADAPTER_H_

#include <boost/lexical_cast.hpp>

#include "AdapterI.h"
#include "MemcProxy.h"
#include "Singleton.h"


namespace xce {
namespace feed {

using namespace MyUtil;

class MemcFeedLoaderAdapter : public AdapterI,
                   public AdapterISingleton<MyUtil::McProxyChannel, MemcFeedLoaderAdapter>
{
protected:
  virtual string name() {
    return "MemcFeedLoader";
  }
  virtual string endpoints() {
    return "@MemcFeedLoader";
  }
  virtual size_t cluster() {
    return 10;
  }

public:
  MemcFeedLoaderAdapter() {
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
    _managers.resize(cluster() > 0 ? cluster() : 1);
  }

  void LoadFeedByKey(const string & key) {
    getManagerOneway(GetIdByKey(key))->LoadFeedByKey(key);
  }

  void LoadFeedSeqByKey(const MyUtil::StrSeq & keys) {
    if(!keys.empty()) {
      getManagerOneway(GetIdByKey(keys[0]))->LoadFeedSeqByKey(keys);
    }
  }

  Str2StrMap GetFeedSeqByKey(const MyUtil::StrSeq & keys) {
    if(!keys.empty()) {
      return getManager(GetIdByKey(keys[0]))->GetFeedSeqByKey(keys);
    }
    return Str2StrMap();
  }

private:
  Ice::Long GetIdByKey(const string & key) {
    Ice::Long id = 0;
    size_t pos = key.find_first_of('#');
    if (pos != string::npos) {
      try {
        id = boost::lexical_cast<Ice::Long>(key.c_str() + pos + 1);
      } catch (boost::bad_lexical_cast & e) {}
    }

    return id;
  }

  MemcFeedLoaderPrx getManagerOneway(int id){
    return locate<MemcFeedLoaderPrx>(_managersOneway, "M", id, ONE_WAY);
  }

  MemcFeedLoaderPrx getManager(int id){
    return locate<MemcFeedLoaderPrx>(_managers, "M", id, TWO_WAY);
  }

  vector<MemcFeedLoaderPrx> _managersOneway;
  vector<MemcFeedLoaderPrx> _managers;
};

}
}

#endif // _MC_PROXY_LOADER_ADAPTER_H_
