#ifndef XCE_DB_DESCRIPTOR_IMPL_H__
#define XCE_DB_DESCRIPTOR_IMPL_H__

#include <map>
#include <set>
#include <string>
#include <boost/thread/mutex.hpp>

#include "base/hashmap.h"
#include <Ice/Ice.h>
#include "dbdesc/dbdescriptor.h"

// java 需要的接口
// query
// subscribe / unsubscribe

// web 需要的接口
// subscribe(*)
// update(key, action, DbInstance)

namespace com { namespace xiaonei { namespace xce {

class DbDescriptorI : public DbDescriptor {
public:
  DbDescriptorI();

  // 1 
  virtual void reload(const Ice::Current& = Ice::Current());

  virtual DbInstanceSeq getDbInstances(const Ice::Current& = Ice::Current());
  virtual DbInstancePtr getDbInstance(const std::string&, const Ice::Current& = Ice::Current());
  virtual void subscribe(const std::string&, const DbObserverPrx&, const Ice::Current& = Ice::Current());
  virtual void unsubscribe(const std::string&, const DbObserverPrx&, const Ice::Current& = Ice::Current());

  virtual DbObserverMap query(const std::string&, const Ice::Current& = Ice::Current());

  // 2
  virtual bool update(const ::std::string&, const ::std::string&, const DbInstancePtr&, const ::Ice::Current& = Ice::Current());

  void ApplyConfig(const Ice::PropertiesPtr&);

private:
  void Init();
  void Notify(std::string const& name, DbInstancePtr db);  
  bool IsEqualInstance(const DbInstancePtr& instance);

private:
  // 实现set<Prx> 有两种方式
  // 1 less_by_id
  // 2 特化 less，必须全局
  template<typename T>
  struct less_by_id : public std::binary_function<T, T, bool> {
    bool operator()(const T& left, const T & right) const {
      return left->ice_getIdentity() < right->ice_getIdentity();
    }
  };

  // 一个key只能被一个client订阅一次
  typedef std::set<DbObserverPrx, less_by_id<DbObserverPrx> > observer_set;
  typedef std::hash_map<std::string, observer_set> observer_by_name;

  observer_by_name observers_;

  DbInstanceMap dbs_;
  boost::mutex guard_; // 只使用1把大锁
  
  DbServerPtr init_db_;
};

}}}
#endif // XCE_DB_DESCRIPTOR_IMPL_H__
