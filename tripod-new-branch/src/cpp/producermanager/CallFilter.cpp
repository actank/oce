#include "CallFilter.h"

namespace com {
namespace renren {
namespace tripod {

CountPair::CountPair(int maxCount) : curCount_(0), maxCount_(maxCount) {
}

bool CountPair::inc() {
    IceUtil::Mutex::Lock lock(this);
    return ++curCount_ <= maxCount_;
}

void CountPair::dec() {
    IceUtil::Mutex::Lock lock(this);
    --curCount_;
}

FilterHolder::FilterHolder() {
    //add business config here
}

CallFilter::CallFilter(const FilterHolder& filterHolder, const std::string& businessId) :
                        filterHolder_(filterHolder), businessId_(businessId) {
}

bool CallFilter::filter() {
    std::map<std::string, CountPairPtr>::iterator findIt = filterHolder_.filterMap_.find(businessId_);
    if(findIt == filterHolder_.filterMap_.end()) {
        return false;
    }

    CountPairPtr counter = findIt->second;
    if(counter->inc()) {
        return true;
    }else {
            std::ostringstream os;
            os << "CallFilter::filter too many calls for business " << businessId_ << " max is " << counter->maxCount;
            MCE_WARN(os.str());
            return false;
    }
}

CallFilter::~CallFilter() {
    std::map<std::string, CountPairPtr>::iterator findIt = filterHolder_.filterMap_.find(businessId_);
    if(findIt == filterHolder_.filterMap_.end()) {
        return;
    }

    CountPairPtr counter = findIt->second;

    counter->dec();
}

}
}
}
