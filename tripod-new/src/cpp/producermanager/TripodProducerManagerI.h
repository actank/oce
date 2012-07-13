#ifndef _TRIPOD_PRODUCERMANAGER_H__
#define _TRIPOD_PRODUCERMANAGER_H__

#include "TripodProducerManager.h"
#include "TripodProducerFactory.h"
#include "CallFilter.h"
#include <ServiceI.h>

namespace com {
namespace renren {
namespace tripod {

class TripodProducerManagerI : virtual public ProducerManager, virtual public MyUtil::Singleton<TripodProducerManagerI>{
public:
  virtual DataMap produce(const KeySeq& keys, 
    const std::string& business, bool isMaster, const Ice::Current& c);
  
  virtual KeySeq produceKeys(const std::string& beginKey, int limit, 
    const std::string& business, bool hotKey, const Ice::Current& c);

private:
  FilterHolder filterHolder_;
  TripodProducerFactory factory_;
};

}
}
}

#endif
