#ifndef _MESSAGECONFIG_SINGLETON_HPP__
#define _MESSAGECONFIG_SINGLETON_HPP__

#include <boost/thread.hpp>

namespace com {
namespace renren {
namespace messageconfig {

template<class T>
class Singleton {
 public:
  static T& GetInstance() {
    if( instance_ == NULL ) {
      boost::upgrade_lock<boost::shared_mutex> lock(lock_);
      boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
      if( instance_ == NULL ) {
        instance_ = new T;
      }
    }
    return *instance_;
  }

 protected:
  Singleton() { }
  ~Singleton() {
   if(instance_) {
     T* t = instance_;
     instance_ = NULL;
     delete t;
   }
  }
 private:
  static T* instance_;
  static boost::shared_mutex lock_;
};

template<class T> T* Singleton<T>::instance_ = NULL;
template<class T> boost::shared_mutex Singleton<T>::lock_;

}}} // end com::renren::messageconfig

#endif
