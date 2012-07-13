#ifndef _TRIPOD_PRODUCERFACTORY_H__
#define _TRIPOD_PRODUCERFACTORY_H__

#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "TripodProducer.h"

namespace com {
namespace renren {
namespace tripod {

class TripodProducerFactory {
private:

  std::map<std::string, TripodProducerPtr> map_;
public:
  TripodProducerFactory();
  TripodProducerPtr get(const std::string& businessId);
};

}
}
}

#endif
