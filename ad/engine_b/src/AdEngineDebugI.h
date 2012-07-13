#ifndef AD_ENGINE_DEBUG_H
#define AD_ENGINE_DEBUG_H

#include <string>
#include "Singleton.h"
#include "AdEngineDebug.h"

namespace xce {
namespace ad {

class AdEngineDebugI : public AdEngineDebug, public MyUtil::Singleton<AdEngineDebugI> {
public:
  virtual ::MyUtil::LongSeq UrlMaping(const ::std::string&, ::Ice::Int, const ::Ice::Current& = ::Ice::Current());  
};

}
}

#endif //AD_ENGINE_DEBUG_H
