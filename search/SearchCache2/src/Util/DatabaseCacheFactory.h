#ifndef SEARCHCACHE_SRC_ABSTRACTDBCACHEFACTORY_H_
#define SEARCHCACHE_SRC_ABSTRACTDBCACHEFACTORY_H_

#include "IDbCache.h"
#include "TokyoTyrantCache.h"

namespace xce {
namespace searchcache {
    class DatabaseCacheFactory : public MyUtil::Singleton<DatabaseCacheFactory> {
        public:
            boost::shared_ptr<IDbCache> getDbCache(std::string dsn) {
                IDbCache* p = new TokyoTyrantCache(dsn);
                boost::shared_ptr<IDbCache> ttCache = boost::shared_ptr<IDbCache>(p); 
                return ttCache;
            }
    };
};
};
#endif

