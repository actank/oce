#ifndef _TRIPOD_PRODUCERMANAGERCLIENT_H__
#define _TRIPOD_PRODUCERMANAGERCLIENT_H__

#include <TripodProducerManager.h>

#include <Common.h>
#include <HashHolder.hpp>
#include <ConfigClientFactory.h>
#include <Ice/Ice.h>

namespace com {
namespace renren {
namespace tripod {

class ProducerManagerClient {
    friend class ClientFactory;
    protected:
    ProducerManagerClient(const std::string& zkAddress);

    public:
    DataMap produce(const KeySeq& keys, const std::string& businessId, bool useMaster, const long timeout);
    KeySeq produceKeys(const std::string& beginKey, int limit, const std::string& businessId, bool hotKey, const long timeout);

    private:
    DataMap produceUseJava(const KeySeq& keys, const std::string& businessId, bool useMaster, const long timeout);
    DataMap produceUseCc(const KeySeq& keys, const std::string& businessId, bool useMaster, const long timeout);

    KeySeq produceUseJava(const std::string& beginKey, int limit, const std::string& businessId, bool hotKey, const long timeout);
    KeySeq produceUseCc(const std::string& beginKey, int limit, const std::string& businessId, bool hotKey, const long timeout);
    
    ProducerManagerConfigClientPtr getConfig();

    private:
    std::map<std::string, std::string> prxToccOrjavaMap_;
    boost::shared_mutex prxToccOrjavaMapMutex_;
    
    ProducerManagerConfigClientPtr config_;
    boost::shared_mutex configMutex_;
    std::string zkAddress_;

};

}
}
}
#endif
