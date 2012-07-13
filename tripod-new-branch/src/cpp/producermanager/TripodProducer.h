#ifndef _TRIPOD_PRODUCER_H__
#define _TRIPOD_PRODUCER_H__

#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace com {
namespace renren {
namespace tripod {


class TripodProducer {
public:
  virtual std::map<std::string, std::string> create(const std::vector<std::string>& keys, bool isMaster) = 0;
  virtual std::vector<std::string> createHotKeys(const std::string& beginKey, int limit) = 0;
  virtual std::vector<std::string> createKeys(const std::string& beginKey, int limit) = 0;
};

typedef boost::shared_ptr<TripodProducer> TripodProducerPtr;


}
}
}

#endif
