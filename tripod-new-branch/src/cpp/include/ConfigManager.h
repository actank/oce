#ifndef _TRIPOD_CONFIGMANAGER_H__
#define _TRIPOD_CONFIGMANAGER_H__

#include "Common.h"

namespace com {
namespace renren {
namespace tripod {

class BufferContainer {
    public:
        BufferContainer();

    public:
        char buffer[1048576];
        int buflen;
};
typedef boost::shared_ptr<BufferContainer> BufferContainerPtr;

class ConfigManager {

    friend class ConfigManagerFactory;

    private:
    ConfigManager();

    public:
    bool initialize(const std::string& zkAddress);

    bool updateConfig( zhandle_t* zk );

    void registerClient(const ConfigClientPtr& cc);

    ConfigDataPtr getConfigData();

    private:
    boost::shared_mutex lock_;
    ConfigDataPtr configData_;
    ZkManagerPtr zm_;

    std::vector<ConfigClientPtr> clients_;
    boost::shared_mutex clientsLock_;

    private:
    bool init(ConfigDataPtr& config, zhandle_t* zk, bool& updateFlag);

    bool updateManagerConfig(const ConfigDataPtr& config, zhandle_t* zk, bool checkVersion, bool& updateFlag);
    bool updateProducerConfig(const ConfigDataPtr& config, zhandle_t* zk, bool checkVersion, bool& updateFlag);
    bool updateCacheConfig(const ConfigDataPtr& config, zhandle_t* zk, bool checkVersion, bool& updateFlag);

    bool getChildrenDatas(const ConfigDataPtr& config, zhandle_t* zk, const std::ostringstream& path, bool needVersion = false);
};

}
}
}

#endif
