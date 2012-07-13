#include "AdEngineDebugI.h"
#include "AdStruct.h"
#include "IceLogger.h"

namespace xce {
namespace ad {

using namespace std;
using namespace MyUtil;

LongSeq AdEngineDebugI::UrlMaping(const string& referer, Ice::Int stage, const Ice::Current&) {
  MCE_DEBUG("AdEngineDebugI::UrlMaping --> referer = " << referer << ", stage = " << stage);
  LongSeq result;
  UserProfile dummy;
  dummy.set_stage(stage);
  AdZoneDict dict = AdZonePool::instance().GetAdZoneOrdered(referer, stage);
  for(AdZoneDict::iterator it = dict.begin(); it != dict.end(); ++it) {
    result.push_back(it->second->id());
  }
  for(LongSeq::iterator it = result.begin(); it != result.end(); ++it) {
    MCE_DEBUG("AdEngineDebugI::UrlMaping --> adzone_id = " << *it);
  }
  return result;
}

}
}
