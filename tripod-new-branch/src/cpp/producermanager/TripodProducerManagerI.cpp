#include "TripodProducerManagerI.h"
#include "TripodProducer.h"
#include <Monitor/client/service/ServiceMonitor.h>

void MyUtil::initialize() {
    MyUtil::ServiceI& service = MyUtil::ServiceI::instance();
  service.getAdapter()->add(&com::renren::tripod::TripodProducerManagerI::instance(), service.createIdentity("M", ""));
}

namespace com {
namespace renren {
namespace tripod {

std::map<string, string> TripodProducerManagerI::produce(const std::vector<std::string>& keys, const std::string& business, bool isMaster, const Ice::Current& current) {
  std::ostringstream os;
  os<<"Key : ";
  for(KeySeq::const_iterator it = keys.begin(); it != keys.end(); ++it) {
      os<<*it<<" ";
  }
  os<<" business :"<<business;
  os<<" isMaster :"<<isMaster;
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
 
  CallFilter cf(filterHolder_, business);
  if(!cf.filter()) {
      return std::map<std::string, std::string>();
  }
 
  TripodProducerPtr producer = TripodProducerFactory::get(business);
  if (producer == NULL) {
    throw TripodProducerNotSupportedException();
  }
  return producer->create(keys, isMaster);
}

std::vector<string> TripodProducerManagerI::produceKeys(const std::string& beginKey, int limit, const std::string& business, bool hotKey, const Ice::Current& current) {
  std::ostringstream os;
  os<<"beginKey : "<< beginKey << " limit : " << limit << " business : " << business << " hotKey : " << hotKey;
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  
  CallFilter cf(filterHolder_, business);
  if(!cf.filter()) {
      return std::vector<std::string>();
  }
 
  TripodProducerPtr producer = TripodProducerFactory::get(business);
  if (producer == NULL) {
    throw TripodProducerNotSupportedException();
  }
  if(hotKey) {
      return producer->createHotKeys(beginKey, limit);
  }else {
      return producer->createKeys(beginKey, limit);
  }
}


}
}
}

int main() {
  return 0;
}
