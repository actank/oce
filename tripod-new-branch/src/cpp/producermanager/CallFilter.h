#ifndef _TRIPOD_CALLFILTER_H__
#define _TRIPOD_CALLFILTER_H__

#include <map>
#include <ServiceI.h>

namespace com {
namespace renren {
namespace tripod {

class CountPair : public IceUtil::Mutex, public Ice::Object {
    public:
        CountPair(int maxCount);
    
    public:
        int maxCount_;
        int curCount_;

    public:
        bool inc();
        void dec();
};

typedef IceUtil::Handle<CountPair> CountPairPtr;

class FilterHolder {
    public:
        FilterHolder();
    public:
        std::map<std::string, CountPairPtr> filterMap_;
};

class CallFilter {
    public:
        CallFilter(const FilterHolder& filterHolder, const std::string& businessId);
        bool filter();
        ~CallFilter();

    private:
        FilterHolder filterHolder_;
        std::string businessId_;
};

}
}
}

#endif
