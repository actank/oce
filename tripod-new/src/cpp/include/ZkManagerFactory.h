#ifndef _TRIPOD_ZKMANAGERFACTORY_H__
#define _TRIPOD_ZKMANAGERFACTORY_H__

#include "Common.h"
#include <Singleton.hpp>
#include "ZkManager.h"

namespace com {
namespace renren {
namespace tripod {

class ZkManagerFactory : public Singleton<ZkManagerFactory> {
    public:
        ZkManagerPtr getZkManager(std::string zkAddress);

    private:
        std::map<std::string, ZkManagerPtr> zkManagerMap_;

        boost::shared_mutex zkManagerMaplock_;

};

}
}
}
#endif
