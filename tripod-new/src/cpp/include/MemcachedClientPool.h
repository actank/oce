#ifndef _TRIPOD_MEMCACHEDCLIENTPOOL_H__
#define _TRIPOD_MEMCACHEDCLIENTPOOL_H__

#include <libmemcached/memcached.hpp>

#include <deque>
#include <boost/thread/mutex.hpp>

namespace com {
namespace renren {
namespace tripod {

class MemcachedClientPool {
    public:
        MemcachedClientPool(const std::string& address, int port, size_t coreSize = 10, size_t maxSize = 100);

        ~MemcachedClientPool();

        memcache::Memcache* borrowItem();

        void returnItem(memcache::Memcache* item);


        memcache::Memcache* create();

        void destory(memcache::Memcache* item);

    private:
        std::string address_;
        int port_;

        size_t coreSize_;
        size_t maxSize_;

        boost::mutex unUsedMutex_;
        std::deque<memcache::Memcache*> unUsed_;
        size_t used_;
};

}
}
}

#endif
