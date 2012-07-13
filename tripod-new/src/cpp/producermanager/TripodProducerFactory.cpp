#include "TripodProducerFactory.h"
#include "SearchTripodProducer.h"
#include "BuddyByIdCacheProducer.h"

namespace com {
namespace renren {
namespace tripod {

TripodProducerFactory::TripodProducerFactory() {
    map_.insert(make_pair("SearchCache", new com::renren::searchtripod::SearchTripodProducer));
    map_.insert(std::make_pair("buddybyidcache", new xce::buddy::BuddyByIdCacheProducer));
}

TripodProducerPtr TripodProducerFactory::get(const std::string& businessId) {
    std::map<std::string, TripodProducerPtr>::const_iterator findIt = map_.find(businessId);
    if(findIt != map_.end()) {
        return findIt->second;
    }
    return TripodProducerPtr();
}

}}} //com::renren::tripod

