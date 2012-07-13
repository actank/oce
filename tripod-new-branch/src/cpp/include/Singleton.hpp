#ifndef _TRIPOD_SINGLETON_HPP__
#define _TRIPOD_SINGLETON_HPP__

namespace com {
namespace renren {
namespace tripod {

template<class T>
    class Singleton {
        public:
            static T* getInstance() {
                if( instance__ == NULL ) {
                    boost::upgrade_lock<boost::shared_mutex> lock(lock__);
                    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
                    if( instance__ == NULL ) {
                        instance__ = new T;
                    }
                }
                return instance__;
            }

        protected:
            Singleton(){}
            ~Singleton() {
                if(instance__) {
                    T* t = instance__;
                    instance__ = NULL;
                    free(t);
                }
            }

        private:
            static T* instance__;

            static boost::shared_mutex lock__;
    };

template<class T> T* Singleton<T>::instance__ = NULL;
template<class T> boost::shared_mutex Singleton<T>::lock__;

}
}
}
#endif
