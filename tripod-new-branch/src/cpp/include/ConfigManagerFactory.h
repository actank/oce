#ifndef _TRIPOD_CONFIGMANAGERFACTORY_H__
#define _TRIPOD_CONFIGMANAGERFACTORY_H__

#include "Common.h"
#include <Singleton.hpp>
#include "ConfigManager.h"

namespace com {
namespace renren {
namespace tripod {

class ConfigManagerFactory : public Singleton<ConfigManagerFactory> {
    public:
        ConfigManagerPtr getConfigManager(std::string zkAddress);
    
    private:
        std::map<std::string, ConfigManagerPtr> configManagerMap_;

        boost::shared_mutex configManagerMaplock_;
};

}
}
}

#endif
