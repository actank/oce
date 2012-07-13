#ifndef _FEED_GATEI_H_
#define _FEED_GATEI_H_

#include <ctemplate/template.h>
#include <boost/lexical_cast.hpp>

#include "Notify.h"
#include "RFeed.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"

namespace xce {
namespace notify {

using namespace MyUtil;
using namespace com::xiaonei::xce;

class RmessageGateI : public FeedGate, public MyUtil::Singleton<RmessageGateI> {
public:
  RmessageGateI() {}
};

}
}

#endif // _FEED_GATEI_H_

