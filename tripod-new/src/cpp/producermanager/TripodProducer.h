#ifndef _TRIPOD_PRODUCER_H__
#define _TRIPOD_PRODUCER_H__

#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <TripodData.h>

namespace com {
namespace renren {
namespace tripod {


class TripodProducer {
public:
  virtual DataMap create(const KeySeq& keys, bool isMaster) = 0;
  virtual KeySeq createHotKeys(const std::string& beginKey, int limit) = 0;
  virtual KeySeq createKeys(const std::string& beginKey, int limit) = 0;
  virtual ~TripodProducer() {}
};

typedef boost::shared_ptr<TripodProducer> TripodProducerPtr;


}
}
}

#endif
