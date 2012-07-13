#ifndef _GOOGLE_AD_HANDLER_H_
#define _GOOGLE_AD_HANDLER_H_

#include "MatchHandlers.h"

namespace xce {
namespace ad {

class GoogleAdHandler : public AdMatchHandler {
public:
  virtual bool handle(MatchHandlerParamter& para, AdResultMap& result);
};

}
}

#endif // _GOOGLE_AD_HANDLER_H_
