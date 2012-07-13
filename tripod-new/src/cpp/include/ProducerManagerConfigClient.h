#ifndef _TRIPOD_PRODUCERMANAGERCONFIGCLIENT_H__
#define _TRIPOD_PRODUCERMANAGERCONFIGCLIENT_H__

#include <TripodProducerManager.h>

#include <Common.h>
#include <HashHolder.hpp>
#include <ConfigClient.h>
#include <Ice/Ice.h>

namespace com {
namespace renren {
namespace tripod {

class ProducerManagerConfigClient : public ConfigClient {
    friend class ConfigClientFactory;
    protected:
    ProducerManagerConfigClient(const std::string& zkAddress);

    public:
    virtual bool update(const ConfigDataPtr& newConfig);

    public:
    ProducerManagerPrx getJavaPrx();
    ProducerManagerPrx getCcPrx();
    
    std::map<std::string, ProducerManagerPrx> getOriginalPrxMap();

    private:
    Ice::CommunicatorPtr ic_;

    HashHolder<ProducerManagerPrx> javaPrxHashHolder_;
    HashHolder<ProducerManagerPrx> ccPrxHashHolder_;

};

}
}
}
#endif
