#ifndef ADMONITORI_H
#define ADMONITORI_H

#include "AdEngine.h"
//#include <IceUtil/Shared.h>
//#include <IceUtil/Handle.h>
#include <string>
#include "Singleton.h"
//#include "../../util/src/AdCommonUtil.h"
//#include "AdConfig.h"

namespace xce {
namespace ad {

class AdMonitorI : public AdMatchMonitor, public MyUtil::Singleton<AdMonitorI> {
public:
  virtual ::std::string MemberStatus(::Ice::Long, const ::Ice::Current& = ::Ice::Current());
  virtual ::std::string CampaignStatus(::Ice::Long, const ::Ice::Current& = ::Ice::Current());
  virtual ::std::string GroupStatus(::Ice::Long, const ::Ice::Current& = ::Ice::Current());
  virtual ::std::string CompeteRef(::Ice::Long, const ::Ice::Current& = ::Ice::Current());
  virtual ::std::string RotationCache(::Ice::Long, const ::Ice::Current& = ::Ice::Current());
};

}
}

#endif //ADMONITORI_H
