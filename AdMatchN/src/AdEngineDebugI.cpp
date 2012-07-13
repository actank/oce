#include "AdEngineDebugI.h"
#include "AdStruct.h"
#include "IceLogger.h"

namespace xce {
namespace ad {

MyUtil::LongSeq AdEngineDebugI::UrlMaping(const ::std::string& url, ::Ice::Int stage, const ::Ice::Current&) {
  AdZoneDict adzones = AdZonePool::instance().GetAdZoneOrdered2(url, stage);
  MyUtil::LongSeq result;
  MCE_WARN("[UrlMaping] url = " << url << ", stage = " << stage);
  for(AdZoneDict::iterator it = adzones.begin(); it != adzones.end(); ++it) {
    Ice::Long zone = it->second->id();
    MCE_WARN("[UrlMaping] adzone_id = " << zone);
    result.push_back(zone);
  }
  return result;
}

}
}
