#ifndef ADENGINEDEBUGI_H_
#define ADENGINEDEBUGI_H_

#include "AdEngineDebug.h"
#include "Singleton.h"

namespace xce {
namespace ad {

class AdEngineDebugI : public AdEngineDebug, public MyUtil::Singleton<AdEngineDebugI> {
public:
    virtual ::MyUtil::LongSeq UrlMaping(const ::std::string&, ::Ice::Int, const ::Ice::Current& = ::Ice::Current());
};

}
}
#endif
