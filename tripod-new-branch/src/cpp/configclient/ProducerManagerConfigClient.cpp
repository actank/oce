#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <Common.h>
#include <ConfigData.h>
#include <ProducerManagerConfigClient.h>

namespace com {
namespace renren {
namespace tripod {

ProducerManagerConfigClient::ProducerManagerConfigClient(const std::string& zkAddress) : ConfigClient(zkAddress, TRIPOD_PRODUCER) {
    MCE_INFO("ProducerManagerConfigClient::ProducerManagerConfigClient zkAddress " << zkAddress);
    ic_ = Ice::initialize();
}

bool ProducerManagerConfigClient::update(const ConfigDataPtr& newConfig) {
    std::map<std::string, ConfigDataPtr>::const_iterator findIt = newConfig->childrenDatas_.find(TRIPOD_PRODUCER);
    if(findIt == newConfig->childrenDatas_.end()) {
        MCE_WARN("ProducerManagerConfigClient::update no produceremanager in config!");
        return false;
    }

    ConfigDataPtr producerConfig = findIt->second;

    findIt = producerConfig->childrenDatas_.find(TRIPOD_PRODUCER_JAVA);
    if(findIt == producerConfig->childrenDatas_.end()) {
        MCE_WARN("ProducerManagerConfigClient::update no java produceremanager in config!");
        return false;
    }

    std::map<std::string, ProducerManagerPrx> javaPrxs;
    for(std::map<std::string, ConfigDataPtr>::const_iterator it = findIt->second->childrenDatas_.begin();
            it != findIt->second->childrenDatas_.end(); ++it) {
        std::vector<std::string> parts;
        boost::algorithm::split(parts, it->second->name_, boost::algorithm::is_any_of(":"));
        if(parts.size() != 2) {
            std::ostringstream oss;
            oss << "ProducerManagerConfigClient::update Exception the config string" << it->second->name_
                << " not match the format: <host>:<port>";
            MCE_WARN(oss.str());
            return false;
        }
        std::string address = parts[0];
        std::string port = parts[1];
        std::ostringstream os;
        os<<"M:tcp -h "<<address<<" -p "<<port<<" -t 1000";
        javaPrxs.insert(make_pair(it->second->name_, ProducerManagerPrx::uncheckedCast(ic_->stringToProxy(os.str()))));
    }

    javaPrxHashHolder_.renew(javaPrxs);

    findIt = producerConfig->childrenDatas_.find(TRIPOD_PRODUCER_CC);
    if(findIt == producerConfig->childrenDatas_.end()) {
        MCE_WARN("ProducerManagerConfigClient::update no cc produceremanager in config!");
        return false;
    }

    std::map<std::string, ProducerManagerPrx> ccPrxs;
    for(std::map<std::string, ConfigDataPtr>::const_iterator it = findIt->second->childrenDatas_.begin();
            it != findIt->second->childrenDatas_.end(); ++it) {
        std::vector<std::string> parts;
        boost::algorithm::split(parts, it->second->name_, boost::algorithm::is_any_of(":"));
        if(parts.size() != 2) {
            std::ostringstream oss;
            oss << "ProducerManagerConfigClient::update Exception the config string" << it->second->name_
                << " not match the format: <host>:<port>";
            MCE_WARN(oss.str());
            return false;
        }
        std::string address = parts[0];
        std::string port = parts[1];
        std::ostringstream os;
        os<<"M:tcp -h "<<address<<" -p "<<port<<" -t 1000";
        ccPrxs.insert(make_pair(it->second->name_, ProducerManagerPrx::uncheckedCast(ic_->stringToProxy(os.str()))));
    }

    ccPrxHashHolder_.renew(ccPrxs);

    return true;
}

std::map<ProducerManagerPrx, KeySeq> ProducerManagerConfigClient::getJavaPrxToKeysMap(const KeySeq& keys) {

    return javaPrxHashHolder_.get(keys);
}

std::map<ProducerManagerPrx, KeySeq> ProducerManagerConfigClient::getCcPrxToKeysMap(const KeySeq& keys) {

    return ccPrxHashHolder_.get(keys);
}

std::map<std::string, ProducerManagerPrx> ProducerManagerConfigClient::getOriginalPrxMap() {
    std::map<std::string, ProducerManagerPrx> res;
    std::map<std::string, ProducerManagerPrx> ccRes = ccPrxHashHolder_.getOriginalPrxMap();
    std::map<std::string, ProducerManagerPrx> javaRes = javaPrxHashHolder_.getOriginalPrxMap();
    res.insert(ccRes.begin(), ccRes.end());
    res.insert(javaRes.begin(), javaRes.end());
    return res;
}

}
}
}
