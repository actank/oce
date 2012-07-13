#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <Common.h>
#include <ConfigData.h>
#include <CacheManagerConfigClient.h>

namespace com {
namespace renren {
namespace tripod {

CacheManagerConfigClient::CacheManagerConfigClient(const std::string& zkAddress) : ConfigClient(zkAddress, TRIPOD_MANAGER) {
    MCE_INFO("CacheManagerConfigClient::CacheManagerConfigClient zkAddress " << zkAddress);
    ic_ = Ice::initialize();
}

bool CacheManagerConfigClient::update(const ConfigDataPtr& newConfig) {
    MCE_INFO("CacheManagerConfigClient::update");
    std::map<std::string, ConfigDataPtr>::const_iterator findIt = newConfig->childrenDatas_.find(TRIPOD_MANAGER);
    if(findIt == newConfig->childrenDatas_.end()) {
        MCE_WARN("CacheManagerConfigClient::update no cachemanager in config!");
        return false;
    }
    std::map<std::string, CacheManagerPrx> prxs;
    for(std::map<std::string, ConfigDataPtr>::const_iterator it = findIt->second->childrenDatas_.begin();
            it != findIt->second->childrenDatas_.end(); ++it) {
            
        std::vector<std::string> parts;
        boost::algorithm::split(parts, it->second->name_, boost::algorithm::is_any_of(":"));
        if(parts.size() != 2) {
            std::ostringstream oss;
            oss << "CacheManagerConfigClient::update Exception the config string" << it->second->name_
                << " not match the format: <host>:<port>";
            MCE_WARN(oss.str());
            return false;
        }
        std::string address = parts[0];
        std::string port = parts[1];
        std::ostringstream os;
        os<<"M:tcp -h "<<address<<" -p "<<port<<" -t 1000";
        prxs.insert(make_pair(it->second->name_, CacheManagerPrx::uncheckedCast(ic_->stringToProxy(os.str()))));
    }

    hashHolder_.renew(prxs);

    return true;
}

std::map<CacheManagerPrx, KeySeq> CacheManagerConfigClient::getPrxToKeysMap(const KeySeq& keys) {
    return hashHolder_.get(keys);
}

std::map<std::string, CacheManagerPrx> CacheManagerConfigClient::getOriginalPrxMap() {
    return hashHolder_.getOriginalPrxMap();
}

}
}
}
