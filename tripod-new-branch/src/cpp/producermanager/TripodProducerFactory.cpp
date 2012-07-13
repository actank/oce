#include "TripodProducerFactory.h"

namespace com {
namespace renren {
namespace tripod {

std::map<std::string, TripodProducerPtr> TripodProducerFactory::map_;

TripodProducerFactory::TripodProducerFactory() {
}

TripodProducerPtr TripodProducerFactory::get(const std::string& businessId) {
    std::map<std::string, TripodProducerPtr>::const_iterator findIt = map_.find(businessId);
    if(findIt != map_.end()) {
        return findIt->second;
    }
    return TripodProducerPtr();
}

}
}
}
