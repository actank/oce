#include <Common.h>
#include <MemcachedClientPool.h>

namespace com {
namespace renren {
namespace tripod {

MemcachedClientPool::MemcachedClientPool(const std::string& address, int port, size_t coreSize, size_t maxSize) :
    address_(address), port_(port), coreSize_(coreSize), maxSize_(maxSize), used_(0) {
        MCE_INFO("MemcachedClientPool::MemcachedClientPool address " << address 
                << " port " << port << " coreSize " << coreSize << " maxSize_" << maxSize_);
    }

MemcachedClientPool::~MemcachedClientPool() {
    boost::mutex::scoped_lock lock(unUsedMutex_);
    for (std::deque<memcache::Memcache*>::iterator it = unUsed_.begin(); it
            != unUsed_.end(); ++it) {
        destory(*it);
    }
}

memcache::Memcache* MemcachedClientPool::borrowItem() {
    memcache::Memcache* rt = NULL;
    bool createNew = false;
    {
        boost::mutex::scoped_lock lock(unUsedMutex_);
        if (unUsed_.empty() && (used_ < maxSize_)) {
            createNew = true;
        } else {
            rt = unUsed_.front();
            unUsed_.pop_front();
        }
    }
    if (createNew) {
        rt = create();
    }
    if (rt != NULL) {
        boost::mutex::scoped_lock lock(unUsedMutex_);
        used_++;
    }
    return rt;
}

void MemcachedClientPool::returnItem(memcache::Memcache* item) {
    boost::mutex::scoped_lock lock(unUsedMutex_);
    unUsed_.push_back(item);
    used_--;
}

memcache::Memcache* MemcachedClientPool::create() {
    memcache::Memcache* ret = new memcache::Memcache;
    ret->addServer(address_, port_);
    ret->setBehavior(MEMCACHED_BEHAVIOR_TCP_KEEPALIVE, true);
    ret->setBehavior(MEMCACHED_BEHAVIOR_SUPPORT_CAS, true);
    ret->setBehavior(MEMCACHED_BEHAVIOR_TCP_NODELAY, true);
    return ret;
}

void MemcachedClientPool::destory(memcache::Memcache* item) {
    if(item) {
        delete item;
    }
}

}
}
}
